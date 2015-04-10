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

//#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
//#else
//#include "xscugic.h"
//#endif

/*
 * Device related constants. These need to defined as per the HW system.
 */
#define DMA_DEVICE_ID		XPAR_AXIVDMA_0_DEVICE_ID

//#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC_DEVICE_ID		XPAR_INTC_0_DEVICE_ID
#define WRITE_INTR_ID		XPAR_INTC_0_AXIVDMA_0_S2MM_INTROUT_VEC_ID
#define READ_INTR_ID		XPAR_INTC_0_AXIVDMA_0_MM2S_INTROUT_VEC_ID
//#else
//#define INTC_DEVICE_ID		XPAR_SCUGIC_SINGLE_DEVICE_ID
//#define WRITE_INTR_ID		XPAR_FABRIC_AXIVDMA_0_S2MM_INTROUT_VEC_ID
//#define READ_INTR_ID		XPAR_FABRIC_AXIVDMA_0_MM2S_INTROUT_VEC_ID
//#endif

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
#define NUM_TEST_FRAME_SETS	10

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

static int SetupIntrSystem(XAxiVdma *AxiVdmaPtr, u16 ReadIntrId,
				u16 WriteIntrId);

static void DisableIntrSystem(u16 ReadIntrId, u16 WriteIntrId);

/* Interrupt call back functions
 */
//static void ReadCallBack(void *CallbackRef, u32 Mask);
//static void ReadErrorCallBack(void *CallbackRef, u32 Mask);
static void WriteCallBack(void *CallbackRef, u32 Mask);
static void WriteErrorCallBack(void *CallbackRef, u32 Mask);

void print(char *str);

int main(void)
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


	Status = SetupIntrSystem(&AxiVdma, READ_INTR_ID, WRITE_INTR_ID);
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

	/* Every set of frame buffer finish causes a completion interrupt
	 */


	volatile int i = 0;
	for(i = 0; i < 1000000; i++) {
	}

	xil_printf("--- Exiting main() --- \r\n");

	for(i = 0; i < 1000000; i++) {
	}

	return XST_SUCCESS;
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
static int SetupIntrSystem(XAxiVdma *AxiVdmaPtr, u16 ReadIntrId,
				u16 WriteIntrId)
{
	int Status;

#ifdef XPAR_INTC_0_DEVICE_ID
	XIntc *IntcInstancePtr =&Intc;


	/* Initialize the interrupt controller and connect the ISRs */
	Status = XIntc_Initialize(IntcInstancePtr, INTC_DEVICE_ID);
	if (Status != XST_SUCCESS) {

		xil_printf( "Failed init intc\r\n");
		return XST_FAILURE;
	}

	Status = XIntc_Connect(IntcInstancePtr, ReadIntrId,
	         (XInterruptHandler)XAxiVdma_ReadIntrHandler, AxiVdmaPtr);
	if (Status != XST_SUCCESS) {

		xil_printf(
		    "Failed read channel connect intc %d\r\n", Status);
		return XST_FAILURE;
	}

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
	XIntc_Enable(IntcInstancePtr, ReadIntrId);
	XIntc_Enable(IntcInstancePtr, WriteIntrId);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_InterruptHandler,
			(void *)IntcInstancePtr);

	Xil_ExceptionEnable();

#else

	XScuGic *IntcInstancePtr = &Intc;	/* Instance of the Interrupt Controller */
	XScuGic_Config *IntcConfig;


	/*
	 * Initialize the interrupt controller driver so that it is ready to
	 * use.
	 */
	IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	if (NULL == IntcConfig) {
		return XST_FAILURE;
	}

	Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
					IntcConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	XScuGic_SetPriorityTriggerType(IntcInstancePtr, ReadIntrId, 0xA0, 0x3);
	XScuGic_SetPriorityTriggerType(IntcInstancePtr, WriteIntrId, 0xA0, 0x3);

	/*
	 * Connect the device driver handler that will be called when an
	 * interrupt for the device occurs, the handler defined above performs
	 * the specific interrupt processing for the device.
	 */
	Status = XScuGic_Connect(IntcInstancePtr, ReadIntrId,
				(Xil_InterruptHandler)XAxiVdma_ReadIntrHandler,
				AxiVdmaPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	Status = XScuGic_Connect(IntcInstancePtr, WriteIntrId,
				(Xil_InterruptHandler)XAxiVdma_WriteIntrHandler,
				AxiVdmaPtr);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	/*
	 * Enable the interrupt for the DMA device.
	 */
	XScuGic_Enable(IntcInstancePtr, ReadIntrId);
	XScuGic_Enable(IntcInstancePtr, WriteIntrId);

	Xil_ExceptionInit();

	/*
	 * Connect the interrupt controller interrupt handler to the hardware
	 * interrupt handling logic in the processor.
	 */
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
				(Xil_ExceptionHandler)XScuGic_InterruptHandler,
				IntcInstancePtr);


	/*
	 * Enable interrupts in the Processor.
	 */
	Xil_ExceptionEnable();


#endif

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
static void DisableIntrSystem(u16 ReadIntrId, u16 WriteIntrId)
{

#ifdef XPAR_INTC_0_DEVICE_ID
	XIntc *IntcInstancePtr =&Intc;

	/* Disconnect the interrupts for the DMA TX and RX channels */
	XIntc_Disconnect(IntcInstancePtr, ReadIntrId);
	XIntc_Disconnect(IntcInstancePtr, WriteIntrId);
#else
	XScuGic *IntcInstancePtr = &Intc;

	XScuGic_Disable(IntcInstancePtr, ReadIntrId);
	XScuGic_Disable(IntcInstancePtr, WriteIntrId);

	XScuGic_Disconnect(IntcInstancePtr, ReadIntrId);
	XScuGic_Disconnect(IntcInstancePtr, WriteIntrId);
#endif
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
		WriteError += 1;
	}
}

