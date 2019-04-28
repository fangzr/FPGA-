/*
 * Osci.c
 *
 *  Created on: 2017年6月30日
 *      Author: Peter
 */
#include"Osci.h"
#include"microblaze_sleep.h"
#include "../tft/lcd.h"

#define FIFO_CONTROL_mWriteReg OSCILLOSCOPE_mWriteReg
#define FIFO_CONTROL_mReadReg OSCILLOSCOPE_mReadReg

#define rd_data_clk_1 FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, rd_data_clk_Regs, 0x01)
#define rd_data_clk_0 FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, rd_data_clk_Regs, 0x00)



volatile u8 data[600]={0};
volatile u8 datalast[600]={0};

volatile u8 level=80;
volatile u8 clkdiv = 0;
volatile u8 yscale = 0;
volatile u8 yscale_last=0;

#define xdisplay 2
//初始化示波器状态机
//输入1.触发电平 2.时钟分频大小
void InitOsci(u8 level,u8 clkdiv)
{
	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, level_Regs, level);
	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, clkdiv_Regs, clkdiv);

	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, Precnt_Regs, 250);
	rd_data_clk_0;
}

//启动一次数据采集
//使Start信号保持1一段时间
void StartOsci()
{
	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, start_Regs, 0x01);
	MB_Sleep(1);
	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, start_Regs, 0x00);
}

//采集结束判断函数
//返回为1则为准备好数据读取
//返回为0则为未准备好读取
u8 Readyread()
{
	return FIFO_CONTROL_mReadReg(Oscilloscope_ADDR, timeout_flg_Regs);
}

//采集一次数据，先判断是否读有效，如果查询100次还是没有数据读取，则放弃本次采集
//输入fifo500个数据给数组
u8 SamplingFifo()
{
	int i=0;
	while((Readyread()!=1))
	{
		i++;
		if(i>100)return 0;
	}
		for(i=0;i<500;i++)
		{
			rd_data_clk_1;
			datalast[i]=data[i];
			rd_data_clk_0;
			data[i] = ((((char)(FIFO_CONTROL_mReadReg(Oscilloscope_ADDR, dout_Regs)))+128)>>yscale);
		}
		return 1;
}

//在480*800的TFT屏幕（横屏）绘制波形
void DrawWaveform()
{
	int i,j=0;

	//擦除上次绘制的波形
	POINT_COLOR=BLACK;
	for(i=0,j=0;i<400 && j<800;i++)
	{
		if(datalast[i]!=data[i] || datalast[i+1]!=data[i+1])
		{
			LCD_DrawLine(j,(datalast[i]+100), j+xdisplay,(datalast[i+1]+100));

		}
		j=j+xdisplay;
	}
	//绘制波形
	POINT_COLOR=YELLOW;
	for(i=0,j=0;i<400 && j<800;i++)
	{
		LCD_DrawLine(j,(data[i]+100), j+xdisplay, (data[i+1]+100));
		j=j+xdisplay;
	}

}

void oscilloscope(u8 level,u8 clkdiv)
{
	u8 i;
	InitOsci(level,clkdiv);	//初始化示波器控制状态机
	StartOsci();			//开始采样
	MB_Sleep(10);
	i=SamplingFifo();			//读取数据
	MB_Sleep(200);
	if(i!=0)
		DrawWaveform();			//绘制波形
}
