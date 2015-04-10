/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xaxivdma.h"
#include "xparameters.h"
#include "xil_exception.h"
#include "delta_calc.h"

#include "xintc.h"
#include "xtft.h"

#define RED_THRESHOLD 		0x00808080
#define GREEN_THRESHOLD		0x00808080

#define FGCOLOR_VALUE		0x0000FF00
#define BGCOLOR_VALUE		0x0
#define WHITECOLOR_VALUE 	0x00FFFFFF
#define REDCOLOR_VALUE		0xFF0000
#define BLUECOLOR_VALUE		0x0000FF
#define YELLOWCOLOR_VALUE	0xFFFF00
#define CYANCOLOR_VALUE		0x00FFFF
#define PURPLECOLOR_VALUE	0x800080
#define ORANGECOLOR_VALUE	0xFFA500
#define OLIVECOLOR_VALUE	0x556B2F
#define AZURECOLOR_VALUE	0xDCDCDC
#define RYLBLUECOLOR_VALUE	0x4169E1
#define DGOLDRCOLOR_VALUE	0xB8860B
#define BROWNCOLOR_VALUE	0xA0522D
#define GRAYCOLOR_VALUE		0x696969
#define GRELCOLOR_VALUE		0xADFF2F
#define LCHIFFONCOLOR_VALUE	0xFFFACD
#define PERUCOLOR_VALUE	0xCD853F

#define TFT_FRAME_ADDR XPAR_MIG_7SERIES_0_BASEADDR
#define TFT_FRAME_ADDR 0x81000000

#define ANIMATION_SPEED		50000

#define SCREEN_WIDTH        320
#define SCREEN_HEIGHT       240

/* VDMA */

/*
 * Device related constants. These need to defined as per the HW system.
 */
#define DMA_DEVICE_ID		XPAR_AXIVDMA_0_DEVICE_ID

#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define WRITE_INTR_ID		XPAR_INTC_0_AXIVDMA_0_VEC_ID

#ifdef XPAR_V6DDR_0_S_AXI_BASEADDR
#define DDR_BASE_ADDR		XPAR_V6DDR_0_S_AXI_BASEADDR
#define DDR_HIGH_ADDR		XPAR_V6DDR_0_S_AXI_HIGHADDR
#elif XPAR_S6DDR_0_S0_AXI_BASEADDR
#define DDR_BASE_ADDR		XPAR_S6DDR_0_S0_AXI_BASEADDR
#define DDR_HIGH_ADDR		XPAR_S6DDR_0_S0_AXI_HIGHADDR
#elif XPAR_AXI_7SDDR_0_S_AXI_BASEADDR
#define DDR_BASE_ADDR		XPAR_AXI_7SDDR_0_S_AXI_BASEADDR
#define DDR_HIGH_ADDR		XPAR_AXI_7SDDR_0_S_AXI_HIGHADDR
#elif XPAR_MIG7SERIES_0_BASEADDR
#define DDR_BASE_ADDR		XPAR_MIG7SERIES_0_BASEADDR
#define DDR_HIGH_ADDR	 	XPAR_MIG7SERIES_0_HIGHADDR
#else
#warning CHECK FOR THE VALID DDR ADDRESS IN XPARAMETERS.H, \
			DEFAULT SET TO 0x01000000
#define DDR_BASE_ADDR		0x01000000
#define DDR_HIGH_ADDR		0x0F000000
#endif

/* Memory space for the frame buffers
 *
 * This example only needs one set of frame buffers, because one video IP is
 * to write to the frame buffers, and the other video IP is to read from the
 * frame buffers.
 *
 * For 16 frames of 1080p, it needs 0x07E90000 memory for frame buffers
 */
#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x01000000)
#define MEM_HIGH_ADDR		DDR_HIGH_ADDR
#define MEM_SPACE		(MEM_HIGH_ADDR - MEM_BASE_ADDR)

/* Read channel and write channel start from the same place
 *
 * One video IP write to the memory region, the other video IP read from it
 */
#define READ_ADDRESS_BASE	MEM_BASE_ADDR
#define WRITE_ADDRESS_BASE	MEM_BASE_ADDR

/* Frame size related constants
 */
#define FRAME_HORIZONTAL_LEN  1024*4  /* 1920 pixels, each pixel 4 bytes */
#define FRAME_VERTICAL_LEN    768   /* 1080 pixels */

/* Subframe to be transferred by Video DMA
 *
 *|<----------------- FRAME_HORIZONTAL_LEN ---------------------->|
 * --------------------------------------------------------------------
 *|                                                                | ^
 *|                                                                | |
 *|               |<-SUBFRAME_HORIZONTAL_SIZE ->|                  | FRAME_
 *|               -----------------------------------              | VERTICAL_
 *|               |/////////////////////////////|  ^               | LEN
 *|               |/////////////////////////////|  |               | |
 *|               |/////////////////////////////|  |               | |
 *|               |/////////////////////////////| SUBFRAME_        | |
 *|               |/////////////////////////////| VERTICAL_        | |
 *|               |/////////////////////////////| SIZE             | |
 *|               |/////////////////////////////|  |               | |
 *|               |/////////////////////////////|  v               | |
 *|                ----------------------------------              | |
 *|                                                                | v
 *--------------------------------------------------------------------
 *
 * Note that SUBFRAME_HORIZONTAL_SIZE and SUBFRAME_VERTICAL_SIZE must ensure
 * to be inside the frame.
 */
#define SUBFRAME_START_OFFSET    0
#define SUBFRAME_HORIZONTAL_SIZE 320*4
#define SUBFRAME_VERTICAL_SIZE   240

/* Number of frames to work on, this is to set the frame count threshold
 *
 * We multiply 15 to the num stores is to increase the intervals between
 * interrupts. If you are using fsync, then it is not necessary.
 */
#define NUMBER_OF_READ_FRAMES	3
#define NUMBER_OF_WRITE_FRAMES	3

/* Number of frames to transfer
 *
 * This is used to monitor the progress of the test, test purpose only
 */
#define NUM_TEST_FRAME_SETS	100

/* Delay timer counter
 *
 * WARNING: If you are using fsync, please increase the delay counter value
 * to be 255. Because with fsync, the inter-frame delay is long. If you do not
 * care about inactivity of the hardware, set this counter to be 0, which
 * disables delay interrupt.
 */
#define DELAY_TIMER_COUNTER	10

/*
 * Device instance definitions
 */
XAxiVdma AxiVdma;

#ifdef XPAR_INTC_0_DEVICE_ID
static XIntc Intc;	/* Instance of the Interrupt Controller */
#else
static XScuGic Intc;	/* Instance of the Interrupt Controller */
#endif

/* Data address
 *
 * Read and write sub-frame use the same settings
 */
static u32 WriteFrameAddr;
static u32 BlockStartOffset;
static u32 BlockHoriz;
static u32 BlockVert;

/* DMA channel setup
 */
static XAxiVdma_DmaSetup WriteCfg;

/* Transfer statics
 */
static int WriteDone;
static int WriteError;

/******************* Function Prototypes ************************************/

static int WriteSetup(XAxiVdma * InstancePtr);
static int StartTransfer(XAxiVdma *InstancePtr);

static int SetupIntrSystem(XAxiVdma *AxiVdmaPtr,u16 WriteIntrId);

static void DisableIntrSystem(u16 WriteIntrId);

/* Interrupt call back functions
 */
//static void ReadCallBack(void *CallbackRef, u32 Mask);
//static void ReadErrorCallBack(void *CallbackRef, u32 Mask);
static void WriteCallBack(void *CallbackRef, u32 Mask);
static void WriteErrorCallBack(void *CallbackRef, u32 Mask);


/* VDMA end */

void print(char *str);
//volatile unsigned int * memptr = (unsigned int*) XPAR_MIG_7SERIES_0_BASEADDR;

