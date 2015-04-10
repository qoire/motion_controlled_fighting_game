`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2014/05/23 16:24:31
// Design Name: 
// Module Name: ov7725_top
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module ov7670_top(
input FSM_CLK,
input clk25,
input  OV7670_VSYNC,
input  OV7670_HREF,
input  OV7670_PCLK,
output OV7670_XCLK,
output OV7670_SIOC,
inout  OV7670_SIOD,
input [7:0] OV7670_D,

output EOL,
output FSYNC,
input READY,
output TVALID,
output [31:0] data_32,

input BTNC,
output pwdn,
output reset
);
wire [16:0] frame_addr;
wire [16:0] capture_addr;      
wire  resend;
  
assign pwdn = 0;
assign reset = 1;
assign  	OV7670_XCLK = clk25;

// The button (BTNC) is used to resend the configuration bits to the camera.
// The button is debounced with a 50 MHz clock
debounce   btn_debounce(
		.clk(clk25),
		.i(BTNC),
		.o(resend)
);


// BRAM using memory generator from IP catalog
// dual-port, 16 bits wide, 76800 deep 



 ov7670_capture capture(
        .FSM_CLK (FSM_CLK),
 		.pclk  (OV7670_PCLK),
 		.vsync (OV7670_VSYNC),
 		.href  (OV7670_HREF),
 		.d     ( OV7670_D),
 		.dout( data_32),
 		.EOL(EOL),
 		.FSYNC(FSYNC),
 		.READY(READY),
 		.TVALID(TVALID)
 	);
 
I2C_AV_Config IIC(
 		.iCLK   ( clk25),    
 		.iRST_N (! resend),    
 		.Config_Done ( config_finished),
 		.I2C_SDAT  ( OV7670_SIOD),    
 		.I2C_SCLK  ( OV7670_SIOC),
 		.LUT_INDEX (),
 		.I2C_RDATA ()
 		); 


// Derive two clocks for the board provided 100 MHz clock.
// Generated using clock wizard in IP Catalog

endmodule
