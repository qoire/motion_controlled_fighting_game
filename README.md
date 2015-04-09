# Motion Controlled Fighting Game

### A. Description
The motion controlled fighting game is implemented on the Nexys 4 DDR board using OV7670 camera as its only peripheral, its hardware components are implemented in verilog using Vivado and hardware components implemented in C using the Vivado SDK. It is a one player fighting game where the user the player fights against a computer opponent via motion controlled inputs captured from the OV7670 camera. The goal of the project is to implement a simple colour based object tracking algorithm in hardware and communicate with the fighting game implemented in C on the microblaze processor.

### B. How to Use
To build the project yourself,

### F. Acknowledgements
The code for communication between the OV7670 pmod camera and the Xilinx VDMA IP Core is a derivative of a VHDL code found here:
http://lauri.xn--vsandi-pxa.com/hdl/zybo-ov7670-to-vga.html
The only changes made are the output format of the 32bit piece of data from RGBA (8:8:8:8) to RGB444
