/*
 * gt9147.c
 *
 *  Created on: 2017年3月10日
 *      Author: Peter
 */
#if 1
#include <stdio.h>
#include <xparameters.h>
#include<microblaze_sleep.h>
#include <xgpio.h>
#include "gt9147.h"
#include "../CTIIC/ctiic.h"
#include <xil_io.h>
#include <string.h>
#include "touch.h"
u8 keysts_;

void delay_ms(int times){
	MB_Sleep(times);
}
//GT9147配置参数表
//第一个字节为版本号(0X60),必须保证新的版本号大于等于GT9147内部
//flash原有版本号,才会更新配置.
const u8 GT9147_CFG_TBL[]=
{
	0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
	0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
	0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
	0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
	0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
	0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
	0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
	0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
	0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
	0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
	0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
	0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
	0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
	0XFF,0XFF,0XFF,0XFF,
};
//发送GT9147配置参数
//mode:0,参数不保存到flash
//     1,参数保存到flash
u8 GT9147_Send_Cfg(u8 mode)
{
	u8 buf[2];
	u8 i=0;
	buf[0]=0;
	buf[1]=mode;	//是否写入到GT9147 FLASH?  即是否掉电保存
	for(i=0;i<sizeof(GT9147_CFG_TBL);i++)buf[0]+=GT9147_CFG_TBL[i];//计算校验和
    buf[0]=(~buf[0])+1;
	GT9147_WR_Reg(GT_CFGS_REG,(u8*)GT9147_CFG_TBL,sizeof(GT9147_CFG_TBL));//发送寄存器配置
	GT9147_WR_Reg(GT_CHECK_REG,buf,2);//写入校验和,和配置更新标记
	return 0;
}
//向GT9147写入一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:写数据长度
//返回值:0,成功;1,失败.
u8 GT9147_WR_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
	u8 ret=0;
	CT_IIC_Start();
 	CT_IIC_Send_Byte(GT_CMD_WR);   	//发送写命令
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte(reg>>8);   	//发送高8位地址
	CT_IIC_Wait_Ack();
	CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	CT_IIC_Wait_Ack();
	for(i=0;i<len;i++)
	{
    	CT_IIC_Send_Byte(buf[i]);  	//发数据
		ret=CT_IIC_Wait_Ack();
		if(ret)break;
	}
    CT_IIC_Stop();					//产生一个停止条件
	return ret;
}
//从GT9147读出一次数据
//reg:起始寄存器地址
//buf:数据缓缓存区
//len:读数据长度
void GT9147_RD_Reg(u16 reg,u8 *buf,u8 len)
{
	u8 i;
 	CT_IIC_Start();
 	CT_IIC_Send_Byte(GT_CMD_WR);   //发送写命令
	CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg>>8);   	//发送高8位地址
	CT_IIC_Wait_Ack();
 	CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
	CT_IIC_Wait_Ack();
 	CT_IIC_Start();
	CT_IIC_Send_Byte(GT_CMD_RD);   //发送读命令
	CT_IIC_Wait_Ack();
	for(i=0;i<len;i++)
	{
    	buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //发数据
	}
    CT_IIC_Stop();//产生一个停止条件
}
//初始化GT9147触摸屏
//返回值:0,初始化成功;1,初始化失败
u8 GT9147_Init(void)
{
	u8 temp[5];
	CT_IIC_Init();      	//初始化电容屏的I2C总线
	Xil_Out32(XPAR_HIER_PERIPH_TFT_GPIO_TCS_BASEADDR ,0x00);			//复位
	delay_ms(10);
	Xil_Out32(XPAR_HIER_PERIPH_TFT_GPIO_TCS_BASEADDR ,0xff);			//释放复位
 	delay_ms(10);
 	delay_ms(100);
	GT9147_RD_Reg(GT_PID_REG,temp,4);//读取产品ID
	temp[4]=0;
	printf("CTP ID:%s\r\n",temp);	//打印ID
	if(strcmp((char*)temp,"9147")==0)//ID==9147
	{
		temp[0]=0X02;
		GT9147_WR_Reg(GT_CTRL_REG,temp,1);//软复位GT9147
		GT9147_RD_Reg(GT_CFGS_REG,temp,1);//读取GT_CFGS_REG寄存器
		if(temp[0]<0X60)//默认版本比较低,需要更新flash配置
		{
			printf("Default Ver:%d\r\n",temp[0]);
			GT9147_Send_Cfg(1);//更新并保存配置
		}
		delay_ms(10);
		temp[0]=0X00;
		GT9147_WR_Reg(GT_CTRL_REG,temp,1);//结束复位
		return 0;
	}
	return 1;
}

//扫描触摸屏(采用查询方式)
//mode:0,正常扫描.
//返回值:当前触屏状态.
//0,触屏无触摸;1,触屏有触摸
u8 GT9147_Scan(int *x, int *y)
{
	u8 buf[4]={0};
	u8 keysts=0;
	u8 temp;

	GT9147_RD_Reg(GT_GSTID_REG,&temp,1);//读取触摸点的状态
	tp_dev.sta=temp;

	if(temp&0x0F)
	{
		keysts=1;
		GT9147_RD_Reg(0X8150,buf,4);	//读取XY坐标值
		*y=479-((u16)buf[1]<<8)-buf[0];
		*x=((u16)buf[3]<<8)+buf[2];
		//printf("=%d,y=%d\n",*x,*y);
		tp_dev.sta=1;
		//tp_dev.x[0]=x;tp_dev.y[0]=y;
	}
	if(*x < 0 || *x > 800 || *y < 0 || *y > 480){
		*x = -1;
		*y = -1;
	}
	temp=0;tp_dev.sta=0;
	GT9147_WR_Reg(GT_GSTID_REG,&temp,1);//清标志

	if(keysts>keysts_){temp=1;tp_dev.sta=1;}
	keysts_=keysts;


	return temp;
}

#endif


