
/*
 * ctiic.h
 *
 *  Created on: 2016年12月13日
 *      Author: Victor_Jiang
 */

#include <stdlib.h>
#include <stdio.h>
#include <xgpio.h>
#include <xil_io.h>





#define CT_SDA_OUT()  *(u32 *)(SDA_BASE_ADDR + 0x04)=0x00//IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_IIC_SDA_BASE, ALTERA_AVALON_PIO_DIRECTION_OUTPUT)
#define CT_SDA_IN()  *(u32 *)(SDA_BASE_ADDR + 0x04)=0xff//IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_IIC_SDA_BASE, ALTERA_AVALON_PIO_DIRECTION_INPUT)

//IIC所有操作函数
void CT_IIC_Init(void);                	//初始化IIC的IO口
void CT_IIC_Start(void);				//发送IIC开始信号
void CT_IIC_Stop(void);	  				//发送IIC停止信号
void CT_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 CT_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
u8 CT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void CT_IIC_Ack(void);					//IIC发送ACK信号
void CT_IIC_NAck(void);					//IIC不发送ACK信号