/*********************************Function Prototypes**********************************/
int TftExample(u32 TftDeviceId);
void drawBox(int x, int y, int length, int height, int color);
void drawLeftRightDescent(int x, int starty, int endy, int thickness, int color);
void drawLeftRightAscent(int x, int starty, int endy, int thickness, int color);
void drawCircle(int centerx, int centery, int radius, int color);
void drawCloud1(int x, int y);
void drawCloud2(int x, int y);
void P1Stance(int* P1pstate, int* P2pstate, int* P1state, int* P2state);
void P2Stance(int* P1pstate, int* P2pstate, int* P1state, int* P2state);
void P1Punch(int* P1pstate, int* P2pstate, int* P1state, int* P2state);
void P2Punch(int* P1pstate, int* P2pstate, int* P1state, int* P2state);
void P1Duck(int* P1pstate, int* P2pstate, int* P1state, int* P2state);
void P2Duck(int* P1pstate, int* P2pstate, int* P1state, int* P2state);
void P1Erase(int* P1pstate, int* P1state, int* P2pstate);
void P2Erase(int* P2pstate, int* P2state, int* P1pstate);
/*********************************Function Prototypes**********************************/

/**************************************Variables***************************************/
static XTft TftInstance;
static int player1 = XTFT_DISPLAY_WIDTH/2 - 44;
static int player2 = XTFT_DISPLAY_WIDTH/2 + 44;
int P1Health = 220;
int P2Health = 419;
int P1pState = 0;
int P2pState = 0;
int P1State = 0;
int P2State = 0;
int P1Hit = 0;
int P2Hit = 0;
int P1Damage = 0;
int P2Damage = 0;
int waste_time;

/**************************************Variables***************************************/

int main()
{
    init_platform();
    vdmaSetup();
    mainCycle(XPAR_TFT_0_DEVICE_ID);

    return 0;
}

int vdmaSetup()
{
	init_platform();

	int Status;
	XAxiVdma_Config *Config;

#if defined(XPAR_UARTNS550_0_BASEADDR)
	Uart550_Setup();
#endif

	WriteDone = 0;
	WriteError = 0;

	WriteFrameAddr = WRITE_ADDRESS_BASE;
	BlockStartOffset = SUBFRAME_START_OFFSET;
	BlockHoriz = SUBFRAME_HORIZONTAL_SIZE;
	BlockVert = SUBFRAME_VERTICAL_SIZE;

	xil_printf("\r\n--- Entering main() --- \r\n");

	/* The information of the XAxiVdma_Config comes from hardware build.
	 * The user IP should pass this information to the AXI DMA core.
	 */
	Config = XAxiVdma_LookupConfig(DMA_DEVICE_ID);
	if (!Config) {
		xil_printf(
		    "No video DMA found for ID %d\r\n", DMA_DEVICE_ID);

		return XST_FAILURE;
	}

	/* Initialize DMA engine */
	Status = XAxiVdma_CfgInitialize(&AxiVdma, Config, Config->BaseAddress);
	if (Status != XST_SUCCESS) {

		xil_printf(
		    "Configuration Initialization failed %d\r\n", Status);

		return XST_FAILURE;
	}

	/*
	 * Setup your video IP that writes to the memory
	 */


	/* Setup the write channel
	 */
	Status = WriteSetup(&AxiVdma);
	if (Status != XST_SUCCESS) {
		xil_printf(
		    	"Write channel setup failed %d\r\n", Status);
		if(Status == XST_VDMA_MISMATCH_ERROR)
			xil_printf("DMA Mismatch Error\r\n");

		return XST_FAILURE;
	}


	Status = SetupIntrSystem(&AxiVdma, WRITE_INTR_ID);
	if (Status != XST_SUCCESS) {

		xil_printf(
		    "Setup interrupt system failed %d\r\n", Status);

		return XST_FAILURE;
	}

	/* Register callback functions
	 */
	XAxiVdma_SetCallBack(&AxiVdma, XAXIVDMA_HANDLER_GENERAL,
	    WriteCallBack, (void *)&AxiVdma, XAXIVDMA_WRITE);

	XAxiVdma_SetCallBack(&AxiVdma, XAXIVDMA_HANDLER_ERROR,
	    WriteErrorCallBack, (void *)&AxiVdma, XAXIVDMA_WRITE);

	/* Enable your video IP interrupts if needed
	 */

	/* Start the DMA engine to transfer
	 */
	Status = StartTransfer(&AxiVdma);
	if (Status != XST_SUCCESS) {
		if(Status == XST_VDMA_MISMATCH_ERROR)
			xil_printf("DMA Mismatch Error\r\n");
		return XST_FAILURE;
	}

	/* Enable DMA read and write channel interrupts
	 *
	 * If interrupts overwhelms the system, please do not enable interrupt
	 */
	XAxiVdma_IntrEnable(&AxiVdma, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_WRITE);
	/*
	XAxiVdma_IntrEnable(&AxiVdma, XAXIVDMA_IXR_ALL_MASK, XAXIVDMA_READ);
	 */

	/* Every set of frame buffer finish causes a completion interrupt
	 */

	//setup delta_calc
	//XPAR_DELTA_CALC_0_S00_AXI_BASEADDR
   	DELTA_CALC_mWriteReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR,  8, RED_THRESHOLD);
    DELTA_CALC_mWriteReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 12, GREEN_THRESHOLD);

	DELTA_CALC_mWriteReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 0, 0x00000000);
	DELTA_CALC_mWriteReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 0, 0xFFFFFFFF);

//	while ((WriteDone < NUM_TEST_FRAME_SETS) /* && !ReadError &&
//	      (ReadDone < NUM_TEST_FRAME_SETS) && !WriteError */) {
 		/* NOP */
//	}

	if (/* ReadError || */ WriteError) {
		xil_printf("Test has transfer error %d\r\n",
		    WriteError);

		Status = XST_FAILURE;
	}
	else {
		xil_printf("Test passed\r\n");
	}

	xil_printf("--- Exiting main() --- \r\n");

//	DisableIntrSystem(WRITE_INTR_ID);

	if (Status != XST_SUCCESS) {
		if(Status == XST_VDMA_MISMATCH_ERROR)
			xil_printf("DMA Mismatch Error\r\n");
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

int mainCycle(u32 TftDeviceId)
{
/*********************************XTFT Setup Start*************************************/
	int Status;
	XTft_Config *TftConfigPtr;

	TftConfigPtr = XTft_LookupConfig(TftDeviceId);
	if (TftConfigPtr == (XTft_Config *)NULL) {
		return XST_FAILURE;
	}

	Status = XTft_CfgInitialize(&TftInstance, TftConfigPtr, TftConfigPtr->BaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	while (XTft_GetVsyncStatus(&TftInstance) != XTFT_IESR_VADDRLATCH_STATUS_MASK);

	XTft_SetFrameBaseAddr(&TftInstance, TFT_FRAME_ADDR);
	XTft_ClearScreen(&TftInstance);
	XTft_SetColor(&TftInstance, FGCOLOR_VALUE, BGCOLOR_VALUE);
/*********************************XTFT Setup Completed*********************************/

/*********************************Health Setup Start***********************************/
	XTft_Write(&TftInstance, 'P');
	XTft_Write(&TftInstance, 'l');
	XTft_Write(&TftInstance, 'a');
	XTft_Write(&TftInstance, 'y');
	XTft_Write(&TftInstance, 'e');
	XTft_Write(&TftInstance, 'r');
	XTft_Write(&TftInstance, ' ');
	XTft_Write(&TftInstance, '1');
	drawBox(0, 15, 220, 10, REDCOLOR_VALUE);

	TftInstance.ColVal = 575;
	XTft_Write(&TftInstance, 'P');
	XTft_Write(&TftInstance, 'l');
	XTft_Write(&TftInstance, 'a');
	XTft_Write(&TftInstance, 'y');
	XTft_Write(&TftInstance, 'e');
	XTft_Write(&TftInstance, 'r');
	XTft_Write(&TftInstance, ' ');
	XTft_Write(&TftInstance, '2');
	drawBox(419, 15, 220, 10, BLUECOLOR_VALUE);
/*********************************Health Setup Completed*******************************/

/*******************************Background Setup Start*********************************/
	drawCircle(340, 80, 50, LCHIFFONCOLOR_VALUE);//Moon

	drawCloud1(60, 200);
	drawCloud2(130, 100);
	drawCloud1(240, 180);
	drawCloud1(380, 200);
	drawCloud2(460, 100);
	drawCloud1(560, 180);

	drawBox(0, 449, 639, 30, PERUCOLOR_VALUE);//Ground
/*********************************Background Setup Completed***************************/

/**************************************Game Start**************************************/
	while(1) {
		while (P1pState == P1State)
//			P1State = rand()%3;
        {
            unsigned int test_data   = DELTA_CALC_mReadReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 4);
            unsigned int system_done = DELTA_CALC_mReadReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 7 * 4);
            unsigned int red_xy      = DELTA_CALC_mReadReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 5 * 4);
            unsigned int green_xy    = DELTA_CALC_mReadReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 6 * 4);
            // bit 0 = 1 done; bit 1 = 1 found red; bit 2 = 1 found green
            if (system_done & 0x7)
            {
                // done and found red and green
                unsigned int red_x, red_y, green_x, green_y;
                
                // high 16 bit is y, low 16 bit is x 
                red_x = (red_xy & 0xffff);
                red_y = (red_xy >> 16);
                green_x = (green_xy & 0xffff);
                green_y = (green_xy >> 16);
                xil_printf("DELTA_CALC: red_x: %d red_y: %d green_x: %d green_y: %d\r\n", red_x, red_y, green_x, green_y);
                
                if (red_y > SCREEN_HEIGHT/2)
                    P1State = 2;
                else
                {
                    static unsigned int prev_delta = 0;
                    unsigned int delta;
                    if (green_x > red_x)
                        delta = green_x - red_x;
                    else
                        delta = red_x - green_x;
                    
                    if (delta > prev_delta)
                        P1State = 1;
                    else
                        P1State = 0;
                    prev_delta = delta;
                }
            }
            else if (system_done & 0x3)
            {
                // found red but not green
            	xil_printf("DELTA_CALC: Green undetected this cycle\r\n");
            }
            else if (system_done & 0x5)
            {
                // found green but not redn
            	xil_printf("DELTA_CALC: Red undetected this cycle\r\n");
            }
            else if (system_done & 0x1)
            {
                // done but found nothing
            	xil_printf("DELTA_CALC: No colors detected this cycle\r\n");
            }
            else
            {
                // FPGA not yet finish
            }
        }
		while (P2pState == P2State)
			P2State = rand()%3;

		//Erasing Both Players
		P1Erase(&P1pState, &P1State, &P2pState);
		P2Erase(&P2pState, &P2State, &P1pState);

		//Erasing Hit Markers
		if (P1Hit){
			drawCircle(player2 - 65, 325, 5, BGCOLOR_VALUE);
			P1Hit = 0;
		}
		if (P2Hit){
			drawCircle(player1 + 79, 325, 5, BGCOLOR_VALUE);
			P2Hit = 0;
		}

		//Drawing Player 1
		if (P1State == 0)
			P1Stance(&P1pState, &P2pState, &P1State, &P2State);
		else if (P1State == 1){
			P1Punch(&P1pState, &P2pState, &P1State, &P2State);
			P1Damage = rand()%3 + 1;
		}
		else
			P1Duck(&P1pState, &P2pState, &P1State, &P2State);

		//Drawing Player 2
		if (P2State == 0)
			P2Stance(&P1pState, &P2pState, &P1State, &P2State);
		else if (P2State == 1){
			P2Punch(&P1pState, &P2pState, &P1State, &P2State);
			P2Damage = rand()%3 + 1;
		}
		else
			P2Duck(&P1pState, &P2pState, &P1State, &P2State);

		//Player 1 Was Hit
		if (P1State == 0 && P2State == 1){
			//Player 1 Loses Health
			if (P1Health > 0){
				if (P2Damage == 1){
					drawBox(P1Health - 11, 15, 11, 10, BLUECOLOR_VALUE);
					P1Health = P1Health - 11;
				}
				else if (P2Damage == 2){
					if (P1Health - 22 >= 0)
						drawBox(P1Health - 22, 15, 22, 10, BLUECOLOR_VALUE);
					else
						drawBox(P1Health - 11, 15, 11, 10, BLUECOLOR_VALUE);
					P1Health = P1Health - 22;
				}
				else{
					if (P1Health - 33 >= 0)
						drawBox(P1Health - 33, 15, 33, 10, BLUECOLOR_VALUE);
					else if (P1Health - 33 == -11)
						drawBox(P1Health - 22, 15, 22, 10, BLUECOLOR_VALUE);
					else
						drawBox(P1Health - 11, 15, 11, 10, BLUECOLOR_VALUE);
					P1Health = P1Health - 33;
				}
			}
			drawCircle(player2 - 65, 325, 5, BLUECOLOR_VALUE);
			P1Hit = 1;
		}

		//Player 2 Was Hit
		if (P2State == 0 && P1State == 1){
			//Player 2 Loses Health
			if (P2Health < 639){
				if (P1Damage == 1){
					drawBox(P2Health, 15, 11, 10, REDCOLOR_VALUE);
					P2Health = P2Health + 11;
				}
				else if (P1Damage == 2){
					if (P2Health + 22 <= 639)
						drawBox(P2Health, 15, 22, 10, REDCOLOR_VALUE);
					else
						drawBox(P2Health, 15, 11, 10, REDCOLOR_VALUE);
					P2Health = P2Health + 22;
				}
				else{
					if (P2Health + 33 <= 639)
						drawBox(P2Health, 15, 33, 10, REDCOLOR_VALUE);
					else if (P2Health + 33 == 650)
						drawBox(P2Health, 15, 22, 10, REDCOLOR_VALUE);
					else
						drawBox(P2Health, 15, 11, 10, REDCOLOR_VALUE);
					P2Health = P2Health + 33;
				}
			}
			drawCircle(player1 + 79, 325, 5, REDCOLOR_VALUE);
			P2Hit = 1;
		}

		//Both Players Were Hit
		if (P1State == 1 && P2State == 1){
			//Player 1 Loses Health
			if (P1Health > 0){
				if (P2Damage == 1){
					drawBox(P1Health - 11, 15, 11, 10, BLUECOLOR_VALUE);
					P1Health = P1Health - 11;
				}
				else if (P2Damage == 2){
					if (P1Health - 22 >= 0)
						drawBox(P1Health - 22, 15, 22, 10, BLUECOLOR_VALUE);
					else
						drawBox(P1Health - 11, 15, 11, 10, BLUECOLOR_VALUE);
					P1Health = P1Health - 22;
				}
				else{
					if (P1Health - 33 >= 0)
						drawBox(P1Health - 33, 15, 33, 10, BLUECOLOR_VALUE);
					else if (P1Health - 33 == -11)
						drawBox(P1Health - 22, 15, 22, 10, BLUECOLOR_VALUE);
					else
						drawBox(P1Health - 11, 15, 11, 10, BLUECOLOR_VALUE);
					P1Health = P1Health - 33;
				}
			}
			if (P2Health < 639){
				if (P1Damage == 1){
					drawBox(P2Health, 15, 11, 10, REDCOLOR_VALUE);
					P2Health = P2Health + 11;
				}
				else if (P1Damage == 2){
					if (P2Health + 22 <= 639)
						drawBox(P2Health, 15, 22, 10, REDCOLOR_VALUE);
					else
						drawBox(P2Health, 15, 11, 10, REDCOLOR_VALUE);
					P2Health = P2Health + 22;
				}
				else{
					if (P2Health + 33 <= 639)
						drawBox(P2Health, 15, 33, 10, REDCOLOR_VALUE);
					else if (P2Health + 33 == 650)
						drawBox(P2Health, 15, 22, 10, REDCOLOR_VALUE);
					else
						drawBox(P2Health, 15, 11, 10, REDCOLOR_VALUE);
					P2Health = P2Health + 33;
				}
			}
		}

		waste_time = 0;
		while (waste_time < ANIMATION_SPEED) {
			waste_time++;
		}

		//The Match Is A Draw
		if(P1Health <= 0 && P2Health >= 639){
			XTft_ClearScreen(&TftInstance);
			TftInstance.RowVal = 232;
			TftInstance.ColVal = 310;
			XTft_Write(&TftInstance, 'D');
			XTft_Write(&TftInstance, 'R');
			XTft_Write(&TftInstance, 'A');
			XTft_Write(&TftInstance, 'W');
			break;
		}

		//Player 1 Has Lost & Player 2 Has Won
		if (P1Health <= 0){
			XTft_ClearScreen(&TftInstance);
			TftInstance.RowVal = 232;
			TftInstance.ColVal = 275;
			XTft_Write(&TftInstance, 'P');
			XTft_Write(&TftInstance, 'l');
			XTft_Write(&TftInstance, 'a');
			XTft_Write(&TftInstance, 'y');
			XTft_Write(&TftInstance, 'e');
			XTft_Write(&TftInstance, 'r');
			XTft_Write(&TftInstance, ' ');
			XTft_Write(&TftInstance, '2');
			XTft_Write(&TftInstance, ' ');
			XTft_Write(&TftInstance, 'W');
			XTft_Write(&TftInstance, 'I');
			XTft_Write(&TftInstance, 'N');
			XTft_Write(&TftInstance, 'S');
			break;
		}

		//Player 2 Has Lost & Player 1 Has Won
		if (P2Health >= 639){
			XTft_ClearScreen(&TftInstance);
			TftInstance.RowVal = 232;
			TftInstance.ColVal = 275;
			XTft_Write(&TftInstance, 'P');
			XTft_Write(&TftInstance, 'l');
			XTft_Write(&TftInstance, 'a');
			XTft_Write(&TftInstance, 'y');
			XTft_Write(&TftInstance, 'e');
			XTft_Write(&TftInstance, 'r');
			XTft_Write(&TftInstance, ' ');
			XTft_Write(&TftInstance, '1');
			XTft_Write(&TftInstance, ' ');
			XTft_Write(&TftInstance, 'W');
			XTft_Write(&TftInstance, 'I');
			XTft_Write(&TftInstance, 'N');
			XTft_Write(&TftInstance, 'S');
			break;
		}

		//Erasing Punch Collision
		if (P1State == 1 && P2State == 1)
			drawBox(player2 - 43, 318, 13, 14, BGCOLOR_VALUE);
	}
	return XST_SUCCESS;
/***************************************Game End***************************************/
}

void drawBox(int x, int y, int length, int height, int color){
	int i, j;
	for(i = x; i <= x + length; i++){
		for(j = y; j <= y + height; j++)
			XTft_SetPixel(&TftInstance, i, j, color);
	}
}

void drawLeftRightDescent(int x, int starty, int endy, int thickness, int color){
	int i, count;
	for (i = x; starty < endy; starty=starty+2, i++){
		count = 1;
		for (; count < thickness; count++) {
			XTft_SetPixel(&TftInstance, i + count, starty, color);
			XTft_SetPixel(&TftInstance, i + count, starty+1, color);
		}
	}
}

void drawLeftRightAscent(int x, int starty, int endy, int thickness, int color){
	int i, count;
	for (i = x; starty > endy; starty=starty-2, i++){
		count = 1;
		for (; count < thickness; count++) {
			XTft_SetPixel(&TftInstance, i + count, starty, color);
			XTft_SetPixel(&TftInstance, i + count, starty - 1, color);
		}
	}
}

void drawCircle(int centerx, int centery, int radius, int color){
	int x, y;
	for (y = -radius; y <= radius; y++){
		for (x = -radius; x <= radius; x++){
			if ((x * x) + (y * y) <= (radius * radius))
				XTft_SetPixel(&TftInstance, centerx + x, centery + y, color);
		}
	}
}

void drawCloud1(int x, int y){
	drawCircle(x, y, 20, AZURECOLOR_VALUE);
	drawCircle(x + 10, y - 20, 20, AZURECOLOR_VALUE);
	drawCircle(x + 15, y + 10, 20, AZURECOLOR_VALUE);
	drawCircle(x + 20, y - 10, 20, AZURECOLOR_VALUE);
	drawCircle(x + 25, y + 5, 15, AZURECOLOR_VALUE);
	drawCircle(x + 30, y - 10, 20, AZURECOLOR_VALUE);
	drawCircle(x + 35, y - 20, 20, AZURECOLOR_VALUE);
	drawCircle(x + 40, y, 20, AZURECOLOR_VALUE);
	drawCircle(x + 50, y - 15, 20, AZURECOLOR_VALUE);
}

void drawCloud2(int x, int y){
	drawCircle(x, y, 20, AZURECOLOR_VALUE);
	drawCircle(x + 10, y - 20, 20, AZURECOLOR_VALUE);
	drawCircle(x + 15, y + 10, 20, AZURECOLOR_VALUE);
	drawCircle(x + 20, y - 10, 20, AZURECOLOR_VALUE);
	drawCircle(x + 25, y + 5, 15, AZURECOLOR_VALUE);
	drawCircle(x + 30, y - 10, 20, AZURECOLOR_VALUE);
	drawCircle(x + 35, y - 20, 20, AZURECOLOR_VALUE);
	drawCircle(x + 40, y + 5, 25, AZURECOLOR_VALUE);
	drawCircle(x + 50, y - 15, 20, AZURECOLOR_VALUE);
	drawCircle(x + 55, y - 5, 25, AZURECOLOR_VALUE);
	drawCircle(x + 60, y - 20, 20, AZURECOLOR_VALUE);
	drawCircle(x + 60, y + 5, 20, AZURECOLOR_VALUE);
	drawCircle(x + 70, y - 10, 20, AZURECOLOR_VALUE);
}

void P1Stance(int* P1pstate, int* P2pstate, int* P1state, int* P2state){
	if (*P1pstate == 1){
		drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
		drawLeftRightDescent(player1 - 10, 320, 355, 15, BROWNCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 12, 355, 310, 10, BROWNCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 32, 310, 300, 15, REDCOLOR_VALUE);//Fist
	}
	else if (*P1pstate == 2){
		drawBox(player1, 300, 15, 15, BROWNCOLOR_VALUE);//Head
		drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
		drawBox(player1 - 10, 377, 10, 60, GRAYCOLOR_VALUE);//Left Leg
		drawBox(player1 + 15, 377, 10, 60, GRAYCOLOR_VALUE);//Right Leg
		drawLeftRightDescent(player1 - 10, 320, 355, 15, BROWNCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 12, 355, 310, 10, BROWNCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 32, 310, 300, 15, REDCOLOR_VALUE);//Fist
	}
	else {
		drawBox(player1, 300, 15, 15, BROWNCOLOR_VALUE);//Head
		drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
		drawLeftRightDescent(player1 - 10, 320, 355, 15, BROWNCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 12, 355, 310, 10, BROWNCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 32, 310, 300, 15, REDCOLOR_VALUE);//Fist
		drawBox(player1 - 10, 377, 10, 60, GRAYCOLOR_VALUE);//Left Leg
		drawBox(player1 + 15, 377, 10, 60, GRAYCOLOR_VALUE);//Right Leg
		drawBox(player1 - 10, 438, 20, 10, YELLOWCOLOR_VALUE);//Left Foot
		drawBox(player1 + 15, 438, 20, 10, YELLOWCOLOR_VALUE);//Right Foot
	}
	*P1pstate = *P1state;
}

void P2Stance(int* P1pstate, int* P2pstate, int* P1state, int* P2state){
	if (*P2pstate == 1){
		drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
		drawLeftRightAscent(player2 - 7, 355, 320, 15, ORANGECOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 30, 309, 355, 10, ORANGECOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 36, 301, 311, 15, BLUECOLOR_VALUE);//Fist
	}
	else if (*P2pstate == 2){
		drawBox(player2, 300, 15, 15, ORANGECOLOR_VALUE);//Head
		drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
		drawBox(player2 - 10, 377, 10, 60, RYLBLUECOLOR_VALUE);//Right Leg
		drawBox(player2 + 15, 377, 10, 60, RYLBLUECOLOR_VALUE);//Left Leg
		drawLeftRightAscent(player2 - 7, 355, 320, 15, ORANGECOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 30, 309, 355, 10, ORANGECOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 36, 301, 311, 15, BLUECOLOR_VALUE);//Fist
	}
	else {
		drawBox(player2, 300, 15, 15, ORANGECOLOR_VALUE);//Head
		drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
		drawLeftRightAscent(player2 - 7, 355, 320, 15, ORANGECOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 30, 309, 355, 10, ORANGECOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 36, 301, 311, 15, BLUECOLOR_VALUE);//Fist
		drawBox(player2 - 10, 377, 10, 60, RYLBLUECOLOR_VALUE);//Right Leg
		drawBox(player2 + 15, 377, 10, 60, RYLBLUECOLOR_VALUE);//Left Leg
		drawBox(player2 - 20, 438, 20, 10, AZURECOLOR_VALUE);//Right Foot
		drawBox(player2 + 5, 438, 20, 10, AZURECOLOR_VALUE);//Left Foot
	}
	*P2pstate = *P2state;
}

void P1Punch(int* P1pstate, int* P2pstate, int* P1state, int* P2state){
	if (*P1state != *P2state){
		if (*P1pstate == 0){
			drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
			drawBox(player1, 320, 60, 10, BROWNCOLOR_VALUE);//Arm
			drawBox(player1 + 60, 318, 13, 14, REDCOLOR_VALUE);//Fist
		}
		else if (*P1pstate == 2){
			drawBox(player1, 300, 15, 15, BROWNCOLOR_VALUE);//Head
			drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
			drawBox(player1, 320, 60, 10, BROWNCOLOR_VALUE);//Arm
			drawBox(player1 + 60, 318, 13, 14, REDCOLOR_VALUE);//Fist
			drawBox(player1 - 10, 377, 10, 60, GRAYCOLOR_VALUE);//Left Leg
			drawBox(player1 + 15, 377, 10, 60, GRAYCOLOR_VALUE);//Right Leg
		}
		else {
			drawBox(player1, 300, 15, 15, BROWNCOLOR_VALUE);//Head
			drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
			drawBox(player1, 320, 60, 10, BROWNCOLOR_VALUE);//Arm
			drawBox(player1 + 60, 318, 13, 14, REDCOLOR_VALUE);//Fist
			drawBox(player1 - 10, 377, 10, 60, GRAYCOLOR_VALUE);//Left Leg
			drawBox(player1 + 15, 377, 10, 60, GRAYCOLOR_VALUE);//Right Leg
			drawBox(player1 - 10, 438, 20, 10, YELLOWCOLOR_VALUE);//Left Foot
			drawBox(player1 + 15, 438, 20, 10, YELLOWCOLOR_VALUE);//Right Foot
		}
	}
	else {
		if (*P1pstate == 0){
			drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
			drawBox(player1, 320, 45, 10, BROWNCOLOR_VALUE);//Arm
			drawBox(player1 + 45, 318, 13, 14, REDCOLOR_VALUE);//Fist
		}
		else if (*P1pstate == 2){
			drawBox(player1, 300, 15, 15, BROWNCOLOR_VALUE);//Head
			drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
			drawBox(player1, 320, 45, 10, BROWNCOLOR_VALUE);//Arm
			drawBox(player1 + 45, 318, 13, 14, REDCOLOR_VALUE);//Fist
			drawBox(player1 - 10, 377, 10, 60, GRAYCOLOR_VALUE);//Left Leg
			drawBox(player1 + 15, 377, 10, 60, GRAYCOLOR_VALUE);//Right Leg
		}
		else {
			drawBox(player1, 300, 15, 15, BROWNCOLOR_VALUE);//Head
			drawBox(player1 - 15, 316, 45, 60, GRELCOLOR_VALUE);//Torso
			drawBox(player1, 320, 45, 10, BROWNCOLOR_VALUE);//Arm
			drawBox(player1 + 45, 318, 13, 14, REDCOLOR_VALUE);//Fist
			drawBox(player1 - 10, 377, 10, 60, GRAYCOLOR_VALUE);//Left Leg
			drawBox(player1 + 15, 377, 10, 60, GRAYCOLOR_VALUE);//Right Leg
			drawBox(player1 - 10, 438, 20, 10, YELLOWCOLOR_VALUE);//Left Foot
			drawBox(player1 + 15, 438, 20, 10, YELLOWCOLOR_VALUE);//Right Foot
		}
	}
	*P1pstate = *P1state;;
}

void P2Punch(int* P1pstate, int* P2pstate, int* P1state, int* P2state){
	if (*P2state != *P1state){
		if (*P2pstate == 0){
			drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
			drawBox(player2 - 45, 320, 60, 10, ORANGECOLOR_VALUE);//Arm
			drawBox(player2 - 59, 318, 13, 14, BLUECOLOR_VALUE);//Fist
		}
		else if (*P2pstate == 2){
			drawBox(player2, 300, 15, 15, ORANGECOLOR_VALUE);//Head
			drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
			drawBox(player2 - 45, 320, 60, 10, ORANGECOLOR_VALUE);//Arm
			drawBox(player2 - 59, 318, 13, 14, BLUECOLOR_VALUE);//Fist
			drawBox(player2 - 10, 377, 10, 60, RYLBLUECOLOR_VALUE);//Right Leg
			drawBox(player2 + 15, 377, 10, 60, RYLBLUECOLOR_VALUE);//Left Leg
		}
		else {
			drawBox(player2, 300, 15, 15, ORANGECOLOR_VALUE);//Head
			drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
			drawBox(player2 - 45, 320, 60, 10, ORANGECOLOR_VALUE);//Arm
			drawBox(player2 - 59, 318, 13, 14, BLUECOLOR_VALUE);//Fist
			drawBox(player2 - 10, 377, 10, 60, RYLBLUECOLOR_VALUE);//Right Leg
			drawBox(player2 + 15, 377, 10, 60, RYLBLUECOLOR_VALUE);//Left Leg
			drawBox(player2 - 20, 438, 20, 10, AZURECOLOR_VALUE);//Right Foot
			drawBox(player2 + 5, 438, 20, 10, AZURECOLOR_VALUE);//Left Foot
		}
	}
	else {
		if (*P2pstate == 0){
			drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
			drawBox(player2 - 30, 320, 45, 10, ORANGECOLOR_VALUE);//Arm
			drawBox(player2 - 43, 318, 13, 14, BLUECOLOR_VALUE);//Fist
		}
		else if (*P2pstate == 2){
			drawBox(player2, 300, 15, 15, ORANGECOLOR_VALUE);//Head
			drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
			drawBox(player2 - 30, 320, 45, 10, ORANGECOLOR_VALUE);//Arm
			drawBox(player2 - 43, 318, 13, 14, BLUECOLOR_VALUE);//Fist
			drawBox(player2 - 10, 377, 10, 60, RYLBLUECOLOR_VALUE);//Right Leg
			drawBox(player2 + 15, 377, 10, 60, RYLBLUECOLOR_VALUE);//Left Leg
		}
		else {
			drawBox(player2, 300, 15, 15, ORANGECOLOR_VALUE);//Head
			drawBox(player2 - 15, 316, 45, 60, OLIVECOLOR_VALUE);//Torso
			drawBox(player2 - 30, 320, 45, 10, ORANGECOLOR_VALUE);//Arm
			drawBox(player2 - 43, 318, 13, 14, BLUECOLOR_VALUE);//Fist
			drawBox(player2 - 10, 377, 10, 60, RYLBLUECOLOR_VALUE);//Right Leg
			drawBox(player2 + 15, 377, 10, 60, RYLBLUECOLOR_VALUE);//Left Leg
			drawBox(player2 - 20, 438, 20, 10, AZURECOLOR_VALUE);//Right Foot
			drawBox(player2 + 5, 438, 20, 10, AZURECOLOR_VALUE);//Left Foot
		}
	}
	*P2pstate = *P2state;
}

