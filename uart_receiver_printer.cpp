#include <iostream>
#include <string>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <boost/chrono.hpp>

using namespace std;
namespace json = boost::json;

std::queue<json::object> message_queue;
std::mutex queue_mutex;
bool uart_reading_failed = false;

// Function to read data from UART
std::string read_uart(boost::asio::serial_port& serial) {
    boost::asio::streambuf buf;
    try {
        boost::asio::read_until(serial, buf, "\\");
    } catch (boost::system::system_error& e) {
        uart_reading_failed = true;
        std::cerr << "UART reading failed: " << e.what() << std::endl;
        return "";
    }
    std::istream is(&buf);
    std::string data((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
    return data;
}


boost::json::object parse_data_to_json(std::string& buffer) {
    boost::json::object json_obj;
    std::istringstream iss(buffer);
    std::string line;
    int total_calculated = 0;
    int total_reported = 0;
    bool total_found = false;

    while (std::getline(iss, line)) {
        // Remove leading and trailing whitespace
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;  // Skip empty lines

        // Split the line into key and value
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value_str = line.substr(colon_pos + 1);
            value_str.erase(0, value_str.find_first_not_of(" \t"));

            if (key == "TOTAL") {
                total_found = true;
                total_reported = std::stoi(value_str.substr(0, value_str.find(' ')));
                json_obj[key] = total_reported;
            } else {
                int value = std::stoi(value_str.substr(0, value_str.find(' ')));
                json_obj[key] = value;
                total_calculated += value;
            }
        }
    }

    // Only add the VALID field if TOTAL was found
    if (total_found) {
        json_obj["VALID"] = (total_calculated == total_reported);
    }

    return json_obj;
}

void process_uart_data(const std::string& data, std::string& buffer, std::queue<boost::json::object>& json_queue) {
    // Append new data to buffer
    buffer += data;

    size_t package_start = buffer.find('/');
    size_t package_end = buffer.find('\\');

    while (package_start != std::string::npos && package_end != std::string::npos) {
        // Extract the complete package
        std::string package = buffer.substr(package_start + 1, package_end - package_start - 1);

        // Parse the complete package and push it to the queue
        boost::json::object json_obj = parse_data_to_json(package);
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            json_queue.push(json_obj);
        }

        // Remove the processed package from the buffer
        buffer.erase(0, package_end + 1);

        // Look for the next package in the remaining buffer
        package_start = buffer.find('/');
        package_end = buffer.find('\\');
    }
    // At this point, buffer may contain an incomplete package that will be processed later
}

// Function to read from UART and enqueue the data
void uart_read_task(boost::asio::serial_port& serial) {
    std::cout << "This is uart read task" << endl;
    std::string buffer;  // Buffer to store incomplete data
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        // Read data from UART
        std::string uart_data = read_uart(serial);
        if (!uart_data.empty()) {
            // Process the data, buffer incomplete parts, and push complete JSON objects to the queue
            process_uart_data(uart_data, buffer, message_queue);
        }
    }
}


// Function to calculate the next 10-second boundary
std::chrono::system_clock::time_point get_next_boundary() {
    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    auto next_boundary_seconds = (seconds / 10 + 1) * 10;
    auto next_boundary = std::chrono::system_clock::time_point(std::chrono::seconds(next_boundary_seconds));
    return next_boundary;
}


// Function to print data from the queue every 10 seconds
void print_task() {
    std::cout << "This is print task" << endl;
    while (true) {
        auto next_boundary = get_next_boundary();
        std::this_thread::sleep_until(next_boundary); // Wait until the next boundary

        // Get the current time to print
        std::time_t current_time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm* local_time = std::localtime(&current_time);

        char time_buffer[10];
        std::strftime(time_buffer, sizeof(time_buffer), "%T", local_time); // Format time as HH:MM:SS
        std::lock_guard<std::mutex> lock(queue_mutex);
        if (!message_queue.empty()) {
            json::array output_data;
            while (!message_queue.empty()) {
                output_data.push_back(message_queue.front());
                message_queue.pop();
            }
            std::cout << "\n[" << time_buffer << "] " << json::serialize(output_data) << std::endl;
        } else if (uart_reading_failed) {
            std::cerr << "\n[" << time_buffer << "] No data to print. UART reading failed." << std::endl;
        } else {
            std::cerr << "\n[" << time_buffer << "] No data to print." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <device> <baudrate>" << std::endl;
        return 1;
    }

    std::string device = argv[1];
    int baudrate = std::stoi(argv[2]);
    std::cout << "setup device=" << device << " baudrate=" << baudrate << endl;

    try {
        boost::asio::io_service io_service;
        boost::asio::serial_port serial(io_service, device);
        serial.set_option(boost::asio::serial_port_base::baud_rate(baudrate));
        serial.set_option(boost::asio::serial_port_base::character_size(8));
        serial.set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
        serial.set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
        serial.set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));

        // Start UART reading and printing tasks
        std::thread uart_thread(uart_read_task, std::ref(serial));
        std::thread print_thread(print_task);

        uart_thread.join();
        print_thread.join();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}

