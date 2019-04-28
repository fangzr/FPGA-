
/*
 * ctiic.h
 *
 *  Created on: 2016��12��13��
 *      Author: Victor_Jiang
 */

#include <stdlib.h>
#include <stdio.h>
#include <xgpio.h>
#include <xil_io.h>





#define CT_SDA_OUT()  *(u32 *)(SDA_BASE_ADDR + 0x04)=0x00//IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_IIC_SDA_BASE, ALTERA_AVALON_PIO_DIRECTION_OUTPUT)
#define CT_SDA_IN()  *(u32 *)(SDA_BASE_ADDR + 0x04)=0xff//IOWR_ALTERA_AVALON_PIO_DIRECTION(PIO_IIC_SDA_BASE, ALTERA_AVALON_PIO_DIRECTION_INPUT)

//IIC���в�������
void CT_IIC_Init(void);                	//��ʼ��IIC��IO��
void CT_IIC_Start(void);				//����IIC��ʼ�ź�
void CT_IIC_Stop(void);	  				//����IICֹͣ�ź�
void CT_IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 CT_IIC_Read_Byte(unsigned char ack);	//IIC��ȡһ���ֽ�
u8 CT_IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void CT_IIC_Ack(void);					//IIC����ACK�ź�
void CT_IIC_NAck(void);					//IIC������ACK�ź�



