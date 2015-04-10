
`timescale 1 ps / 1 ps

    module delta_calc_v1_0_M00_AXI #
    (
        // Users to add parameters here
        parameter SCREEN_WIDTH = 1024,
        parameter SCREEN_HEIGHT = 768,
        parameter VIDEO_WIDTH = 320,
        parameter VIDEO_HEIGHT = 240,
        parameter BLOCK_WIDTH = 10,
        parameter BLOCK_HEIGHT = 10,
        parameter VIDEO_X_BLK_NUM = 32,
        parameter VIDEO_Y_BLK_NUM = 24,
        parameter READ_ADDR_OFFSET = 32'h00000000,
        parameter WRITE_ADDR_OFFSET = 32'h05000000,

        // User parameters ends
        // Do not modify the parameters beyond this line

        // The master will start generating data from the C_M_START_DATA_VALUE value
        parameter  C_M_START_DATA_VALUE	= 32'hAA000000,
        // The master requires a target slave base address.
        // The master will initiate read and write transactions on the slave with base address specified here as a parameter.
        parameter  C_M_TARGET_SLAVE_BASE_ADDR	= 32'h81000000,
        // Width of M_AXI address bus.
        // The master generates the read and write addresses of width specified as C_M_AXI_ADDR_WIDTH.
        parameter integer C_M_AXI_ADDR_WIDTH	= 32,
        // Width of M_AXI data bus.
        // The master issues write data and accept read data where the width of the data bus is C_M_AXI_DATA_WIDTH
        parameter integer C_M_AXI_DATA_WIDTH	= 32,
        // Transaction number is the number of write
        // and read transactions the master will perform as a part of this example memory test.
        parameter integer C_M_TRANSACTIONS_NUM	= 4
    )
    (
        // Users to add ports here
        input wire VSYNC,
        input wire module_init,
        output wire [31:0] w_bdata,
		input wire [31:0] set_rgb_r,
		input wire [31:0] set_rgb_g,
		input wire [31:0] set_rgb_b,
        output wire [31:0] red_xy,
        output wire [31:0] green_xy,
        output wire [31:0] system_done,
        
		
		// input wire RGB values
		

        // User ports ends
        // Do not modify the ports beyond this line

        // Initiate AXI transactions
        /*
        input wire  INIT_AXI_TXN,
        */
        // Asserts when ERROR is detected
        /*
        output reg  ERROR,
        */
        // Asserts when AXI transactions is complete
        /*
        output wire  TXN_DONE,
        */
        // AXI clock signal
        input wire  M_AXI_ACLK,
        // AXI active low reset signal
        input wire  M_AXI_ARESETN,
        // Master Interface Write Address Channel ports. Write address (issued by master)
        output wire [C_M_AXI_ADDR_WIDTH-1 : 0] M_AXI_AWADDR,
        // Write channel Protection type.
        // This signal indicates the privilege and security level of the transaction,
        // and whether the transaction is a data access or an instruction access.
        output wire [2 : 0] M_AXI_AWPROT,
        // Write address valid.
        // This signal indicates that the master signaling valid write address and control information.
        output wire  M_AXI_AWVALID,
        // Write address ready.
        // This signal indicates that the slave is ready to accept an address and associated control signals.
        input wire  M_AXI_AWREADY,
        // Master Interface Write Data Channel ports. Write data (issued by master)
        output wire [C_M_AXI_DATA_WIDTH-1 : 0] M_AXI_WDATA,
        // Write strobes.
        // This signal indicates which byte lanes hold valid data.
        // There is one write strobe bit for each eight bits of the write data bus.
        output wire [C_M_AXI_DATA_WIDTH/8-1 : 0] M_AXI_WSTRB,
        // Write valid. This signal indicates that valid write data and strobes are available.
        output wire  M_AXI_WVALID,
        // Write ready. This signal indicates that the slave can accept the write data.
        input wire  M_AXI_WREADY,
        // Master Interface Write Response Channel ports.
        // This signal indicates the status of the write transaction.
        input wire [1 : 0] M_AXI_BRESP,
        // Write response valid.
        // This signal indicates that the channel is signaling a valid write response
        input wire  M_AXI_BVALID,
        // Response ready. This signal indicates that the master can accept a write response.
        output wire  M_AXI_BREADY,
        // Master Interface Read Address Channel ports. Read address (issued by master)
        output wire [C_M_AXI_ADDR_WIDTH-1 : 0] M_AXI_ARADDR,
        // Protection type.
        // This signal indicates the privilege and security level of the transaction,
        // and whether the transaction is a data access or an instruction access.
        output wire [2 : 0] M_AXI_ARPROT,
        // Read address valid.
        // This signal indicates that the channel is signaling valid read address and control information.
        output wire  M_AXI_ARVALID,
        // Read address ready.
        // This signal indicates that the slave is ready to accept an address and associated control signals.
        input wire  M_AXI_ARREADY,
        // Master Interface Read Data Channel ports. Read data (issued by slave)
        input wire [C_M_AXI_DATA_WIDTH-1 : 0] M_AXI_RDATA,
        // Read response. This signal indicates the status of the read transfer.
        input wire [1 : 0] M_AXI_RRESP,
        // Read valid. This signal indicates that the channel is signaling the required read data.
        input wire  M_AXI_RVALID,
        // Read ready. This signal indicates that the master can accept the read data and response information.
        output wire  M_AXI_RREADY
    );

      // function called clogb2 that returns an integer which has the
      // value of the ceiling of the log base 2
      function integer clogb2 (input integer bit_depth);
          begin
              for(clogb2=0; bit_depth>0; clogb2=clogb2+1)
                  bit_depth = bit_depth >> 1;
          end
      endfunction

      // TRANS_NUM_BITS is the width of the index counter for
      // number of write or read transaction.
      localparam integer VIDEO_X_NUM_BITS = clogb2(VIDEO_WIDTH-1);
      localparam integer VIDEO_Y_NUM_BITS = clogb2(VIDEO_HEIGHT-1);
      localparam integer BLOCK_X_NUM_BITS = clogb2(VIDEO_X_BLK_NUM-1);
      localparam integer BLOCK_Y_NUM_BITS = clogb2(VIDEO_Y_BLK_NUM-1);

      // Example State machine to initialize counter, initialize write transactions,
      // initialize read transactions and comparison of read data with the
      // written data words.
      parameter [3:0] IDLE = 4'b0000, // This state initiates AXI4Lite transaction
            // after the state machine changes state to INIT_WRITE
            // when there is 0 to 1 transition on INIT_AXI_TXN
        INIT_WRITE   = 4'b0001, // This state initializes write transaction,
            // once writes are done, the state machine
            // changes state to INIT_READ
        REPEAT_WRITE = 4'b0010, // This state repeats write transaction
        INIT_READ = 4'b0011, // This state initializes read transaction
            // once reads are done, the state machine
            // changes state to INIT_COMPLETE
        REPEAT_READ = 4'b0100, // This state repeats read transaction
        INIT_READ_Y = 4'b0101,  // This state initializes a new horizontal line read transaction
            // once done, the state machine changes state to READ_Y
        READ_Y = 4'b0110,  // This state start a new horizontal line read transaction
            // once done, the state machine changes state to INIT_READ
        INIT_READ_BLOCK = 4'b0111,  // This state initializes a new read block transaction
            // once done, the state machine changes state to READ_BLOCK
        READ_BLOCK = 4'b1000,  // This state start a new read block transaction
            // once done, the state machine changes state to INIT_READ_Y
        INIT_READ_ROW = 4'b1001,  // This state initializes a new vertical read block transaction
            // once done, the state machine changes state to READ_ROW
        READ_ROW = 4'b1010,  // This state start a new vertical read block transaction
            // once done, the state machine changes state to INIT_COMPLETE
        INIT_COMPLETE = 4'b1111; // This state issues the status of comparison
            // of the written data with the read data

      reg [3:0] mst_exec_state;

      // AXI4LITE signals
      //write address valid
      reg  	axi_awvalid;
      //write data valid
      reg  	axi_wvalid;
      //read address valid
      reg  	axi_arvalid;
      //read data acceptance
      reg  	axi_rready;
      //write response acceptance
      reg   axi_bready;
      //write address
      reg [C_M_AXI_ADDR_WIDTH-1 : 0] 	axi_awaddr;
      //write data
      reg [C_M_AXI_DATA_WIDTH-1 : 0] 	axi_wdata;
      //read addresss
      reg [C_M_AXI_ADDR_WIDTH-1 : 0] 	axi_araddr;
      //Asserts when there is a write response error
      wire      write_resp_error;
      //Asserts when there is a read response error
      wire  	read_resp_error;
      //A pulse to initiate a write transaction
      reg  	start_single_write;
      //A pulse to initiate a read transaction
      reg   start_single_read;
      //A pulse to initiate a horizontal line read transaction
      reg   start_single_line;
      //A pulse to initiate a horizontal block read transaction
      reg   start_single_block;
      //A pulse to initiate a vertical block read transaction
      reg   start_single_row;
      //Asserts when a single beat write transaction is issued and remains asserted till the completion of write trasaction.
      reg   write_issued;
      //Asserts when a single beat read transaction is issued and remains asserted till the completion of read trasaction.
      reg   read_issued;
      //flag that marks the completion of write trasactions. The number of write transaction is user selected by the parameter C_M_TRANSACTIONS_NUM.
      reg   writes_done;
      //flag that marks the completion of read trasactions. The number of read transaction is user selected by the parameter C_M_TRANSACTIONS_NUM
      reg   reads_done;
      //The error register is asserted when any of the write response error, read response error or the data mismatch flags are asserted.
      reg   error_reg;
      //index counter to track the number of write transaction issued
      reg [VIDEO_X_NUM_BITS : 0]    write_index;
      //index counter to track the number of read transaction issued
      reg [VIDEO_X_NUM_BITS : 0]    read_index;
      //index counter to track the number of rows
      reg [VIDEO_Y_NUM_BITS : 0]    lines_index;
      //index counter to track the number of rows
      reg [BLOCK_X_NUM_BITS : 0]    blocks_index;
      //index counter to track the number of rows
      reg [BLOCK_Y_NUM_BITS : 0]    rows_index;
      //Flag marks the completion of comparison of the read data with the expected read data
      reg   compare_done;
      //Flag marks the completion of read/write of the read data with the expected read data
      reg   block_done;
      //Flag marks the completion of read/write of the read data with the expected read data
      reg   row_done;
      //Flag marks the completion of read/write of the read data with the expected read data
      reg   frame_done;
      //Flag is asserted when the write index reaches the last write transction number
      reg   last_write;
      //Flag is asserted when the read index reaches the last read transction number
      reg   last_read;
      reg   init_txn_ff;
      reg   init_txn_ff2;
      reg   init_txn_edge;
      wire  init_txn_pulse;
      reg   init_write;
      reg   init_read;


      // I/O Connections assignments

      //Adding the offset address to the base addr of the slave
      assign M_AXI_AWADDR	= C_M_TARGET_SLAVE_BASE_ADDR + WRITE_ADDR_OFFSET + axi_awaddr;
      //AXI 4 write data
      assign M_AXI_WDATA	= axi_wdata;
      assign M_AXI_AWPROT	= 3'b000;
      assign M_AXI_AWVALID	= axi_awvalid;
      //Write Data(W)
      assign M_AXI_WVALID	= axi_wvalid;
      //Set all byte strobes in this example
      assign M_AXI_WSTRB	= 4'b1111;
      //Write Response (B)
      assign M_AXI_BREADY	= axi_bready;
      //Read Address (AR)
      assign M_AXI_ARADDR	= C_M_TARGET_SLAVE_BASE_ADDR + READ_ADDR_OFFSET + axi_araddr;
      assign M_AXI_ARVALID	= axi_arvalid;
      assign M_AXI_ARPROT	= 3'b001;
      //Read and Read Response (R)
      assign M_AXI_RREADY	= axi_rready;
      //Example design I/O
      assign TXN_DONE	= compare_done;
      assign init_txn_pulse	= (!init_txn_ff2) && init_txn_ff;

      //user based
      assign w_bdata = test_data;
      assign red_xy = reg_red_xy;
      assign green_xy = reg_green_xy;
      assign system_done = reg_system_done;


      //Generate a pulse to initiate AXI transaction.
      always @ ( posedge M_AXI_ACLK )
        begin
          // Initiates AXI transaction delay
          if ( M_AXI_ARESETN == 0 )
            begin
              init_txn_ff <= 1'b0;
              init_txn_ff2 <= 1'b0;
            end
          else
            begin
              /*
              init_txn_ff <= INIT_AXI_TXN;
              */
              init_txn_ff <= module_init;
              init_txn_ff2 <= init_txn_ff;
            end
        end


    //--------------------
    //Write Address Channel
    //--------------------

    // The purpose of the write address channel is to request the address and
    // command information for the entire transaction.  It is a single beat
    // of information.

    // Note for this example the axi_awvalid/axi_wvalid are asserted at the same
    // time, and then each is deasserted independent from each other.
    // This is a lower-performance, but simplier control scheme.

    // AXI VALID signals must be held active until accepted by the partner.

    // A data transfer is accepted by the slave when a master has
    // VALID data and the slave acknoledges it is also READY. While the master
    // is allowed to generated multiple, back-to-back requests by not
    // deasserting VALID, this design will add rest cycle for
    // simplicity.

    // Since only one outstanding transaction is issued by the user design,
    // there will not be a collision between a new request and an accepted
    // request on the same clock cycle.

      always @ ( posedge M_AXI_ACLK )
        begin
          //Only VALID signals must be deasserted during reset per AXI spec
          //Consider inverting then registering active-low reset for higher fmax
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              axi_awvalid <= 1'b0;
            end
          else if ( init_write )
            begin
              axi_awvalid <= 1'b0;
            end
          //Signal a new address/data command is available by user logic
          else
            begin
              if ( start_single_write )
                begin
                  axi_awvalid <= 1'b1;
                end
              //Address accepted by interconnect/slave (issue of M_AXI_AWREADY by slave)
              else if ( M_AXI_AWREADY && axi_awvalid )
                begin
                  axi_awvalid <= 1'b0;
                end
            end
        end


      // start_single_write triggers a new write
      // transaction. write_index is a counter to
      // keep track with number of write transaction
      // issued/initiated
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              write_index <= 0;
            end
          else if ( init_write )
            begin
              write_index <= 0;
            end
          // Signals a new write address/ write data is
          // available by user logic
          else if ( start_single_write )
            begin
              write_index <= write_index + 1;
            end
        end


    //--------------------
    //Write Data Channel
    //--------------------

      //The write data channel is for transfering the actual data.
      //The data generation is speific to the example design, and
      //so only the WVALID/WREADY handshake is shown here
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              axi_wvalid <= 1'b0;
            end
          else if ( init_write )
            begin
              axi_wvalid <= 1'b0;
            end
          //Signal a new address/data command is available by user logic
          else if ( start_single_write )
            begin
              axi_wvalid <= 1'b1;
            end
          //Data accepted by interconnect/slave (issue of M_AXI_WREADY by slave)
          else if ( M_AXI_WREADY && axi_wvalid )
            begin
              axi_wvalid <= 1'b0;
            end
        end


    //----------------------------
    //Write Response (B) Channel
    //----------------------------

      //The write response channel provides feedback that the write has committed
      //to memory. BREADY will occur after both the data and the write address
      //has arrived and been accepted by the slave, and can guarantee that no
      //other accesses launched afterwards will be able to be reordered before it.

      //The BRESP bit [1] is used indicate any errors from the interconnect or
      //slave for the entire write burst. This example will capture the error.

      //While not necessary per spec, it is advisable to reset READY signals in
      //case of differing reset latencies between master/slave.
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              axi_bready <= 1'b0;
            end
          else if ( init_write )
            begin
              axi_bready <= 1'b0;
            end
          // accept/acknowledge bresp with axi_bready by the master
          // when M_AXI_BVALID is asserted by slave
          else if ( M_AXI_BVALID && ~axi_bready )
            begin
              axi_bready <= 1'b1;
            end
          // deassert after one clock cycle
          else if ( axi_bready )
            begin
              axi_bready <= 1'b0;
            end
          // retain the previous value
          else
            axi_bready <= axi_bready;
        end

      //Flag write errors
      assign write_resp_error = (axi_bready & M_AXI_BVALID & M_AXI_BRESP[1]);


    //----------------------------
    //Read Address Channel
    //----------------------------

      //start_single_row triggers a new read block transaction. rows_index is a counter to
      //keep track with number of read block transaction issued/initiated
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              rows_index <= 0;
            end
          // Signals a new read address is
          // available by user logic
          else if ( start_single_row )
            begin
              rows_index <= rows_index + 1;
            end
        end

      //start_single_block triggers a new read block transaction. blocks_index is a counter to
      //keep track with number of read block transaction issued/initiated
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              blocks_index <= 0;
            end
          else if ( start_single_row )
            begin
              blocks_index <= 0;
            end
          // Signals a new read address is
          // available by user logic
          else if ( start_single_block )
            begin
              blocks_index <= blocks_index + 1;
            end
        end

      //start_single_line triggers a new read transaction. read_index is a counter to
      //keep track with number of read transaction issued/initiated
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              lines_index <= 0;
            end
          else if ( start_single_block )
            begin
              lines_index <= 0;
            end
          // Signals a new read address is
          // available by user logic
          else if ( start_single_line )
            begin
              lines_index <= lines_index + 1;
            end
        end

      //start_single_read triggers a new read transaction. read_index is a counter to
      //keep track with number of read transaction issued/initiated
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              read_index <= 0;
            end
          else if ( init_read )
            begin
              read_index <= 0;
            end
          // Signals a new read address is
          // available by user logic
          else if ( start_single_read )
            begin
              read_index <= read_index + 1;
            end
        end

      // A new axi_arvalid is asserted when there is a valid read address
      // available by the master. start_single_read triggers a new read
      // transaction
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              axi_arvalid <= 1'b0;
            end
          else if ( init_read )
            begin
              axi_arvalid <= 1'b0;
            end
          //Signal a new read address command is available by user logic
          else if ( start_single_read )
            begin
              axi_arvalid <= 1'b1;
            end
          //RAddress accepted by interconnect/slave (issue of M_AXI_ARREADY by slave)
          else if ( M_AXI_ARREADY && axi_arvalid )
            begin
              axi_arvalid <= 1'b0;
            end
          // retain the previous value
        end


    //--------------------------------
    //Read Data (and Response) Channel
    //--------------------------------

      //The Read Data channel returns the results of the read request
      //The master will accept the read data by asserting axi_rready
      //when there is a valid read data available.
      //While not necessary per spec, it is advisable to reset READY signals in
      //case of differing reset latencies between master/slave.
   
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              axi_rready <= 1'b0;
            end
          else if ( init_read )
            begin
              axi_rready <= 1'b0;
            end
          // accept/acknowledge rdata/rresp with axi_rready by the master
          // when M_AXI_RVALID is asserted by slave
          else if ( M_AXI_RVALID && ~axi_rready )
            begin
              axi_rready <= 1'b1;
            end
          // deassert after one clock cycle
          else if ( axi_rready )
            begin
              axi_rready <= 1'b0;
            end
          // retain the previous value
        end

      //Flag write errors
      assign read_resp_error = (axi_rready & M_AXI_RVALID & M_AXI_RRESP[1]);

      // USER FSM Instantiations

      reg [31 : 0] curr_raddr; //current read address offset
      reg [31 : 0] curr_waddr; //current write address offset
      reg [31 : 0] bdata[0 : BLOCK_WIDTH-1]; // data stored from M_AXI_RDATA
      reg [31 : 0] test_data;
      reg [31 : 0] reg_red_xy;
      reg [31 : 0] reg_green_xy;
      reg [31 : 0] reg_system_done;

      // USER MASTER FSM Instantiations

      reg [31 : 0] mv_red_x;
      reg [31 : 0] mv_red_y;
      reg [31 : 0] mv_green_x;
      reg [31 : 0] mv_green_y;
      reg [31 : 0] mv_top_left_addr;
      reg [31 : 0] mv_counter;
      reg [31 : 0] mv_output_delta;

      reg [31 : 0] blk_red;
      reg [31 : 0] blk_green;
      reg [31 : 0] blk_blue;

      wire [31 : 0] rgb_red_r;
      wire [31 : 0] rgb_red_g;
      wire [31 : 0] rgb_red_b;

      assign rgb_red_r = set_rgb_r[23 : 16] * BLOCK_WIDTH * BLOCK_HEIGHT;
      assign rgb_red_g = set_rgb_r[15 :  8] * BLOCK_WIDTH * BLOCK_HEIGHT;
      assign rgb_red_b = set_rgb_r[ 7 :  0] * BLOCK_WIDTH * BLOCK_HEIGHT;

      wire [31 : 0] rgb_green_r;
      wire [31 : 0] rgb_green_g;
      wire [31 : 0] rgb_green_b;

      assign rgb_green_r = set_rgb_g[23 : 16] * BLOCK_WIDTH * BLOCK_HEIGHT;
      assign rgb_green_g = set_rgb_g[15 :  8] * BLOCK_WIDTH * BLOCK_HEIGHT;
      assign rgb_green_b = set_rgb_g[ 7 :  0] * BLOCK_WIDTH * BLOCK_HEIGHT;
      
      reg   found_red;
      reg   found_green;


    //--------------------------------
    //User Logic
    //--------------------------------

      //Address/Data Stimulus

      //Address/data pairs for this example. The read and write values should
      //match.
      //Modify these as desired for different address patterns.

      //Write Addresses
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              axi_awaddr <= 0;
            end
          else if ( init_write )
            begin
              axi_awaddr <= curr_waddr;
            end
          // Signals a new write address/ write data is
          // available by user logic
          else if ( M_AXI_AWREADY && axi_awvalid )
            begin
              axi_awaddr <= axi_awaddr + 32'h00000004;
            end
        end

      // Write data generation
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              axi_wdata <= C_M_START_DATA_VALUE;
            end
          else if ( init_write )
            begin
              axi_wdata <= bdata[0];
            end
            // Signals a new write address/ write data is
            // available by user logic
          else if ( M_AXI_WREADY && axi_wvalid )
            begin
              axi_wdata <= bdata[write_index];
            end
        end

      //Read Addresses
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0  || init_txn_pulse == 1'b1 )
            begin
              axi_araddr <= 0;
            end
          else if ( init_read )
            begin
              axi_araddr <= curr_raddr;
            end
          // Signals a new write address/ write data is
          // available by user logic
          else if ( M_AXI_ARREADY && axi_arvalid )
            begin
              axi_araddr <= axi_araddr + 32'h00000004;
            end
        end

      // FSM for calculation of EACH BLOCK
      //implement master command interface state machine
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 1'b0 )
            begin
              // reset condition
              // All the signals are assigned default values under reset condition
              mst_exec_state <= IDLE;

              start_single_line <= 1'b0;
              compare_done <= 1'b0;

              curr_raddr <= 32'h00000000;
              curr_waddr <= 32'h00000000;

              found_red <= 1'b0;
              found_green <= 1'b0;
              reg_system_done <= 0;
           end
          else
            begin
              // state transition
              case ( mst_exec_state )

                IDLE:
                  // This state is responsible to initiate
                  // AXI transaction when init_txn_pulse is asserted
                  if ( init_txn_pulse == 1'b1 )
                    begin
                      start_single_line <= 1'b0;
                      compare_done <= 1'b0;

                      curr_raddr <= 32'h00000000;
                      curr_waddr <= 32'h00000000;

                      found_red <= 1'b0;
                      found_green <= 1'b0;
                      reg_system_done <= 0;

                      mst_exec_state <= INIT_READ_ROW;
                    end
                  else
                    begin
                      mst_exec_state  <= IDLE;
                    end

                INIT_READ_ROW:
                  begin
                    start_single_row <= 1'b1;
                    mst_exec_state <= READ_ROW;
                  end

                READ_ROW:
                  begin
                    start_single_row <= 1'b0;
                    mst_exec_state <= INIT_READ_BLOCK;
                  end

                INIT_READ_BLOCK:
                  begin
                    start_single_block <= 1'b1;
                    mst_exec_state <= READ_BLOCK;
                  end

                READ_BLOCK:
                  begin
                    start_single_block <= 1'b0;
                    mst_exec_state <= INIT_READ_Y;
                  end

                INIT_READ_Y:
                  begin
                    curr_raddr <= (rows_index - 1) * BLOCK_HEIGHT * SCREEN_WIDTH * 4 +
                                  lines_index * SCREEN_WIDTH * 4 +
                                  (blocks_index - 1) * BLOCK_WIDTH * 4;
                    curr_waddr <= (rows_index - 1) * BLOCK_HEIGHT * SCREEN_WIDTH * 4 +
                                  lines_index * SCREEN_WIDTH * 4 +
                                  (blocks_index - 1) * BLOCK_WIDTH * 4;

                    start_single_line <= 1'b1;
                    mst_exec_state <= READ_Y;

                    start_single_write <= 1'b0;
                    write_issued  <= 1'b0;
                    start_single_read <= 1'b0;
                    read_issued <= 1'b0;
                    init_write <= 1'b0;
                    init_read <= 1'b0;
                  end
                  
                READ_Y:
                  begin
                    start_single_line <= 1'b0;
                    mst_exec_state <= INIT_READ;
                  end

                INIT_WRITE:
                  begin
                    start_single_write <= 1'b0;
                    write_issued <= 1'b0;
                    if ( init_write == 0 )
                      begin
                        init_write <= 1'b1;
                        mst_exec_state <= INIT_WRITE;
                      end
                    else
                      begin
                        init_write <= 1'b0;
                        mst_exec_state <= REPEAT_WRITE;
                      end
                  end

                REPEAT_WRITE:
                  begin
                    init_write <= 1'b0;

                    // This state is responsible to issue start_single_write pulse to
                    // initiate a write transaction. Write transactions will be
                    // issued until last_write signal is asserted.
                    // write controller
                    if ( writes_done )
                      begin
                        mst_exec_state <= INIT_COMPLETE;
                      end
                    else
                      begin
                        mst_exec_state <= REPEAT_WRITE;

                        if ( ~axi_awvalid && ~axi_wvalid && ~M_AXI_BVALID && ~last_write && ~start_single_write && ~write_issued )
                          begin
                            start_single_write <= 1'b1;
                            write_issued  <= 1'b1;
                          end
                        else if ( axi_bready )
                          begin
                            write_issued  <= 1'b0;
                          end
                        else
                          begin
                            start_single_write <= 1'b0; //Negate to generate a pulse
                          end
                      end
                  end

                INIT_READ:
                  begin
                    start_single_read <= 1'b0;
                    read_issued <= 1'b0;
                    if ( init_read == 0 )
                      begin
                        init_read <= 1'b1;
                        mst_exec_state <= INIT_READ;
                      end
                    else
                      begin
                        init_read <= 1'b0;
                        mst_exec_state <= REPEAT_READ;
                      end
                  end

                REPEAT_READ:
                  begin
                    init_read <= 1'b0;

                    // This state is responsible to issue start_single_read pulse to
                    // initiate a read transaction. Read transactions will be
                    // issued until last_read signal is asserted.
                    // read controller
                    if ( reads_done )
                      begin
                        mst_exec_state <= INIT_WRITE;
                      end
                    else
                      begin
                        mst_exec_state <= REPEAT_READ;

                        if ( ~axi_arvalid && ~M_AXI_RVALID && ~last_read && ~start_single_read && ~read_issued )
                          begin
                            start_single_read <= 1'b1;
                            read_issued  <= 1'b1;
                          end
                        else if (axi_rready)
                          begin
                            read_issued  <= 1'b0;
                          end
                        else
                          begin
                            start_single_read <= 1'b0; //Negate to generate a pulse
                          end
                       end
                  end

                INIT_COMPLETE:
                  begin
                    // This state is responsible to issue the state of comparison
                    // of written data with the read data. If no error flags are set,
                    // compare_done signal will be asseted to indicate success.
                    if ( block_done )
                      begin
                        if (rows_index == 6 && blocks_index == 9 )
                          begin
//                            test_data <= blk_red;
                            test_data <= set_rgb_r;
                          end
                        if ( !found_red &&
                             blk_red   > rgb_red_r &&
                             blk_green < rgb_red_g &&
                             blk_blue  < rgb_red_b )
                          begin
                            found_red <= 1'b1;
                            reg_red_xy[31 : 16] <= (rows_index - 1) * BLOCK_HEIGHT;
                            reg_red_xy[15 :  0] <= (blocks_index - 1) * BLOCK_WIDTH;
                            reg_system_done[1 : 1] <= 1'b1;
                          end
                        else if ( !found_green &&
                             blk_red   < rgb_green_r &&
                             blk_green > rgb_green_g &&
                             blk_blue  < rgb_green_b )
                          begin
                            found_green <= 1'b1;
                            reg_green_xy[31 : 16] <= (rows_index - 1) * BLOCK_HEIGHT;
                            reg_green_xy[15 :  0] <= (blocks_index - 1) * BLOCK_WIDTH;
                            reg_system_done[2 : 2] <= 1'b1;
                          end
                      end

                    if ( frame_done )
                      begin
                        reg_system_done[0 : 0] <= 1'b1;
                        compare_done <= 1'b1;
                        mst_exec_state <= IDLE;
                      end
                    else if ( row_done )
                      begin
                        mst_exec_state <= INIT_READ_ROW;
                      end
                    else if ( block_done )
                      begin
                        mst_exec_state <= INIT_READ_BLOCK;
                      end
                    else
                      begin
                        mst_exec_state <= INIT_READ_Y;
                      end
                  end
                default :
                  begin
                    mst_exec_state <= IDLE;
                  end
              endcase
            end
        end //MASTER_EXECUTION_PROC

      //Terminal write count
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            last_write <= 1'b0;
          else if ( init_write )
            last_write <= 1'b0;
          //The last write should be associated with a write address ready response
          else if ( (write_index == BLOCK_WIDTH) && M_AXI_AWREADY )
            last_write <= 1'b1;
          else
            last_write <= last_write;
        end

      //Check for last write completion.

      //This logic is to qualify the last write count with the final write
      //response. This demonstrates how to confirm that a write has been
      //committed.
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              writes_done <= 1'b0;
            end
          else if ( init_write )
            begin
              writes_done <= 1'b0;
            end
          //The writes_done should be associated with a bready response
          else if ( last_write && M_AXI_BVALID && axi_bready )
            begin
              writes_done <= 1'b1;
            end
          else
            begin
              writes_done <= writes_done;
            end
        end

      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              block_done <= 1'b0;
            end
          else if ( start_single_block )
            begin
              block_done <= 1'b0;
            end
          //The writes_done should be associated with a bready response
          else if ( (lines_index == BLOCK_HEIGHT) && writes_done )
            begin
              block_done <= 1'b1;
            end
          else
            begin
              block_done <= block_done;
            end
        end

      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              row_done <= 1'b0;
            end
          else if ( start_single_row )
            begin
              row_done <= 1'b0;
            end
          //The writes_done should be associated with a bready response
          else if ( (blocks_index == VIDEO_X_BLK_NUM) &&
                    (lines_index  == BLOCK_HEIGHT) &&
                    writes_done )
            begin
              row_done <= 1'b1;
            end
          else
            begin
              row_done <= row_done;
            end
        end

      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              frame_done <= 1'b0;
            end
          //The writes_done should be associated with a bready response
          else if ( (rows_index   == VIDEO_Y_BLK_NUM) &&
                    (blocks_index == VIDEO_X_BLK_NUM) &&
                    (lines_index  == BLOCK_HEIGHT) &&
                    writes_done )
            begin
              frame_done <= 1'b1;
            end
          else
            begin
              frame_done <= frame_done;
            end
        end

    //------------------
    //Read example
    //------------------

      //Read Data
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
            end
          //The read data when available (on axi_rready) is compared with the expected data
          else if ( !reads_done && M_AXI_RVALID && axi_rready )
            begin
              bdata[ read_index - 1 ] <= M_AXI_RDATA;
            end
          else
            begin
              // read_index starts from 1
              bdata[ read_index - 1 ] <= bdata[ read_index - 1 ];
            end
        end

      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            begin
              blk_red   <= 32'h00000000;
              blk_green <= 32'h00000000;
              blk_blue  <= 32'h00000000;
            end
          else if ( start_single_block )
            begin
              blk_red   <= 32'h00000000;
              blk_green <= 32'h00000000;
              blk_blue  <= 32'h00000000;
            end
          //The read data when available (on axi_rready) is compared with the expected data
          else if ( !reads_done && M_AXI_RVALID && axi_rready )
            begin
              blk_red   <= blk_red   + M_AXI_RDATA[23 : 16];
              blk_green <= blk_green + M_AXI_RDATA[15 :  8];
              blk_blue  <= blk_blue  + M_AXI_RDATA[ 7 :  0];
            end
          else
            begin
              // read_index starts from 1
              blk_red   <= blk_red;
              blk_green <= blk_green;
              blk_blue  <= blk_blue;
            end
        end

      //Terminal Read Count
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            last_read <= 1'b0;
          else if ( init_read )
            last_read <= 1'b0;
          //The last read should be associated with a read address ready response
          else if ( (read_index == BLOCK_WIDTH) && M_AXI_ARREADY )
            last_read <= 1'b1;
          else
            last_read <= last_read;
        end

      /*
       Check for last read completion.

       This logic is to qualify the last read count with the final read
       response/data.
       */
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0 || init_txn_pulse == 1'b1 )
            reads_done <= 1'b0;
          else if ( init_read )
            reads_done <= 1'b0;
          //The reads_done should be associated with a read ready response
          else if ( last_read && M_AXI_RVALID && axi_rready )
            reads_done <= 1'b1;
          else
            reads_done <= reads_done;
        end

    //-----------------------------
    //Example design error register
    //-----------------------------

      // Register and hold any data mismatches, or read/write interface errors
      always @ ( posedge M_AXI_ACLK )
        begin
          if ( M_AXI_ARESETN == 0  || init_txn_pulse == 1'b1 )
            error_reg <= 1'b0;

          //Capture any error types
          else if ( write_resp_error || read_resp_error )
            error_reg <= 1'b1;
          else
            error_reg <= error_reg;
        end

      // Add user logic here

      // User logic ends

    endmodule
