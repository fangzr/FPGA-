/*
 * Fat16.h
 *
 *  Created on: 2012-2-24
 *      Author: Ming
 */

#ifndef FAT16_H_
#define FAT16_H_

#include<stdlib.h>
#include"SD_spi.h"


#define TRUE		0
#define	FALSE		1
#define MBR_ADDR	0
#define NULL		0
#define BLOCK_SIZE  512
#define NAME_ERROR  2

//---------------------------------------------------------------//
			//BPB(BIOS Parameter Block)��Ϣ//
typedef struct{
	u16 BytesPerSec;	//ÿ�����������ֽ�
	u8	SecPerClus; 	//ÿ�����ж��ٸ�����
	u16	RsvdSecNum;		//����������
	u8	FATsNum;		//�ж��ٸ�FAT��
	u16	RootEntNum;		//��Ŀ¼����ĵǼ�����Ŀ
	u16	SecPerFAT;  	//ÿ��FAT���ж��ٸ�����
	u16	HiddSec;		//���ص�������
	u32 SecNum; 		//�ܵ�������
} FAT_BPB;

//--------------------------------------------------------------//
			 //��Ҫ�����ƫ�Ƶ�ַ//
typedef struct{
	u32 Logic;			//�����ȣ��߼�����0��������0�������ƫ�Ƶ�ַ
	u32 FAT1;			//FAT1
	u32	FAT2;			//FAT2
	u32	FDT;			//��Ŀ¼��FDT��File Directory Table��
	u32	Clus;			//���ݴ�
} FAT_OFFSET;

//------------------------------------------------------------//
	// 8.3 file name//
typedef struct{
	u8 Name[8];
	u8 Type[3];
} FILE_NAME;
//-------------------------------------------------------------//
// 8.3 file name entry:
//=========================================
//offset  length  description
//  0       8  		name (space padded)
//  8       3  		extension (space padded)
//  11      1  		attributes (FAT16_ATTRIB_*)
//	12		1		reserved
//	13		1		creating time(the ms)
//	14		2   	creating time(the s)
//  16		2		creating date
//	18		2		access date
//	20		2		addressH
//	22		2		updating time
//	24		2		updating date
//	26		2		addressL
//	28		4		size
//==========================================
typedef struct{
	FILE_NAME 	FileName;
	u8 		FileAttrib;
	u8 		Rsvd;
	u8		FileCreateMs;
	u8 		FileCreateTime[2];
	u8 		FileCreateDate[2];
	u8		FileAccessDate[2];
	u8 		FilePositH[2];
	u8 		FileUpdateTime[2];
	u8 		FileUpdateDate[2];
	u8   	FilePositL[2];
	u8      FileSize[4];
} DIR;

//-----------------------------------------------------------//
//Function Name	: InitFAT
//description	: ��ʼ��FAT�ļ�ϵͳ
//Input			: None
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 InitFAT();

//-----------------------------------------------------------//
//Function Name	: FindFile
//description	: ��������Ѱ��ָ���ļ�
//Input			: u8 *pFile_Name : ҪѰ�ҵ��ļ���ָ��
//Output		: DIR *pDir : Ŀ¼����Ϣָ��
//				  u8 *pDir_Index : Ŀ¼������ָ��
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 FindFile(u8 *pFileName,DIR *pDir,u16 *pDir_Index);

//-----------------------------------------------------------//
//Function Name	: ReadFile
//description	: ��һ���ļ�
//Input			: char *pFileName : �ļ���
//				  u32 Start �����ļ��Ŀ�ʼ��ַ���ļ���ͷ���𣬵�λByte��
//				  u32 Length : Ҫ���ļ��ĳ��ȣ���λByte��
//Output		: u8 *pFile : �ļ�����ָ��
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 ReadFile(u8 *pFileName,u32 Start,u32 Length,u8 *pFile);

//-----------------------------------------------------------//
//Function Name	: CreatFile
//description	: ����һ���ļ�
//Input			: u8 *pFileName : �ļ���ָ��
//				  u32 Size ���ļ���С
//				  u8 *pFile ���ļ�����ָ��
//Output		: 0
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 CreatFile(u8 *pFileName,u32 Size,u8 *pFile);

//-----------------------------------------------------------//
//Function Name	: WriteFile
//description	: дһ���ļ�
//Input			: u8 *pFileName : �ļ���ָ��
//				  u32 Start : д�ļ��Ŀ�ʼ��ַ���ļ���ͷ���𣬵�λByte��
//				  u32 Length : Ҫд�����ݳ��ȣ���λByte��
//				  u8 *pFile : ָ��Ҫд���ݵ��ļ�ָ��
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 WriteFile(u8 *pFileName,u32 Start,u32 Length,u8 *pFile);

#endif /* FAT16_H_ */
