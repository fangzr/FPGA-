/*
 * ctiic.c
 *
 *  Created on: 2017年3月10日
 *      Author: Peter
 */
/*
 * ctiic.c
 *
 *  Created on: 2016年12月13日
 *      Author: Victor_Jiang
 */
#include "ctiic.h"
#include "stdlib.h"
#include <xil_io.h>
#include "xil_types.h"
#include "../tft/MB_USLEEP.h"
#define SDA_BASE_ADDR 0x40090000
#define SCL_BASE_ADDR 0x400B0000
#define ALT_USLEEP MB_USLEEP
#define usleep MB_USLEEP

#define IIC_RLS_SDA     do\
                        {\
                            *(u32 *)(SDA_BASE_ADDR + 0x04)=0xff;\
                        }while(0)                        //float input,with external 4.7Kohm resistor pulling up
#define IIC_CLR_SDA     do\
                        {\
                            *(u32 *)(SDA_BASE_ADDR)=0x00;\
                           *(u32 *)(SDA_BASE_ADDR + 0x04)=0x00;\
                        }while(0)                        //output LOW
#define IIC_RLS_SCL     do\
                        {\
                             *(u32 *)(SCL_BASE_ADDR + 0x04)=0xff;\
                        }while(0)                        //float input,with external 4.7Kohm resistor pulling up
#define IIC_CLR_SCL     do\
                        {\
                            *(u32 *)(SCL_BASE_ADDR)=0x00;\
                           *(u32 *)(SCL_BASE_ADDR + 0x04)=0x00;\
                        }while(0)                        //output LOW

#define IIC_SDA_VAL     *(u32 *)(SDA_BASE_ADDR)//(IORD_ALTERA_AVALON_PIO_DATA(PIO_IIC_SDA_BASE))
#define IIC_SCL_VAL    *(u32 *)(SCL_BASE_ADDR)
#define IIC_SCL_SYNC    while(!IIC_SCL_VAL)
#define CT_READ_SDA 	IIC_SDA_VAL

#define IIC_NAKRTN      do\
                        {\
                            if(!IIC_ReceiveACK())\
                            {\
                                IIC_Stop();\
                                return(0);\
                            }\
                        }while(0)



//控制I2C速度的延时
void CT_Delay(void)
{
	MB_USLEEP(5);
}
//电容触摸芯片IIC接口初始化
void CT_IIC_Init(void)
{
    IIC_RLS_SCL;
    IIC_RLS_SDA;
    CT_IIC_Stop();
//    return IIC_GeneralReset();
}
//产生IIC起始信号
void CT_IIC_Start(void)
{
    IIC_RLS_SDA;
    usleep(3);
    IIC_RLS_SCL;
    usleep(4);        //Start condition set up time >4.7us
    IIC_SCL_SYNC;
    IIC_CLR_SDA;    //Send Start signal
    usleep(4);        //Start condition lock time >4us
    IIC_CLR_SCL;    //Seize Bus.Prepare to send or receive data
}
//产生IIC停止信号
void CT_IIC_Stop(void)
{
    IIC_CLR_SDA;
    usleep(3);
    IIC_RLS_SCL;
    usleep(4);        //Stop condition set up time >4us
    IIC_SCL_SYNC;
    IIC_RLS_SDA;    //Send Stop signal
    usleep(4);
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 CT_IIC_Wait_Ack(void)
{
    unsigned char IIC_Ack;
    IIC_RLS_SDA;
    usleep(3);
    IIC_RLS_SCL;
    usleep(3);
    IIC_SCL_SYNC;
    IIC_Ack=!IIC_SDA_VAL;        //Whether received acknowledge
    IIC_CLR_SCL;                //Seize Bus
    return IIC_Ack;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void CT_IIC_Send_Byte(u8 c)
{
    unsigned char BitCnt;
    for(BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        if(c & 0x80)
            IIC_RLS_SDA;
        else
            IIC_CLR_SDA;
        c <<= 1;
        usleep(2);
        IIC_RLS_SCL;            //Pull SCL High.Order device to receive data bits
        usleep(3);                //the High period of SCL >4us
        IIC_SCL_SYNC;
        IIC_CLR_SCL;
    }
}

//Acknowledge    a=1:ACK;    a=0:NAK
void IIC_Acknowledge(char a)
{
    if(a)
        IIC_CLR_SDA;
    else
        IIC_RLS_SDA;    //Send ACK or NAK
    usleep(2);
    IIC_RLS_SCL;        //the High period of SCL >4us
    usleep(3);
    IIC_SCL_SYNC;
    IIC_CLR_SCL;        //Seize Bus
}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
u8 CT_IIC_Read_Byte(unsigned char ack)
{
    unsigned char Data;
    unsigned char BitCnt;
    Data = 0;
    IIC_RLS_SDA;
    for(BitCnt = 0; BitCnt < 8; BitCnt++)
    {
        usleep(3);
        IIC_RLS_SCL;
        usleep(2);
        IIC_SCL_SYNC;
        Data <<= 1;
        Data |= IIC_SDA_VAL;    //Receive data bits,put them in "Data"
        IIC_CLR_SCL;                //the Low period of SCL >4.7us
    }
	IIC_Acknowledge(ack);
 	return Data;
}



