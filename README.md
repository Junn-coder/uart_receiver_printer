# uart_receiver_printer

0) Install virtualBox 7.0 with two virtual machines on Windows11.

ubuntu virtual machine 1

Distributor ID:	Ubuntu

Description:	Ubuntu 14.04.6 LTS

Release:	14.04

Codename:	trusty

I can run sudo ./pacific_scales_simulator.py on vm 1 after install python2. 


ubuntu virtual machine 2

Distributor ID:	Ubuntu

Description:	Ubuntu 20.04.6 LTS

Release:	20.04

Codename:	focal

1. Install softwares 

Install boost_1_75_0 and g++ compiler on ubuntu virtual machine 2

cd /tmp

wget https://boostorg.jfrog.io/artifactory/main/release/1.75.0/source/boost_1_75_0.tar.gz

tar -xvzf boost_1_75_0.tar.gz

cd boost_1_75_0

sudo ./bootstrap.sh --prefix=/usr/local

sudo ./b2 install

#install g++ compile enviroments

sudo apt update

sudo apt install build-essential

sudo snap install cmake


2) Checkout the source codes which include CMakeLists.txt  uart_receiver_printer.cpp.
   
mkdir receiver

cp CMakeLists.txt  uart_receiver_printer.cpp ./receiver/

cd receiver

mkdir build

cd build

cmake ..

make 

#do make and compile uart_receiver_printer.cpp to get bin file: uart_receiver_printer

in build file folder, there should be like: MakeCache.txt  CMakeFiles  cmake_install.cmake  Makefile  uart_receiver_printer

3) Testing
   
*) Setup two Virtual machines communicating each other by virtual UART.

Open VirtualBox Manager.

Go to VM Settings > Serial Ports.

Set Port Mode to Host Pipe.

Use the path \\.\pipe\virtualport1 for the named pipe.

Ensure the Create Pipe option is checked.

Apply changes above and start your two VMs.


in VM1

#The dialout group in Linux is commonly associated with serial port access. Adding your user to this group.

sudo usermod -aG dialout $USER or sudo chmod 666 /dev/ttyS0

log out and log in

sudo socat PTY,link=/dev/ttyS0,raw TCP-LISTEN:1234

launch another console and run:

sudo ./pacific_scales_simulator.py

#I added print(output_stream) in form_scales_pkt() in pacific_scales_simulator.py. So,pacific_scales_simulator.py can print out the packet content. 

keven@keven-VirtualBox:/media/sf_myzone/mtdata/PacificScales_Simulator_1.0$ sudo ./pacific_scales_simulator.py

[sudo] password for keven: 

Serial Port          : /dev/ttyS0

Baud rate            : 9600

Simulator Mode       : loop

Simulator Periodicity: 2

Number of channels   : 6

Input CSV File       : mass_test.csv


/
A    :   5200 Kg
B    :  17200 Kg
C    :  22200 Kg
D    :  15200 Kg
E    :  19200 Kg
F    :  11200 Kg
TOTAL:  90200 Kg
\

/
A    :   5300 Kg
B    :  17300 Kg
C    :  22300 Kg
D    :  15300 Kg
E    :  19300 Kg
F    :  11300 Kg
TOTAL:  90800 Kg
\


On VM2, do commands as below:

sudo usermod -aG dialout $USER or sudo chmod 666 /dev/ttyS1

log out and log in

sudo socat PTY,link=/dev/ttyS1,raw TCP:10.0.0.10:1234   ###note:10.0.0.10 is vm1's IP address. VM2 can successfully ping VM1 in the testing.

#launch another console:

sudo ./uart_receiver_printer /dev/ttyS1 9600


[19:11:10] [{"A    ":5200,"B    ":17200,"C    ":22200,"D    ":15200,"E    ":19200,"F    ":11200,"TOTAL":90200,"VALID":false},{"A    ":5000,"B    ":17000,"C    ":22000,"D    ":15000,"E    ":19000,"F    ":11000,"TOTAL":89000,"VALID":true},{"A    ":5100,"B    ":17100,"C    ":22100,"D    ":15100,"E    ":19100,"F    ":11100,"TOTAL":89600,"VALID":true},{"A    ":5200,"B    ":17200,"C    ":22200,"D    ":15200,"E    ":19200,"F    ":11200,"TOTAL":90200,"VALID":true},{"A    ":5300,"B    ":17300,"C    ":22300,"D    ":15300,"E    ":19300,"F    ":11300,"TOTAL":90800,"VALID":true},{"A    ":5400,"B    ":17400,"C    ":22400,"D    ":15400,"E    ":19400,"F    ":11400,"TOTAL":91400,"VALID":true},{"A    ":5500,"B    ":17500,"C    ":22500,"D    ":15500,"E    ":19500,"F    ":11500,"TOTAL":92000,"VALID":true},{"A    ":5600,"B    ":17600,"C    ":22600,"D    ":15600,"E    ":19600,"F    ":11600,"TOTAL":92600,"VALID":true},{"A    ":5700,"B    ":17700,"C    ":22700,"D    ":15700,"E    ":19700,"F    ":11700,"TOTAL":93200,"VALID":true},{"A    ":5800,"B    ":17800,"C    ":22800,"D    ":15800,"E    ":19800,"F    ":11800,"TOTAL":93800,"VALID":true},{"A    ":5900,"B    ":17900,"C    ":22900,"D    ":15900,"E    ":19900,"F    ":11900,"TOTAL":94400,"VALID":true},{"A    ":6000,"B    ":18000,"C    ":23000,"D    ":16000,"E    ":20000,"F    ":12000,"TOTAL":95000,"VALID":true},{"A    ":7000,"B    ":19000,"C    ":28000,"D    ":17000,"E    ":22000,"F    ":12000,"TOTAL":105000,"VALID":true},{"A    ":5000,"B    ":17000,"C    ":22000,"D    ":15000,"E    ":19000,"F    ":11000,"TOTAL":89000,"VALID":true},{"A    ":5100,"B    ":17100,"C    ":22100,"D    ":15100,"E    ":19100,"F    ":11100,"TOTAL":89600,"VALID":true},{"A    ":5200,"B    ":17200,"C    ":22200,"D    ":15200,"E    ":19200,"F    ":11200,"TOTAL":90200,"VALID":true},{"A    ":5300,"B    ":17300,"C    ":22300,"D    ":15300,"E    ":19300,"F    ":11300,"TOTAL":90800,"VALID":true},{"A    ":5400,"B    ":17400,"C    ":22400,"D    ":15400,"E    ":19400,"F    ":11400,"TOTAL":91400,"VALID":true},{"A    ":5500,"B    ":17500,"C    ":22500,"D    ":15500,"E    ":19500,"F    ":11500,"TOTAL":92000,"VALID":true}]

[19:11:20] [{"A    ":5600,"B    ":17600,"C    ":22600,"D    ":15600,"E    ":19600,"F    ":11600,"TOTAL":92600,"VALID":true},{"A    ":5700,"B    ":17700,"C    ":22700,"D    ":15700,"E    ":19700,"F    ":11700,"TOTAL":93200,"VALID":true},{"A    ":5800,"B    ":17800,"C    ":22800,"D    ":15800,"E    ":19800,"F    ":11800,"TOTAL":93800,"VALID":true},{"A    ":5900,"B    ":17900,"C    ":22900,"D    ":15900,"E    ":19900,"F    ":11900,"TOTAL":94400,"VALID":true},{"A    ":6000,"B    ":18000,"C    ":23000,"D    ":16000,"E    ":20000,"F    ":12000,"TOTAL":95000,"VALID":true},{"A    ":7000,"B    ":19000,"C    ":28000,"D    ":17000,"E    ":22000,"F    ":12000,"TOTAL":105000,"VALID":true},{"A    ":5000,"B    ":17000,"C    ":22000,"D    ":15000,"E    ":19000,"F    ":11000,"TOTAL":89000,"VALID":true},{"A    ":5100,"B    ":17100,"C    ":22100,"D    ":15100,"E    ":19100,"F    ":11100,"TOTAL":89600,"VALID":true},{"A    ":5200,"B    ":17200,"C    ":22200,"D    ":15200,"E    ":19200,"F    ":11200,"TOTAL":90200,"VALID":true},{"A    ":5300,"B    ":17300,"C    ":22300,"D    ":15300,"E    ":19300,"F    ":11300,"TOTAL":90800,"VALID":true},{"A    ":5400,"B    ":17400,"C    ":22400,"D    ":15400,"E    ":19400,"F    ":11400,"TOTAL":91400,"VALID":true},{"A    ":5500,"B    ":17500,"C    ":22500,"D    ":15500,"E    ":19500,"F    ":11500,"TOTAL":92000,"VALID":true},{"A    ":5600,"B    ":17600,"C    ":22600,"D    ":15600,"E    ":19600,"F    ":11600,"TOTAL":92600,"VALID":true},{"A    ":5700,"B    ":17700,"C    ":22700,"D    ":15700,"E    ":19700,"F    ":11700,"TOTAL":93200,"VALID":true},{"A    ":5800,"B    ":17800,"C    ":22800,"D    ":15800,"E    ":19800,"F    ":11800,"TOTAL":93800,"VALID":true},{"A    ":5900,"B    ":17900,"C    ":22900,"D    ":15900,"E    ":19900,"F    ":11900,"TOTAL":94400,"VALID":true},{"A    ":6000,"B    ":18000,"C    ":23000,"D    ":16000,"E    ":20000,"F    ":12000,"TOTAL":95000,"VALID":true},{"A    ":7000,"B    ":19000,"C    ":28000,"D    ":17000,"E    ":22000,"F    ":12000,"TOTAL":105000,"VALID":true},{"A    ":5000,"B    ":17000,"C    ":22000,"D    ":15000,"E    ":19000,"F    ":11000,"TOTAL":89000,"VALID":true}]


Note: 

1) In the testing, the uart baudrate on both VMs are set 9600. You can use stty to double check tty.

For VM1: stty -F /dev/ttyS0 -a

For VM2: stty -F /dev/ttyS1 -a

2) A quick way to test UART communication on both VMs.
   
in VM1: 

echo "Hello from VM1" | sudo tee /dev/ttyS0 > /dev/null

in VM2: 

cat /dev/ttyS1

#use ttyS1, not ttyS0, cat command in VM2 should print what string is echoed in VM1.



