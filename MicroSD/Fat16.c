/*
 * Fat16.c
 *
 *  Created on: 2012-2-24
 *      Author: Ming
 */

#include"Fat16.h"

FAT_BPB Bpb;
FAT_OFFSET Offset;
//--------------------------------------------内部函数----------------------------------------------------------------//
//-----------------------------------------------------------//
//Function Name	: ToUper
//description	: 将小写字母转为大写字母
//Input			: u8 *str：字符串指针
//Output		: None
//Return		: None
//-----------------------------------------------------------//
void ToUper(u8 *str)
{
	do{
		if(*str >= 'a' && *str <= 'z') *str = *str - 'a' + 'A';
	}while(*str++);
}
//-----------------------------------------------------------//
//Function Name	: MemSet
//description	: 对动态申请的堆区进行置零
//Input			: u8 *pMem : 指向申请堆区的指针
//				  u16 Size : 申请堆区的大小
//Output		: None
//Return		: None
//-----------------------------------------------------------//
void MemSet(u8 *pMem,u16 Size)
{
	for(;Size > 0;Size--) *pMem++ = 0;
}
//-----------------------------------------------------------//
//Function Name	: MemCopy
//description	: 按字节进行一块区域的拷贝
//Input			: u8 *pSourse : 要拷贝区域的指针
//				  u16 Size : 拷贝字节数
//Output		: at_u8 *pDest : 拷贝后存放区域的指针
//Return		: None
//-----------------------------------------------------------//
void MemCopy(u8 *pDest,u8 *pSourse,u16 Size)
{
	for(;Size > 0;Size--) *pDest++ = *pSourse++;

}
//-----------------------------------------------------------//
//Function Name	: InitDIR
//description	: 给DIR初始化赋初值
//Input			: DIR *pDir：DIR指针
//Output		: None
//Return		: None
//-----------------------------------------------------------//
void InitDIR(DIR *pDir)
{
	u8 i,*p = (u8*)pDir;
	for(i = 0;i < 32;i++){
		*p++ = 0x00;
	}
}
//-----------------------------------------------------------//
//Function Name	: FileNameConvert
//description	: 将字符串文件名转换成8.3格式文件名
//Input			: char *pFileName : 字符串文件名指针
//Output		: FILE_NAME *pFile_Name : 8.3格式文件名指针
//Return		: 0:成功
//				  1:非法文件名
//-----------------------------------------------------------//
u8 FileNameConvert(u8 *pFileName,FILE_NAME *pFile_Name)
{
	u8 count = 0,*p = pFileName;

	ToUper(p);							//文件名转为大写

	do{
		if(*p == (u8)'.'){
			if(count <= 8){	           	//文件名长度小于等于8
				MemCopy(pFile_Name->Name,pFileName,count);
				for(;count < 8;count++) pFile_Name->Name[count] = ' ';
				MemCopy(pFile_Name->Type,p + 1,3);
				return TRUE;
			}
			else{						//文件名大于8
				MemCopy(pFile_Name->Name,(u8*)pFileName,6);
				pFile_Name->Name[6] = '~';
				pFile_Name->Name[7] = '1';
				MemCopy(pFile_Name->Type,p + 1,3);
				return TRUE;
			}
		}
		count++;
	}while(*p++ != ' ');

	return FALSE;
}
//-----------------------------------------------------------//
//Function Name	: IsEqual
//description	: 判断两个数据串是否相等
//Input			: u8 *pDataA : 数据串A
//				  u8 *pDataB : 数据串B
//				  u8 Size : 数据串长度
//Output		: None
//Return		: 0:相等
//				  1:不相等
//-----------------------------------------------------------//
u8 IsEqual(u8 *pDataA,u8 *pDataB,u8 Size)
{
	for(;Size > 0;Size--){
		if(*pDataA++ != *pDataB++) return FALSE;
	}

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: ReadFAT
//description	: 读FAT表的指定项信息
//Input			: u16 Clus_Index : 簇索引
//Output		: None
//Return		: 2~65535:下一簇信息
//				  1:失败
//-----------------------------------------------------------//
u16 ReadFAT(u16 Clus_Index)
{
	u8 ok;
	u8 *pBuf = malloc(BLOCK_SIZE);
	u32 SecAddr = Offset.FAT1 + Clus_Index / (BLOCK_SIZE / 2);
	u16 ClusAddr = Clus_Index % (BLOCK_SIZE / 2);

	if(pBuf == NULL) return FALSE;	//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);		//堆区置零
	ok = ReadSec(SecAddr,pBuf);		//读对应FAT表扇区
	if(ok != TRUE){
		free(pBuf);					//释放申请的内存
		return FALSE;
	}

	return (pBuf[2 * ClusAddr] + (pBuf[2 * ClusAddr + 1] << 8));
}
//-----------------------------------------------------------//
//Function Name	: FindEmptyFAT
//description	: 寻找指定FAT项后一个空的FAT项
//Input			: None
//Output		: None
//Return		: 2~65535:空簇
//				  1:失败
//-----------------------------------------------------------//
u16 FindEmptyFAT(u16 Clus_Index)
{
	u16 i,j,Empty_Clus;
	u8 ok;
	u8 *pBuf = malloc(BLOCK_SIZE);
	u32 SecAddr = Offset.FAT1 + Clus_Index / (BLOCK_SIZE / 2);

	if(pBuf == NULL) return FALSE;		//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);			//堆区置零
	for(i = SecAddr;i < Offset.FAT1 + Bpb.SecPerFAT;i++){
		ok = ReadSec(i,pBuf);			//读对应FAT表扇区
		if(ok != TRUE){
			free(pBuf);					//释放申请的内存
			return FALSE;
		}
		for(j = 0;j < BLOCK_SIZE / 2;j++){
			Empty_Clus = (i - Offset.FAT1) * BLOCK_SIZE / 2 + j;
			if((pBuf[2 * j] + (pBuf[2 * j + 1] << 8)) == 0 && Empty_Clus > Clus_Index)	//如果簇未被占用且簇编号大于给定簇索引
				return  Empty_Clus;
		}
	}

	return FALSE;
}
//-----------------------------------------------------------//
//Function Name	: WriteFAT
//description	: 写FAT表的指定项信息
//Input			: u16 Clus_Index : 簇索引
//				  u16 Next_Clus ：要写的内容
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 WriteFAT(u16 Clus_Index,u16 Next_Clus)
{
	u8 ok,temp[2] = {Next_Clus,Next_Clus >> 8};
	u8 *pBuf = malloc(BLOCK_SIZE);
	u32 SecAddr = Offset.FAT1 + Clus_Index / (BLOCK_SIZE / 2);
	u16 ClusAddr = Clus_Index % (BLOCK_SIZE / 2);

	if(pBuf == NULL) return FALSE;			//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);				//堆区置零
	ok = ReadSec(SecAddr,pBuf);				//读对应FAT表扇区
	if(ok != TRUE){
		free(pBuf);							//释放申请的内存
		return FALSE;
	}
	MemCopy(&pBuf[2 * ClusAddr],temp,2);	//将要写的FAT项内容拷贝进缓冲区
	WriteSec(SecAddr,pBuf);					//写指定FAT项到FAT1和FAT2
	WriteSec(SecAddr + Bpb.SecPerFAT,pBuf);
	free(pBuf);								//释放申请的内存

	return TRUE;

}
//-----------------------------------------------------------//
//Function Name	: ReadMBR
//description	: 读取主引导扇区信息，得到逻辑扇区地址
//Input			: None
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 ReadMBR(void)
{
	u8 ok;
	u8 *pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;		//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);			//堆区置零
	ok = ReadSec(MBR_ADDR,pBuf);		//读MBR扇区
	if(ok != TRUE){						//如果读MBR失败或读取的签名不对，返回
		free(pBuf);						//释放申请的内存
		return FALSE;
	}
	if( pBuf[0x01FE] + (pBuf[0x01FF] << 8) != 0xAA55 ){
		free(pBuf);						//释放申请的内存
		return FALSE;
	}
	Offset.Logic = pBuf[0x01C6] + (pBuf[0x01C7] << 8)			//得到逻辑扇区相对物理扇区（0扇区）的地址
					+ (pBuf[0x01C8] << 16) + (pBuf[0x01C9] << 24);
	free(pBuf);							//释放申请的内存

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: ReadBPB
//description	: 读取逻辑扇区0（BPB）信息，获得FAT1,FAT2,FDT,簇等参数
//Input			: None
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 ReadBPB(void)
{
	xil_printf("goto here2!");
	u8 ok;
	u8 *pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;			//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);				//堆区置零
	ok = ReadSec(Offset.Logic,pBuf);		//读逻辑扇区0（BPB）
	if(ok != TRUE){
		free(pBuf);							//释放申请的内存
		return FALSE;
	}

	Bpb.BytesPerSec = pBuf[0x0B] + (pBuf[0x0C] << 8);	//得到每扇区字节数
	Bpb.SecPerClus = pBuf[0x0D];						//得到每簇扇区数
	Bpb.RsvdSecNum = pBuf[0x0E] + (pBuf[0x0F] << 8);	//得到保留扇区数
	Bpb.FATsNum = pBuf[0x10];							//得到FAT表个数
	Bpb.RootEntNum = pBuf[0x11] + (pBuf[0x12] << 8);	//得到根目录项数
	Bpb.SecPerFAT = pBuf[0x16] + (pBuf[0x17] << 8);		//得到每个FAT表扇区数
	Bpb.HiddSec = pBuf[0x1C] + (pBuf[0x1D] << 8)		//得到隐藏扇区数
					+ (pBuf[0x1E] << 16) + (pBuf[0x1F] << 24);
														//得到磁盘总扇区数
	Bpb.SecNum = pBuf[0x13] + (pBuf[0x14] << 8) 		//FATSz16
					+ pBuf[0x20] + (pBuf[0x21] << 8) 	//或FATSz32
					+ (pBuf[0x22] << 16) + (pBuf[0x23] << 24);
	free(pBuf);								//释放申请的内存

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: GetOffset
//description	: 获得磁盘重要区域（FAT1~2，FDT，数据簇0）相对物理0扇区的偏移地址
//Input			: None
//Output		: None
//Return		: None
//-----------------------------------------------------------//
void GetOffset(void)
{

	Offset.FAT1 = Offset.Logic + Bpb.RsvdSecNum;			//获得FAT1的地址
	if(Bpb.FATsNum == 2)									//获得FAT2的地址，如果没有FAT2，则把FAT2的地址当做与FAT1一样处理
		Offset.FAT2 = Offset.FAT1 + Bpb.SecPerFAT;
	else
		Offset.FAT2 = Offset.FAT1;
	Offset.FDT = Offset.FAT1 + Bpb.SecPerFAT * Bpb.FATsNum;	//获得根目录目录地址
	Offset.Clus = Offset.FDT + 32 - Bpb.SecPerClus * 2;		//获得数据簇0的地址
}
//-----------------------------------------------------------//
//Function Name	: ReadDIR
//description	: 读根目录指定项
//Input			: u16 Dir_Index : 目录项索引
//Output		: DIR *pDir : 目录项信息
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 ReadDIR(u16 Dir_Index,DIR *pDir)
{
	u32 SecAddr = Offset.FDT + Dir_Index / 16;
	u16 DirAddr = (Dir_Index % 16) * 32;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;				//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);					//堆区置零
	ok = ReadSec(SecAddr,pBuf);					//读对应扇区根目录表
	if(ok != TRUE){
		free(pBuf);								//释放申请的内存
		return FALSE;
	}
	MemCopy((u8*)pDir,pBuf + DirAddr,32);
	free(pBuf);									//释放申请的内存

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: FindEmptyDIR
//description	: 寻找空的目录项
//Input			: None
//Output		: None
//Return		: 2~65535:空目录项号
//				  1:失败
//-----------------------------------------------------------//
u16 FindEmptyDIR()
{
	u8 i,j,ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;			//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);				//堆区置零
	for(i = 0;i < Bpb.RootEntNum / 16;i++){
		ok = ReadSec(i + Offset.FDT,pBuf);	//读对应扇区根目录表
		if(ok != TRUE){
			free(pBuf);						//释放申请的内存
			return FALSE;
		}
		for(j = 0;j < 16;j++){
			if(pBuf[32 * j] == 0x00){
				free(pBuf);					//释放申请的内存
				return i * 16 + j;
			}
		}
	}
	free(pBuf);								//释放申请的内存

	return FALSE;
}
//-----------------------------------------------------------//
//Function Name	: WriteDIR
//description	: 写指定目录项
//Input			: 目录项索引
//				      要写的目录信息指针
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 WriteDIR(u16 Dir_Index,DIR *pDir)
{
	u32 SecAddr = Offset.FDT + Dir_Index / 16;
	u16 DirAddr = (Dir_Index % 16) * 32;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;			//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);				//堆区置零
	ok = ReadSec(SecAddr,pBuf);				//读对应扇区根目录表
	if(ok != TRUE){
		free(pBuf);							//释放申请的内存
		return FALSE;
	}
	MemCopy(pBuf + DirAddr,(u8*)pDir,32);//将要写的DIR数据复制到缓冲区
	WriteSec(SecAddr,pBuf);					//写对应的根目录表
	free(pBuf);								//释放申请的内存

	return TRUE;
}
//----------------------------------------------外部函数--------------------------------------------------------//
//-----------------------------------------------------------//
//Function Name	: InitFAT
//description	: 初始化FAT文件系统
//Input			: None
//Output		: None
//Return		: 0:成功
//				  1:失败
//-----------------------------------------------------------//
u8 InitFAT()
{
	if(InitSD() == FALSE) return FALSE;		//初始化SD卡

	if(ReadMBR()== FALSE) return FALSE;		//读主引导扇区（物理扇区0）
	xil_printf("goto here1!");
	ReadBPB();								//读逻辑扇区0
	GetOffset();							//计算磁盘重要区域的偏移地址（相对于物理扇区0）

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: FindFile
//description	: 根据名字寻找指定文件
//Input			: u8 *pFile_Name : 要寻找的文件名指针
//Output		: DIR *pDir : 目录项信息指针
//				  u8 *pDir_Index : 目录项索引指针
//Return		: 0:成功
//				  1:失败
//				  2:文件名不合法
//-----------------------------------------------------------//
u8 FindFile(u8 *pFileName,DIR *pDir,u16 *pDir_Index)
{
	u8 i,j,DIRBlockNum = Bpb.RootEntNum * 32 / BLOCK_SIZE;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);
	FILE_NAME pFile_Name;

	if(pBuf == NULL) return FALSE;				//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);					//堆区置零

	if(FileNameConvert(pFileName,&pFile_Name) == FALSE){	//文件名转为8.3格式
		free(pBuf);								//释放申请的内存
		return NAME_ERROR;
	}
	MemCopy((u8*)&pDir->FileName,(u8*)&pFile_Name,11);
	*pDir_Index = 0;

	for(i = 0;i < DIRBlockNum;i++){
		ok = ReadSec(Offset.FDT + i,pBuf);		//读对应扇区根目录表
		if(ok != TRUE){
			free(pBuf);							//释放申请的内存
			return FALSE;
		}
		for(j = 0;j < BLOCK_SIZE / 32;j++){		//判断每目录项的名字是否相等
			if(pBuf[j * 32] == 0x00 && pBuf[(j + 1) * 32 == 0x00]) return FALSE;
			if(TRUE == IsEqual(pBuf + j * 32,(u8*)&pFile_Name,11)){
				MemCopy((u8*)pDir,pBuf + j * 32,32);
				free(pBuf);						//释放申请的内存
				return TRUE;
			}
			(*pDir_Index)++;
		}
	}
	free(pBuf);									//释放申请的内存

	return FALSE;
}
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
u8 ReadFile(u8 *pFileName,u32 Start,u32 Length,u8 *pFile)
{
	DIR Dir;
	u16 i,Dir_Index,Clus_Index,Next_Clus;
	u32 File_Size,Sec_Addr;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;			//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);				//堆区置零
	if(FindFile(pFileName,&Dir,&Dir_Index) != TRUE){
		free(pBuf);							//释放申请的内存
		return FALSE;
	}

	Clus_Index = (Dir.FilePositL[1] << 8)+ Dir.FilePositL[0];		//获得文件开始簇号
	File_Size = (Dir.FileSize[3] << 24) + (Dir.FileSize[2] << 16) 	//获得文件大小
				+ (Dir.FileSize[1] << 8) + Dir.FileSize[0];
	if(Start > File_Size){											//如果读开始地址大于文件大小就返回
		free(pBuf);													//释放申请的内存
		return FALSE;
	}
	if(Start + Length > File_Size) Length = File_Size - Start;		//如果读逾界就读到文件结尾
	for(i = 0;i < Start / (Bpb.BytesPerSec * Bpb.SecPerClus);i++){	//获得要读的起始簇号
		Clus_Index = ReadFAT(Clus_Index);
	}

	//--读第一个扇区--//
										//获得要读的扇区的地址
	Sec_Addr = Offset.Clus + Clus_Index * Bpb.SecPerClus + Start % (Bpb.SecPerClus * Bpb.BytesPerSec) / BLOCK_SIZE;
	ok = ReadSec(Sec_Addr,pBuf);
	if(ok != TRUE){
		free(pBuf);						//释放申请的内存
		return FALSE;
	}
	for(i = Start % (Bpb.SecPerClus * Bpb.BytesPerSec) % BLOCK_SIZE;i < BLOCK_SIZE;i++){//将第一个扇区要读的数据对应读出来
		*pFile++ = pBuf[i];
		Length--;
		if(Length == 0){

			free(pBuf);					//释放申请的内存
			return TRUE;
		}
	}
	//--读余下的扇区直到Length个数据都读出--//
	while(1){
										//如果写完一个簇就寻找下一个簇地址，反之写扇区地址Sec_Addr加1
		if((double)((Sec_Addr - Offset.Clus + 1) / Bpb.SecPerClus) == ((double)(Sec_Addr - Offset.Clus + 1)) / Bpb.SecPerClus){
			Next_Clus = ReadFAT(Clus_Index);
			if(Next_Clus == FALSE){
				free(pBuf);				//释放申请的内存
				return FALSE;
			}
			Clus_Index = Next_Clus;
										//获得要读的下一扇区的地址
			Sec_Addr = Offset.Clus + Clus_Index * Bpb.SecPerClus + Start % (Bpb.SecPerClus * Bpb.BytesPerSec) / BLOCK_SIZE;
		}
		else Sec_Addr++;
		if(Length < BLOCK_SIZE){		//读最后一个扇区数据
			ok = ReadSec(Sec_Addr,pBuf);
			if(ok != TRUE){
				free(pBuf);				//释放申请的内存
				return FALSE;
			}
			for(i = 0;i < Length;i++)	//将最后一个扇区要读的数据读出
				*pFile++ = pBuf[i];
			free(pBuf);					//释放申请的内存
			return TRUE;
		}
		ok = ReadSec(Sec_Addr,pFile);	//读除去第一个和最后一个的扇区
		if(ok != TRUE){
			free(pBuf);					//释放申请的内存
			return FALSE;
		}
		pFile += BLOCK_SIZE;
		Length -= BLOCK_SIZE;
	}

}
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
u8 CreatFile(u8 *pFileName,u32 Size,u8 *pFile)
{
	DIR Dir;
	InitDIR(&Dir);
	u8 i,j;
	u16 Dir_Index,Clus_Index,Next_Clus;
	u16 Clus_Num = Size / (Bpb.BytesPerSec * Bpb.SecPerClus) + 1;	//文件总共要用到的簇数目
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL){							//申请动态内存失败，返回
		free(pBuf);								//释放申请的内存
		return FALSE;
	}
	MemSet(pBuf,BLOCK_SIZE);					//堆区置零

	//--查找文件是否存在，查找空目录项和FAT项--//
	ok = FindFile(pFileName,&Dir,&Dir_Index);	//文件名已经存在或非法文件名
	if(ok == TRUE || ok == NAME_ERROR){
		free(pBuf);								//释放申请的内存
		return FALSE;
	}
	Dir_Index = FindEmptyDIR();					//寻找空的文件目录项
	if(Dir_Index == FALSE){
		free(pBuf);								//释放申请的内存
		return FALSE;
	}
	Clus_Index = FindEmptyFAT(0);				//寻找空的文件分配表项
	if(Clus_Index == FALSE){
		free(pBuf);								//释放申请的内存
		return FALSE;
	}

	//--将文件的位置和大小的高低位交换再写入--//
	Dir.FilePositL[1] = Clus_Index >> 8;
	Dir.FilePositL[0] = Clus_Index;
	Dir.FileSize[3] = Size >> 24;
	Dir.FileSize[2] = Size >> 16;
	Dir.FileSize[1] = Size >> 8;
	Dir.FileSize[0] = Size;
	Dir.FileAttrib = 0x20;
	ok = WriteDIR(Dir_Index,&Dir);
	if(ok != TRUE){
		//释放申请的内存
		free(pBuf);
		return FALSE;
	}

	//--写文件分配表和文件内容--//
	for(j = 0;j < Clus_Num;j++){
		if(j == Clus_Num - 1){							//写最后一个簇
			if(TRUE != WriteFAT(Clus_Index,0xFFFF)){	//写文件分配表
				free(pBuf);								//释放申请的内存
				return FALSE;
			}
			for(i = 0;i < Bpb.SecPerClus;i++){			//写文件内容
				MemCopy(pBuf,pFile + (j * Bpb.SecPerClus + i) * Bpb.BytesPerSec,Size > BLOCK_SIZE ? BLOCK_SIZE : Size);
				ok = WriteSec(Offset.Clus + Clus_Index * Bpb.SecPerClus + i,pBuf);
				if(ok != TRUE){
					free(pBuf);							//释放申请的内存
					return FALSE;
				}
				if(Size <= BLOCK_SIZE){					//最后一个扇区写完就返回
					free(pBuf);							//释放申请的内存
					return TRUE;
				}
				Size -= BLOCK_SIZE;
			}
		}
		else{
			Next_Clus = FindEmptyFAT(Clus_Index);		//寻找下一个空簇
			if(TRUE != WriteFAT(Clus_Index,Next_Clus)){	//写文件分配表
				free(pBuf);								//释放申请的内存
				return FALSE;
			}
			for(i = 0;i < Bpb.SecPerClus;i++){			//写文件内容
				ok = WriteSec(Offset.Clus + Clus_Index * Bpb.SecPerClus + i,pFile + Bpb.BytesPerSec * (j * Bpb.SecPerClus + i));
				if(ok != TRUE){
					free(pBuf);							//释放申请的内存
					return FALSE;
				}
				Size -= BLOCK_SIZE;
			}
			Clus_Index = Next_Clus;
		}
	}
	free(pBuf);											//释放申请的内存

	return TRUE;
}
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
u8 WriteFile(u8 *pFileName,u32 Start,u32 Length,u8 *pFile)
{
	DIR Dir;
	u16 i,Dir_Index,Clus_Index,Next_Clus;
	u32 File_Size,Sec_Addr,Length_Temp = Length;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;						//申请动态内存失败，返回
	MemSet(pBuf,BLOCK_SIZE);							//堆区置零
	if(FindFile(pFileName,&Dir,&Dir_Index) != TRUE){
		free(pBuf);										//释放申请的内存
		return FALSE;
	}

	Clus_Index = (Dir.FilePositL[1] << 8)+ Dir.FilePositL[0];		//获得文件起始簇号
	File_Size = (Dir.FileSize[3] << 24) + (Dir.FileSize[2] << 16)	//获得文件大小
				+ (Dir.FileSize[1] << 8) + Dir.FileSize[0];
	if(Start > File_Size){											//如果写开始地址大于文件大小就返回
		free(pBuf);													//释放申请的内存
		return FALSE;
	}
	for(i = 0;i < Start / (Bpb.BytesPerSec * Bpb.SecPerClus);i++){	//获得要写的起始簇号
		Clus_Index = ReadFAT(Clus_Index);
	}
	//--写第一个扇区--//
													//获得要写的扇区的地址
	Sec_Addr = Offset.Clus + Clus_Index * Bpb.SecPerClus + Start % (Bpb.SecPerClus * Bpb.BytesPerSec) / BLOCK_SIZE;
	ok = ReadSec(Sec_Addr,pBuf);
	if(ok != TRUE){
		free(pBuf);									//释放申请的内存
		return FALSE;
	}
													//将第一个扇区要写的数据对应写入缓存
	for(i = Start % (Bpb.SecPerClus * Bpb.BytesPerSec) % BLOCK_SIZE;i < BLOCK_SIZE;i++){
		pBuf[i] = *pFile++;
		Length--;
		if(Length == 0) break;
	}
	ok = WriteSec(Sec_Addr,pBuf);					//将第一个扇区数据写入
	if(ok != TRUE || Length == 0){
		free(pBuf);									//释放申请的内存
		if(File_Size < Start + Length_Temp -Length){//如果文件变大，修改根目录项的文件大小
			File_Size = Start + Length_Temp -Length;
			Dir.FileSize[0] = File_Size;
			Dir.FileSize[1] = File_Size >> 8;
			Dir.FileSize[2] = File_Size >> 16;
			Dir.FileSize[3] = File_Size >> 24;
			WriteDIR(Dir_Index,&Dir);
		}
		if(ok != TRUE) return FALSE;
		else return TRUE;
	}
	//--写余下的扇区直到Length个数据都写入--//
	while(1){													//如果写完一个簇就寻找下一个簇地址，反之写扇区地址Sec_Addr加1
		if((double)((Sec_Addr - Offset.Clus + 1) / Bpb.SecPerClus) == ((double)(Sec_Addr - Offset.Clus + 1)) / Bpb.SecPerClus){
			Next_Clus = ReadFAT(Clus_Index);
			if(Next_Clus == FALSE){
				free(pBuf);										//释放申请的内存
				if(File_Size < Start + Length_Temp -Length){	//如果文件变大，修改根目录项的文件大小
					File_Size = Start + Length_Temp -Length;
					Dir.FileSize[0] = File_Size;
					Dir.FileSize[1] = File_Size >> 8;
					Dir.FileSize[2] = File_Size >> 16;
					Dir.FileSize[3] = File_Size >> 24;
					WriteDIR(Dir_Index,&Dir);
				}
				return FALSE;
			}
			if(Next_Clus == 0xFFFF){							//如果写完原文件最后一个簇，则分配一个新簇
				Next_Clus = FindEmptyFAT(Clus_Index);
				ok = WriteFAT(Clus_Index,Next_Clus);
				if(ok != TRUE){
					free(pBuf);									//释放申请的内存
					if(File_Size < Start + Length_Temp -Length){//如果文件变大，修改根目录项的文件大小
						File_Size = Start + Length_Temp -Length;
						Dir.FileSize[0] = File_Size;
						Dir.FileSize[1] = File_Size >> 8;
						Dir.FileSize[2] = File_Size >> 16;
						Dir.FileSize[3] = File_Size >> 24;
						WriteDIR(Dir_Index,&Dir);
					}
					return FALSE;
				}
				ok = WriteFAT(Next_Clus,0xFFFF);
				if(ok != TRUE){
					free(pBuf);									//释放申请的内存
					if(File_Size < Start + Length_Temp -Length){//如果文件变大，修改根目录项的文件大小
						File_Size = Start + Length_Temp -Length;
						Dir.FileSize[0] = File_Size;
						Dir.FileSize[1] = File_Size >> 8;
						Dir.FileSize[2] = File_Size >> 16;
						Dir.FileSize[3] = File_Size >> 24;
						WriteDIR(Dir_Index,&Dir);
					}
					return FALSE;
				}
			}
			Clus_Index = Next_Clus;
			Sec_Addr = Offset.Clus + Clus_Index * Bpb.SecPerClus;//获得要写的扇区的地址
		}
		else Sec_Addr++;

		if(Length < BLOCK_SIZE){								//写最后一个扇区数据
			ok = ReadSec(Sec_Addr,pBuf);
			if(ok != TRUE){
				free(pBuf);										//释放申请的内存
				if(File_Size < Start + Length_Temp -Length){	//如果文件变大，修改根目录项的文件大小
					File_Size = Start + Length_Temp -Length;
					Dir.FileSize[0] = File_Size;
					Dir.FileSize[1] = File_Size >> 8;
					Dir.FileSize[2] = File_Size >> 16;
					Dir.FileSize[3] = File_Size >> 24;
					WriteDIR(Dir_Index,&Dir);
				}
				return FALSE;
			}
			for(i = 0;i < Length;i++)							//将最后一个扇区要写的数据对应写入缓存
				pBuf[i] = *pFile++;
			ok = WriteSec(Sec_Addr,pBuf);						//将最后一个扇区数据写入
			free(pBuf);											//释放申请的内存
			if(ok == TRUE) Length = 0;
			if(File_Size < Start + Length_Temp -Length){		//如果文件变大，修改根目录项的文件大小
				File_Size = Start + Length_Temp -Length;
				Dir.FileSize[0] = File_Size;
				Dir.FileSize[1] = File_Size >> 8;
				Dir.FileSize[2] = File_Size >> 16;
				Dir.FileSize[3] = File_Size >> 24;
				WriteDIR(Dir_Index,&Dir);
			}
			if(ok == TRUE)
				return TRUE;
			else
				return FALSE;
		}

		ok = WriteSec(Sec_Addr,pFile);							//写除去第一个和最后一个的扇区
		if(ok != TRUE){
			free(pBuf);											//释放申请的内存
			if(File_Size < Start + Length_Temp -Length){		//如果文件变大，修改根目录项的文件大小
				File_Size = Start + Length_Temp -Length;
				Dir.FileSize[0] = File_Size;
				Dir.FileSize[1] = File_Size >> 8;
				Dir.FileSize[2] = File_Size >> 16;
				Dir.FileSize[3] = File_Size >> 24;
				WriteDIR(Dir_Index,&Dir);
			}
			return FALSE;
		}
		pFile += BLOCK_SIZE;
		Length -= BLOCK_SIZE;
	}
}
