/*
 * SD_spi.h
 *
 *  Created on: 2012-4-2
 *      Author: Ming
 */

#ifndef SD_SPI_H_
#define SD_SPI_H_

#include"SD_spi_Solution.h"

//-----------------------------------------------------------//
//Function Name	: InitSD
//description	: 初始化SD卡（包括复位，spi模式初始化和spi工作速度设定
//Input			: None
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 InitSD();

//-----------------------------------------------------------//
//Function Name	: ReadSec
//description	: 读SD卡指定扇区
//Input			: u32 Sec_Index ：扇区号
//Output		: u8 * buffer ：指向存放读取数据缓冲区的指针
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 ReadSec(u32 Sec_Index,u8 * buffer);

//-----------------------------------------------------------//
//Function Name	: WriteSec
//description	: 写SD卡指定扇区
//Input			: u32 Sec_Index ：扇区号
//				  u8 * buffer ：要写数据的指针
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 WriteSec(u32 Sec_Index,u8 * buffer);

#endif /* SD_SPI_H_ */
