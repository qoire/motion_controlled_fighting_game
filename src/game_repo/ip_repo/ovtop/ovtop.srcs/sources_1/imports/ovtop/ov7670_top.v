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
input clk25,
input  OV7670_VSYNC,
input  OV7670_HREF,
input  OV7670_PCLK,
output OV7670_XCLK,
output OV7670_SIOC,
inout  OV7670_SIOD,
input [7:0] OV7670_D,

output m_axis_tlast,
output m_axis_tuser,
input m_axis_tready,
output m_axis_tvalid,
output [31:0] m_axis_tdata,

input BTNC,
output pwdn,
output reset
);    
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
 	
ov7670_axi_stream_capture capture(
 		.pclk  (OV7670_PCLK),
        .vsync (OV7670_VSYNC),
        .href  (OV7670_HREF),
        .d     (OV7670_D),
        .m_axis_tdata(m_axis_tdata),
        .m_axis_tlast(m_axis_tlast),
        .m_axis_tuser(m_axis_tuser),
        .m_axis_tready(m_axis_tready),
        .m_axis_tvalid(m_axis_tvalid)        
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
