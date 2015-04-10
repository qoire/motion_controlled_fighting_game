# Motion Controlled Fighting Game
-----------------------------------------------------
### A. Description
The Motion Controlled Punching Game is a _“Rock Em Sock Em”_ style punching game. A player contests against a CPU player. The punching characters are displayed on a VGA screen with the input from a camera in the corner of the screen. The player inputs are received by motion tracking of colored patches attached to the arm and outside palm of the user. The scoring system determines the distance between the two patches to determine the extension of the arm, and scales the damage according to the extension. The greater the extension the more damage is done.
A PMOD Camera is used to obtain visual input from the user. The camera operates at 320x240 pixels and its input is processed by an IP to track motion.

Hardware System (file is huge feel free to click in!):
![alt text][system_setup]
-----------------------------------------------------
### B. How to Use
We have included software, hardware, and the algorithm we intended to implement in MATLAB in the /src folder. 
If you wish to properly build the main system:
1. Navitage to /src/game_repo/cam_vga_full_test and open dm_test.xpr with Vivado
2. Generate bitstream with Vivado and export to SDK
3. Within the SDK there could be multiple test projects, open up test1 and the associated support package (test1_bsd). All software source code should be present inside helloworld.c

To view documents support/describing the system:
1. Navigate to docs, look for final_report.pdf, ECE532_presentation_slides.pdf
-----------------------------------------------------
### F. Acknowledgements
Parts of the OV7670_top module was derived from Professor Chow's sample code (not included here)

The code for communication between the OV7670 pmod camera and the Xilinx VDMA IP Core is a derivative of a VHDL code found here:
[Code](http://lauri.xn--vsandi-pxa.com/hdl/zybo-ov7670-to-vga.html)
The only changes made are the output format of the 32bit piece of data from RGBA (8:8:8:8) to RGB444

[system_setup]: https://cloud.githubusercontent.com/assets/4521292/7080333/6a9e7392-defe-11e4-9a9e-a2a1cae01f35.png
-----------------------------------------------------
### G. Contributions
#### Group members:
Syed Talal Ashraf
Syed Muhammad Adnan Karim
Yao Sun