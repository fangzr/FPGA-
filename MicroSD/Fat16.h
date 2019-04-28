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
			//BPB(BIOS Parameter Block)信息//
typedef struct{
	u16 BytesPerSec;	//每个扇区多少字节
	u8	SecPerClus; 	//每个簇有多少个扇区
	u16	RsvdSecNum;		//保留扇区数
	u8	FATsNum;		//有多少个FAT表
	u16	RootEntNum;		//根目录允许的登记项数目
	u16	SecPerFAT;  	//每个FAT表有多少个扇区
	u16	HiddSec;		//隐藏的扇区数
	u32 SecNum; 		//总的扇区数
} FAT_BPB;

//--------------------------------------------------------------//
			 //重要区域的偏移地址//
typedef struct{
	u32 Logic;			//引导扇（逻辑扇区0）对物理0扇区里的偏移地址
	u32 FAT1;			//FAT1
	u32	FAT2;			//FAT2
	u32	FDT;			//根目录表FDT（File Directory Table）
	u32	Clus;			//数据簇
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
//description	: 初始化FAT文件系统
//Input			: None
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 InitFAT();

//-----------------------------------------------------------//
//Function Name	: FindFile
//description	: 根据名字寻找指定文件
//Input			: u8 *pFile_Name : 要寻找的文件名指针
//Output		: DIR *pDir : 目录项信息指针
//				  u8 *pDir_Index : 目录项索引指针
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 FindFile(u8 *pFileName,DIR *pDir,u16 *pDir_Index);

//-----------------------------------------------------------//
//Function Name	: ReadFile
//description	: 读一个文件
//Input			: char *pFileName : 文件名
//				  u32 Start ：读文件的开始地址（文件开头算起，单位Byte）
//				  u32 Length : 要读文件的长度（单位Byte）
//Output		: u8 *pFile : 文件内容指针
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 ReadFile(u8 *pFileName,u32 Start,u32 Length,u8 *pFile);

//-----------------------------------------------------------//
//Function Name	: CreatFile
//description	: 创建一个文件
//Input			: u8 *pFileName : 文件名指针
//				  u32 Size ：文件大小
//				  u8 *pFile ：文件内容指针
//Output		: 0
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 CreatFile(u8 *pFileName,u32 Size,u8 *pFile);

//-----------------------------------------------------------//
//Function Name	: WriteFile
//description	: 写一个文件
//Input			: u8 *pFileName : 文件名指针
//				  u32 Start : 写文件的开始地址（文件开头算起，单位Byte）
//				  u32 Length : 要写的数据长度（单位Byte）
//				  u8 *pFile : 指向要写内容的文件指针
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 WriteFile(u8 *pFileName,u32 Start,u32 Length,u8 *pFile);

#endif /* FAT16_H_ */
