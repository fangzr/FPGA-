/*
 * SD_spi.c
 *
 *  Created on: 2012-4-2
 *      Author: Ming
 */

#include"SD_spi.h"

//-----------------------------------------------------------//
//Function Name	: InitSD
//description	: 初始化SD卡（包括复位，spi模式初始化和spi工作速度设定
//Input			: None
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 InitSD()
{
	u8 i = 0;
	while(SDReset() != 0){		//复位SD卡
		i ++;
		if(i >= 20) return 1;
	}
	i = 0;
	while(SDInit() != 0){      //初始化为spi模式
		i ++;
		if(i >= 20) return 1;
	}
	spi_speed = 0;				//设置spi工作速度

	return 0;
}
//-----------------------------------------------------------//
//Function Name	: ReadSec
//description	: 读SD卡指定扇区
//Input			: u32 Sec_Index ：扇区号
//Output		: u8 * buffer ：指向存放读取数据缓冲区的指针
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 ReadSec(u32 Sec_Index,u8 * buffer)
{
	return SDReadSector(Sec_Index,buffer);
}
//-----------------------------------------------------------//
//Function Name	: WriteSec
//description	: 写SD卡指定扇区
//Input			: u32 Sec_Index ：扇区号
//				  u8 * buffer ：要写数据的指针
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 WriteSec(u32 Sec_Index,u8 * buffer)
{

	return SDWriteSector(Sec_Index,buffer);
}
