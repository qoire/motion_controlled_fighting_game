`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2014/05/23 15:11:30
// Design Name: 
// Module Name: ov7725_capture
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


module ov7670_capture(
input FSM_CLK,
input pclk,
input vsync,
input href,
input[7:0] d,
output reg[31:0] dout,
output reg EOL,
output reg FSYNC,
input READY,
output reg TVALID
    );

// new fsm definitions
reg r_EOL;
reg r_FSYNC;
reg r_TVALID;
reg [31:0] r_dout;

reg [15:0] dat_latch, r_dat_latch;

//
localparam [5:0]
    s_idle = 0,
    s_fsf = 1,
    s_fsn = 2,
    s_eolf = 3,
    s_eoln = 4,
    s_framef = 5,
    s_framen = 6;
    
reg [5:0] s, ns;
reg [31:0] ctr, r_ctr;

reg href_pulse;
reg pulse_hold;

//set to 0 initially
initial pulse_hold = 0;



// Pulse FSM
//sample signals
always @ (posedge FSM_CLK) begin
    if ((href == 1) && (pulse_hold == 0)) begin
        href_pulse <= 1;
    end
    else begin
        pulse_hold <= href;
    end 
end


always@(posedge FSM_CLK)begin
    s <= ns;
    r_ctr <= ctr;
    r_EOL <= EOL;
    r_FSYNC <= FSYNC;
    r_TVALID <= TVALID;
    r_dat_latch <= dat_latch;
    r_dout <= dout;
    
    //pulse FSM
end

always@(*) begin

    //defaults
    EOL <= r_EOL;
    TVALID <= r_TVALID;
    FSYNC <= r_FSYNC;
    dout <= r_dout;
    ctr <= r_ctr;
    dat_latch <= r_dat_latch;

    case (s) 
        s_idle: begin
            if (vsync == 1 && READY == 1) begin
                ns <= s_fsf;
            end
            else begin
                EOL <= 0;
                FSYNC <= 0;
                TVALID <= 0;
                dat_latch <= 0;
                ctr <= 0;
                ns <= s_idle;
            end
        end
        s_fsf: begin
            TVALID <= 0;
            FSYNC <= 0;
            EOL <= 0;
            
            if (href_pulse == 1 && READY == 1) begin
                dat_latch[7:0] <= d;
                ns <= s_fsn;
            end
            else
                ns <= s_fsf;
        end
        s_fsn: begin
            ctr <= r_ctr + 1; //increment the counter
            FSYNC <= 1; //set first frame
            TVALID <= 1; //indicate valid
            dout <= {dat_latch[7:0], d}; //give data on same frame
            ns <= s_framef;
        end
        s_framef: begin
            //kill fsync
            FSYNC <= 0;
            TVALID <= 0; //data not yet completed (valid)
            
            if (href_pulse == 1) begin
                dat_latch[7:0] <= d;
                ctr <= r_ctr + 1;
                ns <= s_framen;
            end
            else
                ns <= s_framef;
            
            if (vsync == 1) begin
                ns <= s_fsf;
            end
        end
        s_framen: begin
            TVALID <= 1;
            dout <= {dat_latch[7:0], d};

            if (ctr == 319)
                ns <= s_eoln;
            else
                ns <= s_framef;
        end
        s_eolf: begin
            //pixel 320, last pixel per row
            TVALID <= 0;
           
            if (href_pulse == 1 && READY == 1) begin
                dat_latch[7:0] <= d;
                ns <= s_eoln;
                ctr <= 0; //reset counter for next line
            end
            else
                ns <= s_eolf;
                
            if (vsync == 1) begin
                    ns <= s_fsf;
            end
        end
        s_eoln: begin
            TVALID <= 1;
            EOL <= 1;
            dout <= {dat_latch[7:0], d};
            ns <= s_framef;
        end
        default: begin
            ns <= s_fsf;
        end       
    endcase
end

endmodule

