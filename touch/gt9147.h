/*
 * gt9147.h
 *
 *  Created on: 2017年3月10日
 *      Author: Peter
 */

#ifndef GT9147_H_
#define GT9147_H_

#if 1

#ifndef __OTT2001A_H
#define __OTT2001A_H
//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//4.3寸电容触摸屏驱动-OTT2001A部分
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/11
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//无
//////////////////////////////////////////////////////////////////////////////////
#include "../CTIIC/ctiic.h"
#include <stdio.h>
#include<microblaze_sleep.h>
#include <xgpio.h>



//I2C读写命令
#define GT_CMD_WR 		0X28     	//写命令
#define GT_CMD_RD 		0X29		//读命令

//GT9147 部分寄存器定义
#define GT_CTRL_REG 	0X8040   	//GT9147控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT9147配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT9147校验和寄存器
#define GT_PID_REG 		0X8140   	//GT9147产品ID寄存器

#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址


u8 GT9147_Send_Cfg(u8 mode);
u8 GT9147_WR_Reg(u16 reg,u8 *buf,u8 len);
void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len);
u8 GT9147_Init(void);
u8 GT9147_Scan(int *x, int *y);
void delay_ms(int times);

#endif


#endif











#endif /* GT9147_H_ */