void P1Duck(int* P1pstate, int* P2pstate, int* P1state, int* P2state){
	if (*P1pstate == 0){
		drawBox(player1, 360, 15, 15, BROWNCOLOR_VALUE);//Head
		drawBox(player1 - 15, 376, 45, 30, GRELCOLOR_VALUE);//Torso
		drawLeftRightDescent(player1 - 10, 380, 405, 15, BROWNCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 8, 405, 370, 10, BROWNCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 23, 370, 360, 15, REDCOLOR_VALUE);//Fist
		drawLeftRightDescent(player1 - 12, 407, 423, 14, GRAYCOLOR_VALUE);//Left Thigh
		drawLeftRightAscent(player1 - 10, 438, 423, 10, GRAYCOLOR_VALUE);//Left Calf
		drawLeftRightDescent(player1 + 13, 407, 423, 14, GRAYCOLOR_VALUE);//Right Thigh
		drawLeftRightAscent(player1 + 15, 438, 423, 10, GRAYCOLOR_VALUE);//Right Calf
	}
	else if (*P1pstate == 1){
		drawBox(player1, 360, 15, 15, BROWNCOLOR_VALUE);//Head
		drawBox(player1 - 15, 376, 45, 30, GRELCOLOR_VALUE);//Torso
		drawLeftRightDescent(player1 - 10, 380, 405, 15, BROWNCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 8, 405, 370, 10, BROWNCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 23, 370, 360, 15, REDCOLOR_VALUE);//Fist
		drawLeftRightDescent(player1 - 12, 407, 423, 14, GRAYCOLOR_VALUE);//Left Thigh
		drawLeftRightAscent(player1 - 10, 438, 423, 10, GRAYCOLOR_VALUE);//Left Calf
		drawLeftRightDescent(player1 + 13, 407, 423, 14, GRAYCOLOR_VALUE);//Right Thigh
		drawLeftRightAscent(player1 + 15, 438, 423, 10, GRAYCOLOR_VALUE);//Right Calf
	}
	else {
		drawBox(player1, 360, 15, 15, BROWNCOLOR_VALUE);//Head
		drawBox(player1 - 15, 376, 45, 30, GRELCOLOR_VALUE);//Torso
		drawLeftRightDescent(player1 - 10, 380, 405, 15, BROWNCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 8, 405, 370, 10, BROWNCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 23, 370, 360, 15, REDCOLOR_VALUE);//Fist
		drawLeftRightDescent(player1 - 12, 407, 423, 14, GRAYCOLOR_VALUE);//Left Thigh
		drawLeftRightAscent(player1 - 10, 438, 423, 10, GRAYCOLOR_VALUE);//Left Calf
		drawLeftRightDescent(player1 + 13, 407, 423, 14, GRAYCOLOR_VALUE);//Right Thigh
		drawLeftRightAscent(player1 + 15, 438, 423, 10, GRAYCOLOR_VALUE);//Right Calf
		drawBox(player1 - 10, 438, 20, 10, YELLOWCOLOR_VALUE);//Left Foot
		drawBox(player1 + 15, 438, 20, 10, YELLOWCOLOR_VALUE);//Right Foot
	}
	*P1pstate = *P1state;
}

void P2Duck(int* P1pstate, int* P2pstate, int* P1state, int* P2state){
	if (*P2pstate == 0){
		drawBox(player2, 360, 15, 15, ORANGECOLOR_VALUE);//Head
		drawBox(player2 - 15, 376, 45, 30, OLIVECOLOR_VALUE);//Torso
		drawLeftRightAscent(player2 - 7, 403, 380, 15, ORANGECOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 24, 370, 403, 10, ORANGECOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 31, 360, 370, 15, BLUECOLOR_VALUE);//Fist
		drawLeftRightAscent(player2 - 19, 423, 406, 14, RYLBLUECOLOR_VALUE);//Left Thigh
		drawLeftRightDescent(player2 - 17, 423, 438, 10, RYLBLUECOLOR_VALUE);//Left Calf
		drawLeftRightAscent(player2 + 6, 423, 406, 14, RYLBLUECOLOR_VALUE);//Right Thigh
		drawLeftRightDescent(player2 + 8, 423, 438, 10, RYLBLUECOLOR_VALUE);//Right Calf
	}
	else if(*P2pstate == 1){
		drawBox(player2, 360, 15, 15, ORANGECOLOR_VALUE);//Head
		drawBox(player2 - 15, 376, 45, 30, OLIVECOLOR_VALUE);//Torso
		drawLeftRightAscent(player2 - 7, 403, 380, 15, ORANGECOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 24, 370, 403, 10, ORANGECOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 31, 360, 370, 15, BLUECOLOR_VALUE);//Fist
		drawLeftRightAscent(player2 - 19, 423, 406, 14, RYLBLUECOLOR_VALUE);//Left Thigh
		drawLeftRightDescent(player2 - 17, 423, 438, 10, RYLBLUECOLOR_VALUE);//Left Calf
		drawLeftRightAscent(player2 + 6, 423, 406, 14, RYLBLUECOLOR_VALUE);//Right Thigh
		drawLeftRightDescent(player2 + 8, 423, 438, 10, RYLBLUECOLOR_VALUE);//Right Calf
	}
	else {
		drawBox(player2, 360, 15, 15, ORANGECOLOR_VALUE);//Head
		drawBox(player2 - 15, 376, 45, 30, OLIVECOLOR_VALUE);//Torso
		drawLeftRightAscent(player2 - 7, 403, 380, 15, ORANGECOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 24, 370, 403, 10, ORANGECOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 31, 360, 370, 15, BLUECOLOR_VALUE);//Fist
		drawLeftRightAscent(player2 - 19, 423, 406, 14, RYLBLUECOLOR_VALUE);//Left Thigh
		drawLeftRightDescent(player2 - 17, 423, 438, 10, RYLBLUECOLOR_VALUE);//Left Calf
		drawLeftRightAscent(player2 + 6, 423, 406, 14, RYLBLUECOLOR_VALUE);//Right Thigh
		drawLeftRightDescent(player2 + 8, 423, 438, 10, RYLBLUECOLOR_VALUE);//Right Calf
		drawBox(player2 - 20, 438, 20, 10, AZURECOLOR_VALUE);//Right Foot
		drawBox(player2 + 5, 438, 20, 10, AZURECOLOR_VALUE);//Left Foot
	}
	*P2pstate = *P2state;
}

void P1Erase(int* P1pstate, int* P1state, int* P2pstate){
	//No Need To Erase Player 1
	if (*P1pstate == *P1state){
		return;
	}
	//Player 1 Stance --> Punch
	else if (*P1pstate == 0 && *P1state == 1){
		drawLeftRightDescent(player1 - 10, 320, 355, 15, BGCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 12, 355, 310, 10, BGCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 32, 310, 300, 15, BGCOLOR_VALUE);//Fist
		*P1pstate = *P1state;
		return;
	}
	//Player 1 Stance --> Duck
	else if (*P1pstate == 0 && *P1state == 2){
		drawBox(player1, 300, 15, 15, BGCOLOR_VALUE);//Head
		drawBox(player1 - 15, 316, 45, 60, BGCOLOR_VALUE);//Torso
		drawLeftRightDescent(player1 - 10, 320, 355, 15, BGCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 12, 355, 310, 10, BGCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 32, 310, 300, 15, BGCOLOR_VALUE);//Fist
		drawBox(player1 - 10, 377, 10, 60, BGCOLOR_VALUE);//Left Leg
		drawBox(player1 + 15, 377, 10, 60, BGCOLOR_VALUE);//Right Leg
		*P1pstate = *P1state;
		return;
	}
	//Player 1 Punch --> Stance
	else if (*P1pstate == 1 && *P1state == 0){
		drawBox(player1, 320, 60, 10, BGCOLOR_VALUE);//Arm
		drawBox(player1 + 60, 318, 13, 14, BGCOLOR_VALUE);//Fist
		*P1pstate = *P1state;
		return;
	}
	//Player 1 Punch --> Duck
	else if (*P1pstate == 1 && *P1state == 2){
		drawBox(player1, 300, 15, 15, BGCOLOR_VALUE);//Head
		drawBox(player1 - 15, 316, 45, 60, BGCOLOR_VALUE);//Torso
		drawBox(player1, 320, 60, 10, BGCOLOR_VALUE);//Arm
		drawBox(player1 + 60, 318, 13, 14, BGCOLOR_VALUE);//Fist
		drawBox(player1 - 10, 377, 10, 60, BGCOLOR_VALUE);//Left Leg
		drawBox(player1 + 15, 377, 10, 60, BGCOLOR_VALUE);//Right Leg
		*P1pstate = *P1state;
		return;
	}
	//Player 1 Duck --> Stance
	else if (*P1pstate == 2 && *P1state == 0){
		drawBox(player1, 360, 15, 15, BGCOLOR_VALUE);//Head
		drawBox(player1 - 15, 376, 45, 30, BGCOLOR_VALUE);//Torso
		drawLeftRightDescent(player1 - 10, 380, 405, 15, BGCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 8, 405, 370, 10, BGCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 23, 370, 360, 15, BGCOLOR_VALUE);//Fist
		drawLeftRightDescent(player1 - 12, 407, 423, 14, BGCOLOR_VALUE);//Left Thigh
		drawLeftRightAscent(player1 - 10, 438, 423, 10, BGCOLOR_VALUE);//Left Calf
		drawLeftRightDescent(player1 + 13, 407, 423, 14, BGCOLOR_VALUE);//Right Thigh
		drawLeftRightAscent(player1 + 15, 438, 423, 10, BGCOLOR_VALUE);//Right Calf
		*P1pstate = *P1state;
		return;
	}
	//Player 1 Duck --> Punch
	else {
		drawBox(player1, 360, 15, 15, BGCOLOR_VALUE);//Head
		drawBox(player1 - 15, 376, 45, 30, BGCOLOR_VALUE);//Torso
		drawLeftRightDescent(player1 - 10, 380, 405, 15, BGCOLOR_VALUE);//Upper Arm
		drawLeftRightAscent(player1 + 8, 405, 370, 10, BGCOLOR_VALUE);//Forearm
		drawLeftRightAscent(player1 + 23, 370, 360, 15, BGCOLOR_VALUE);//Fist
		drawLeftRightDescent(player1 - 12, 407, 423, 14, BGCOLOR_VALUE);//Left Thigh
		drawLeftRightAscent(player1 - 10, 438, 423, 10, BGCOLOR_VALUE);//Left Calf
		drawLeftRightDescent(player1 + 13, 407, 423, 14, BGCOLOR_VALUE);//Right Thigh
		drawLeftRightAscent(player1 + 15, 438, 423, 10, BGCOLOR_VALUE);//Right Calf
		*P1pstate = *P1state;
		return;
	}
}

void P2Erase(int* P2pstate, int* P2state, int* P1pstate){
	//No Need To Erase Player 2
	if (*P2pstate == *P2state){
		return;
	}
	//Player 2 Stance --> Punch
	else if (*P2pstate == 0 && *P2state == 1){
		drawLeftRightAscent(player2 - 7, 355, 320, 15, BGCOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 30, 309, 355, 10, BGCOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 36, 301, 311, 15, BGCOLOR_VALUE);//Fist
		return;
	}
	//Player 2 Stance --> Duck
	else if (*P2pstate == 0 && *P2state == 2){
		drawBox(player2, 300, 15, 15, BGCOLOR_VALUE);//Head
		drawBox(player2 - 15, 316, 45, 60, BGCOLOR_VALUE);//Torso
		drawLeftRightAscent(player2 - 7, 355, 320, 15, BGCOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 30, 309, 355, 10, BGCOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 36, 301, 311, 15, BGCOLOR_VALUE);//Fist
		drawBox(player2 - 10, 377, 10, 60, BGCOLOR_VALUE);//Right Leg
		drawBox(player2 + 15, 377, 10, 60, BGCOLOR_VALUE);//Left Leg
		*P2pstate = *P2state;
		return;
	}
	//Player 2 Punch --> Stance
	else if (*P2pstate == 1 && *P2state == 0){
		drawBox(player2 - 45, 320, 60, 10, BGCOLOR_VALUE);//Arm
		drawBox(player2 - 59, 318, 13, 14, BGCOLOR_VALUE);//Fist
		*P2pstate = *P2state;
		return;
	}
	//Player 2 Punch --> Duck
	else if (*P2pstate == 1 && *P2state == 2){
		drawBox(player2, 300, 15, 15, BGCOLOR_VALUE);//Head
		drawBox(player2 - 15, 316, 45, 60, BGCOLOR_VALUE);//Torso
		drawBox(player2 - 45, 320, 60, 10, BGCOLOR_VALUE);//Arm
		drawBox(player2 - 59, 318, 13, 14, BGCOLOR_VALUE);//Fist
		drawBox(player2 - 10, 377, 10, 60, BGCOLOR_VALUE);//Right Leg
		drawBox(player2 + 15, 377, 10, 60, BGCOLOR_VALUE);//Left Leg
		*P2pstate = *P2state;
		return;
	}
	//Player 2 Duck --> Stance
	else if (*P2pstate == 2 && *P2state == 0){
		drawBox(player2, 360, 15, 15, BGCOLOR_VALUE);//Head
		drawBox(player2 - 15, 376, 45, 30, BGCOLOR_VALUE);//Torso
		drawLeftRightAscent(player2 - 7, 403, 380, 15, BGCOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 24, 370, 403, 10, BGCOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 31, 360, 370, 15, BGCOLOR_VALUE);//Fist
		drawLeftRightAscent(player2 - 19, 423, 406, 14, BGCOLOR_VALUE);//Left Thigh
		drawLeftRightDescent(player2 - 17, 423, 438, 10, BGCOLOR_VALUE);//Left Calf
		drawLeftRightAscent(player2 + 6, 423, 406, 14, BGCOLOR_VALUE);//Right Thigh
		drawLeftRightDescent(player2 + 8, 423, 438, 10, BGCOLOR_VALUE);//Right Calf
		*P2pstate = *P2state;
		return;
	}
	//Player 2 Duck --> Punch
	else {
		drawBox(player2, 360, 15, 15, BGCOLOR_VALUE);//Head
		drawBox(player2 - 15, 376, 45, 30, BGCOLOR_VALUE);//Torso
		drawLeftRightAscent(player2 - 7, 403, 380, 15, BGCOLOR_VALUE);//Upper Arm
		drawLeftRightDescent(player2 - 24, 370, 403, 10, BGCOLOR_VALUE);//Forearm
		drawLeftRightDescent(player2 - 31, 360, 370, 15, BGCOLOR_VALUE);//Fist
		drawLeftRightAscent(player2 - 19, 423, 406, 14, BGCOLOR_VALUE);//Left Thigh
		drawLeftRightDescent(player2 - 17, 423, 438, 10, BGCOLOR_VALUE);//Left Calf
		drawLeftRightAscent(player2 + 6, 423, 406, 14, BGCOLOR_VALUE);//Right Thigh
		drawLeftRightDescent(player2 + 8, 423, 438, 10, BGCOLOR_VALUE);//Right Calf
		*P2pstate = *P2state;
		return;
	}
}

/*****************************************************************************/
/**
*
* This function sets up the write channel
*
* @param	InstancePtr is the instance pointer to the DMA engine.
*
* @return	XST_SUCCESS if the setup is successful, XST_FAILURE otherwise.
*
* @note		None.
*
******************************************************************************/
static int WriteSetup(XAxiVdma * InstancePtr)
{
	int Index;
	u32 Addr;
	int Status;

	WriteCfg.VertSizeInput = SUBFRAME_VERTICAL_SIZE;
	WriteCfg.HoriSizeInput = SUBFRAME_HORIZONTAL_SIZE;

	WriteCfg.Stride = FRAME_HORIZONTAL_LEN;
	WriteCfg.FrameDelay = 0;  /* This example does not test frame delay */

	WriteCfg.EnableCircularBuf = 1;
	WriteCfg.EnableSync = 0;  /* No Gen-Lock */

	WriteCfg.PointNum = 0;    /* No Gen-Lock */
	WriteCfg.EnableFrameCounter = 0; /* Endless transfers */

	WriteCfg.FixedFrameStoreAddr = 0; /* We are not doing parking */

	Status = XAxiVdma_DmaConfig(InstancePtr, XAXIVDMA_WRITE, &WriteCfg);
	if (Status != XST_SUCCESS) {
		xil_printf(
		    "Write channel config failed %d\r\n", Status);

		return XST_FAILURE;
	}

	/* Initialize buffer addresses
	 *
	 * Use physical addresses
	 */
	Addr = WRITE_ADDRESS_BASE + BlockStartOffset;
	for(Index = 0; Index < NUMBER_OF_WRITE_FRAMES; Index++) {
		WriteCfg.FrameStoreStartAddr[Index] = Addr;

		Addr += FRAME_HORIZONTAL_LEN * FRAME_VERTICAL_LEN;
	}

	/* Set the buffer addresses for transfer in the DMA engine
	 */
	Status = XAxiVdma_DmaSetBufferAddr(InstancePtr, XAXIVDMA_WRITE,
	        WriteCfg.FrameStoreStartAddr);
	if (Status != XST_SUCCESS) {
		xil_printf(
		    "Write channel set buffer address failed %d\r\n", Status);

		return XST_FAILURE;
	}

	/* Clear data buffer
	 */
	memset((void *)WriteFrameAddr, 0,
	    FRAME_HORIZONTAL_LEN * FRAME_VERTICAL_LEN * NUMBER_OF_WRITE_FRAMES);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function starts the DMA transfers. Since the DMA engine is operating
* in circular buffer mode, video frames will be transferred continuously.
*
* @param	InstancePtr points to the DMA engine instance
*
* @return	XST_SUCCESS if both read and write start succesfully
*		XST_FAILURE if one or both directions cannot be started
*
* @note		None.
*
******************************************************************************/
static int StartTransfer(XAxiVdma *InstancePtr)
{
	int Status;

	Status = XAxiVdma_DmaStart(InstancePtr, XAXIVDMA_WRITE);
	if (Status != XST_SUCCESS) {
		xil_printf(
		    "Start Write transfer failed %d\r\n", Status);

		return XST_FAILURE;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/*
*
* This function setups the interrupt system so interrupts can occur for the
* DMA.  This function assumes INTC component exists in the hardware system.
*
* @param	AxiDmaPtr is a pointer to the instance of the DMA engine
* @param	ReadIntrId is the read channel Interrupt ID.
* @param	WriteIntrId is the write channel Interrupt ID.
*
* @return	XST_SUCCESS if successful, otherwise XST_FAILURE.
*
* @note		None.
*
******************************************************************************/
static int SetupIntrSystem(XAxiVdma *AxiVdmaPtr, u16 WriteIntrId)
{
	int Status;

	XIntc *IntcInstancePtr =&Intc;


	/* Initialize the interrupt controller and connect the ISRs */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {

		xil_printf( "Failed init intc\r\n");
		return XST_FAILURE;
	}

	/*
	Status = XIntc_Connect(IntcInstancePtr, ReadIntrId,
	         (XInterruptHandler)XAxiVdma_ReadIntrHandler, AxiVdmaPtr);
	if (Status != XST_SUCCESS) {

		xil_printf(
		    "Failed read channel connect intc %d\r\n", Status);
		return XST_FAILURE;
	}
	 */

	Status = XIntc_Connect(IntcInstancePtr, WriteIntrId,
	         (XInterruptHandler)XAxiVdma_WriteIntrHandler, AxiVdmaPtr);
	if (Status != XST_SUCCESS) {

		xil_printf(
		    "Failed write channel connect intc %d\r\n", Status);
		return XST_FAILURE;
	}

	/* Start the interrupt controller */
	Status = XIntc_Start(IntcInstancePtr, XIN_REAL_MODE);
	if (Status != XST_SUCCESS) {

		xil_printf( "Failed to start intc\r\n");
		return XST_FAILURE;
	}

	/* Enable interrupts from the hardware */
	/*
	XIntc_Enable(IntcInstancePtr, ReadIntrId);
	 */
	XIntc_Enable(IntcInstancePtr, WriteIntrId);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_InterruptHandler,
			(void *)IntcInstancePtr);

	Xil_ExceptionEnable();


	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function disables the interrupts
*
* @param	ReadIntrId is interrupt ID associated w/ DMA read channel
* @param	WriteIntrId is interrupt ID associated w/ DMA write channel
*
* @return	None.
*
* @note		None.
*
******************************************************************************/
static void DisableIntrSystem(u16 WriteIntrId)
{

	XIntc *IntcInstancePtr =&Intc;

	/* Disconnect the interrupts for the DMA TX and RX channels */
	/*
	XIntc_Disconnect(IntcInstancePtr, ReadIntrId);
	 */
	XIntc_Disconnect(IntcInstancePtr, WriteIntrId);
}

/*****************************************************************************/
/*
 * Call back function for write channel
 *
 * This callback only clears the interrupts and updates the transfer status.
 *
 * @param	CallbackRef is the call back reference pointer
 * @param	Mask is the interrupt mask passed in from the driver
 *
 * @return	None
*
******************************************************************************/
static void WriteCallBack(void *CallbackRef, u32 Mask)
{

	if (Mask & XAXIVDMA_IXR_FRMCNT_MASK) {
		WriteDone += 1;
        if (WriteDone >= 10) {
//            xil_printf( "WriteCallBack\r\n");

            WriteDone = 0;

         	DELTA_CALC_mWriteReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR,  8, RED_THRESHOLD);
            DELTA_CALC_mWriteReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 12, GREEN_THRESHOLD);

           	DELTA_CALC_mWriteReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 0, 0x00000000);
            DELTA_CALC_mWriteReg(XPAR_DELTA_CALC_0_S00_AXI_BASEADDR, 0, 0xFFFFFFFF);
        }
	}
}

/*****************************************************************************/
/*
* Call back function for write channel error interrupt
*
* @param	CallbackRef is the call back reference pointer
* @param	Mask is the interrupt mask passed in from the driver
*
* @return	None
*
******************************************************************************/
static void WriteErrorCallBack(void *CallbackRef, u32 Mask)
{

	if (Mask & XAXIVDMA_IXR_ERROR_MASK) {
		xil_printf( "WriteError\r\n");
		WriteError += 1;
	}
}
