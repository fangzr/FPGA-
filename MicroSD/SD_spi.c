/*
 * SD_spi.c
 *
 *  Created on: 2012-4-2
 *      Author: Ming
 */

#include"SD_spi.h"

//-----------------------------------------------------------//
//Function Name	: InitSD
//description	: ��ʼ��SD����������λ��spiģʽ��ʼ����spi�����ٶ��趨
//Input			: None
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 InitSD()
{
	u8 i = 0;
	while(SDReset() != 0){		//��λSD��
		i ++;
		if(i >= 20) return 1;
	}
	i = 0;
	while(SDInit() != 0){      //��ʼ��Ϊspiģʽ
		i ++;
		if(i >= 20) return 1;
	}
	spi_speed = 0;				//����spi�����ٶ�

	return 0;
}
//-----------------------------------------------------------//
//Function Name	: ReadSec
//description	: ��SD��ָ������
//Input			: u32 Sec_Index ��������
//Output		: u8 * buffer ��ָ���Ŷ�ȡ���ݻ�������ָ��
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 ReadSec(u32 Sec_Index,u8 * buffer)
{
	return SDReadSector(Sec_Index,buffer);
}
//-----------------------------------------------------------//
//Function Name	: WriteSec
//description	: дSD��ָ������
//Input			: u32 Sec_Index ��������
//				  u8 * buffer ��Ҫд���ݵ�ָ��
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 WriteSec(u32 Sec_Index,u8 * buffer)
{

	return SDWriteSector(Sec_Index,buffer);
}
