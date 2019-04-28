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
//description	: ��ʼ��SD����������λ��spiģʽ��ʼ����spi�����ٶ��趨
//Input			: None
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 InitSD();

//-----------------------------------------------------------//
//Function Name	: ReadSec
//description	: ��SD��ָ������
//Input			: u32 Sec_Index ��������
//Output		: u8 * buffer ��ָ���Ŷ�ȡ���ݻ�������ָ��
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 ReadSec(u32 Sec_Index,u8 * buffer);

//-----------------------------------------------------------//
//Function Name	: WriteSec
//description	: дSD��ָ������
//Input			: u32 Sec_Index ��������
//				  u8 * buffer ��Ҫд���ݵ�ָ��
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 WriteSec(u32 Sec_Index,u8 * buffer);

#endif /* SD_SPI_H_ */
