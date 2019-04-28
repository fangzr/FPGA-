/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
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
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* XILINX CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <xparameters.h>
#include <xgpio.h>
#include <xintc.h>
#include <xtmrctr.h>
#include <xuartlite.h>
#include "platform.h"
#include "keys/keys.h"
#include<microblaze_sleep.h>
#include "tft/lcd.h"
#include "tft/MB_USLEEP.h"
#include "touch/touch.h"
#include"Measure_Freq.h"
#include "osci/Osci.h"
#include"math.h"
#include "osci/fft.h"
#include "MicroSD/SD_spi_Solution.h"
#include "MicroSD/Fat16.h"
#define delay_ms MB_Sleep



u8 y[200]={0};
char x_div_strstr[20];

XGpio gpioLed;
XIntc intc;
XTmrCtr timer;

void InitIntc()
{
	int sts;

	sts = 	XIntc_Initialize (&intc, XPAR_HIER_PERIPH_MICROBLAZE_0_AXI_INTC_DEVICE_ID);
	if(sts != XST_SUCCESS) printf("intc init error!\n");
	XIntc_Start (&intc,XIN_REAL_MODE);
	if(sts !=XST_SUCCESS) printf("intc start error!\n");

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(
			XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler)XIntc_InterruptHandler,
			&intc
	);
    Xil_ExceptionEnable();
}




void Timer_ISR(void *arg, u8 num)
{
//	oscilloscope(level,clkdiv);
	InitOsci(level,clkdiv);	//初始化示波器控制状态机

	StartOsci();			//开始采样
	MB_Sleep(10);
	SamplingFifo();			//读取数据
	MB_Sleep(200);

	FFT_Top();
	Draw_FFT();
}


void InitTimer()
{
	XTmrCtr_Initialize (&timer, XPAR_HIER_PERIPH_AXI_TIMER_0_DEVICE_ID);
	XTmrCtr_SetResetValue (&timer, 0, XPAR_HIER_PERIPH_AXI_TIMER_0_CLOCK_FREQ_HZ-1);//we want our timer sent a interrupt per 10ms,because it declines till 0,so we -1
	XTmrCtr_SetOptions (&timer, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION | XTC_DOWN_COUNT_OPTION);
	XTmrCtr_SetHandler (&timer, Timer_ISR, (void *)&timer);

	XIntc_Connect (&intc, XPAR_INTC_0_TMRCTR_0_VEC_ID, XTmrCtr_InterruptHandler, (void *)&timer);
	XIntc_Enable (&intc, XPAR_INTC_0_TMRCTR_0_VEC_ID);
	XIntc_Acknowledge (&intc, XPAR_INTC_0_TMRCTR_0_VEC_ID);

	XIntc_Start (&intc, 0);
	XTmrCtr_Start (&timer, 0);
	XIntc_Start (&intc, 1);
	XTmrCtr_Start (&timer, 1);
}



int main()
{
	int i,j=0;
	int x,y;
	float x_div=0;
	float Fs=0.f;
	int decpt,sign;
	u8 yscale_class=10;
    init_platform();

    InitIntc();



//	LCD_Init();
//	LCD_DisplayOn();
//	LCD_CalDispaly();
//	tp_dev.init();

	InitTimer();

	BACK_COLOR = BLACK;
	POINT_COLOR=YELLOW;
	InitOsci(level,clkdiv);	//初始化示波器控制状态机
	Init_fftsintable();//FFT初始化
//	SD_Test();
	while(1)
	{
		Xil_Out8(SCLK_BASE,0xff);
		MB_Sleep(100);
		Xil_Out8(SCLK_BASE,0x00);
		MB_Sleep(100);
	}
	while(1)
		{
			//GT9147_Scan(&y,&x);

			tp_dev.scan(0);
				if((tp_dev.sta))
				{

					GT9147_Scan(&x,&y);
					y=479-y;
					x=799-x;
					if(x<800 && y<480)
					{
//						GT9147_Scan(&x,&y);
//						y=479-y;
//						x=799-x;
						delay_ms(80);
						GT9147_Scan(&x,&y);
						y=479-y;
						x=799-x;

						}
					switch(TP_judgment(x,y))
					{
						case YscaleUp:
							if(yscale<8)
							{
								yscale_last = yscale;
								yscale = yscale+1;
								yscale_class--;
								LCD_Fill(0,0,800,350,BLACK);
								LCD_ShowxNum(133,370,yscale_class,4,24,0);
							}

							break;
						case YscaleDown:
							if(yscale>0)
							{
								yscale_last = yscale;
								yscale = yscale-1;
								yscale_class++;
								LCD_Fill(0,0,800,350,BLACK);
								LCD_ShowxNum(133,370,yscale_class,4,24,0);
							}
							break;
						case XscaleUp:
						{
							(clkdiv) = (clkdiv) + 1;
							Fs = (100000000/((float)(clkdiv)+1))/2;
							Fs = Fs/20000000;
//								x_div = 1/(10*Fs)*1000000.f;
								 char *x_div_str = fcvt(Fs, 6, &decpt, &sign);
								 changechar(x_div_strstr,x_div_str,6,decpt,sign);
								 LCD_ShowString(350,370,200,12,24,x_div_strstr);
								 LCD_ShowString(450,370,800,480,12,"MHz/DIV");
								 //LCD_showString(450,370,"MHz/DIV");
							break;
						}

						case XscaleDown:
						if((clkdiv)>0)
						{
							(clkdiv) = (clkdiv) - 1;
							Fs = (100000000/((float)(clkdiv)+1))/2;
							Fs = Fs/20000000;
//							x_div = 1/(10*Fs)*1000000.f;
							 char *x_div_str = fcvt(Fs, 6, &decpt, &sign);
							 changechar(x_div_strstr,x_div_str,6,decpt,sign);
							 LCD_ShowString(350,370,200,12,24,x_div_strstr);
							 LCD_ShowString(450,370,800,480,12,"MHz/DIV");
						}

							break;
						case LevelUp	:
						if((level)<253)
						{
							(level) = (level) +2;
							LCD_ShowxNum(655,370,level,4,24,0);
						}

							break;
						case LevelDown	:
						if((level)>2)
						{
							(level) = (level)-2;
							LCD_ShowxNum(655,370,level,4,24,0);
						}

							break;
						default: break;
					}
						delay_ms(100);
				}

			//printf("sta=%x\n",tp_dev.sta);
		}


	while(1);

    cleanup_platform();
    return 0;
}
