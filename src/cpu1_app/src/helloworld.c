/******************************************************************************
 *
 * Copyright (C) 2002 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/
/*****************************************************************************/
/**
 * @file xgpio_example.c
 *
 * This file contains a design example using the GPIO driver (XGpio) and hardware
 * device.  It only uses a channel 1 of a GPIO device.
 *
 * This example can be ran on the Xilinx ML300 board using the Prototype Pins &
 * LEDs of the board connected to the GPIO.
 *
 * <pre>
 * MODIFICATION HISTORY:
 *
 * Ver   Who  Date     Changes
 * ----- ---- -------- -----------------------------------------------
 * 1.00a rmm  03/13/02 First release
 * 1.00a rpm  08/04/03 Removed second example and invalid macro calls
 * 2.00a jhl  12/15/03 Added support for dual channels
 * 2.00a sv   04/20/05 Minor changes to comply to Doxygen and coding guidelines
 * 3.00a ktn  11/20/09 Minor changes as per coding guidelines.
 *
 * </pre>
 ******************************************************************************/

/***************************** Include Files *********************************/

#include "xparameters.h"
#include "xgpio.h"
#include "xparameters.h"
#include <stdio.h>
#include "xil_io.h"
#include "xil_mmu.h"
#include "xil_cache.h"
#include "xil_printf.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "sleep.h"
#include "xpseudo_asm.h"

/************************** Constant Definitions *****************************/

#define LED 0x01   /* Assumes bit 0 of GPIO is connected to an LED  */

/*
 * The following constant maps to the name of the hardware instances that
 * were created in the EDK XPS system.
 */
#define GPIO_EXAMPLE_DEVICE_ID  0

/*
 * The following constant is used to wait after an LED is turned on to make
 * sure that it is visible to the human eye.  This constant might need to be
 * tuned for faster or slower processor speeds.
 */
#define LED_DELAY     10000000
#define COMM_VAL        (*(volatile unsigned long *)(0xFFFF8000))

#define LED_CHANNEL 1

XGpio Gpio; /* The Instance of the GPIO Driver */

extern u32 MMUTable;

void MyXil_DCacheFlush(void) {
	Xil_L1DCacheFlush();
	//Xil_L2CacheFlush();
}

void MyXil_SetTlbAttributes(u32 addr, u32 attrib) {
	u32 *ptr;
	u32 section;

	mtcp(XREG_CP15_INVAL_UTLB_UNLOCKED, 0);
	dsb();

	mtcp(XREG_CP15_INVAL_BRANCH_ARRAY, 0);
	dsb();
	MyXil_DCacheFlush();

	section = addr / 0x100000;
	ptr = &MMUTable + section;
	*ptr = (addr & 0xFFF00000) | attrib;
	dsb();
}

int main(void) {
	u32 Data;
	int Status;
	volatile int Delay;
	MyXil_SetTlbAttributes(0xFFFF0000, 0x04de2);
	COMM_VAL = 0;
	/*
	 * Initialize the GPIO driver
	 */
	Status = XGpio_Initialize(&Gpio, GPIO_EXAMPLE_DEVICE_ID);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	/*
	 * Set the direction for all signals to be inputs except the
	 * LED output
	 */
	XGpio_SetDataDirection(&Gpio, LED_CHANNEL, 0x00);

	/* Loop forever blinking the LED */

	while (1) {

		XGpio_DiscreteWrite(&Gpio, LED_CHANNEL, 0x01);

		for (Delay = 0; Delay < LED_DELAY; Delay++)
			;

		XGpio_DiscreteClear(&Gpio, LED_CHANNEL, LED);
		//XGpio_DiscreteSet(&Gpio, LED_CHANNEL, LED);

		COMM_VAL += 1;
		sleep(1);
	}

	return XST_SUCCESS;
}

