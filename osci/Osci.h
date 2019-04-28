/*
 * Osci.h
 *
 *  Created on: 2017年6月30日
 *      Author: Peter
 */

#ifndef SRC_OSCI_OSCI_H_
#define SRC_OSCI_OSCI_H_

#define rd_data_clk_Regs OSCILLOSCOPE_S00_AXI_SLV_REG0_OFFSET
#define level_Regs 		 OSCILLOSCOPE_S00_AXI_SLV_REG1_OFFSET
#define start_Regs		 OSCILLOSCOPE_S00_AXI_SLV_REG2_OFFSET
#define Precnt_Regs		 OSCILLOSCOPE_S00_AXI_SLV_REG3_OFFSET
#define clkdiv_Regs		 OSCILLOSCOPE_S00_AXI_SLV_REG4_OFFSET
#define timeout_flg_Regs OSCILLOSCOPE_S00_AXI_SLV_REG5_OFFSET
#define dout_Regs		 OSCILLOSCOPE_S00_AXI_SLV_REG6_OFFSET
#define Oscilloscope_ADDR 0x44A10000
#define AD_Regs			 OSCILLOSCOPE_S00_AXI_SLV_REG7_OFFSET

#include "Oscilloscope.h"

extern volatile u8 data[600];
extern volatile u8 datalast[600];
extern volatile u8 level;
extern volatile u8 clkdiv;
extern volatile u8 yscale;
extern volatile u8 yscale_last;

//初始化示波器状态机
//输入1.触发电平 2.时钟分频大小
void InitOsci(u8 level,u8 clkdiv);

//启动一次数据采集
//使Start信号保持1一段时间
void StartOsci();

//采集结束判断函数
//返回为1则为准备好数据读取
//返回为0则为未准备好读取
u8 Readyread();

//采集一次数据，先判断是否读有效，如果查询100次还是没有数据读取，则放弃本次采集
//输入fifo500个数据给数组
u8 SamplingFifo();

//在480*800的TFT屏幕（横屏）绘制波形
void DrawWaveform();
void oscilloscope(u8 level,u8 clkdiv);

#endif /* SRC_OSCI_OSCI_H_ */
