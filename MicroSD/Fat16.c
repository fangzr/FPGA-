/*
 * Fat16.c
 *
 *  Created on: 2012-2-24
 *      Author: Ming
 */

#include"Fat16.h"

FAT_BPB Bpb;
FAT_OFFSET Offset;
//--------------------------------------------�ڲ�����----------------------------------------------------------------//
//-----------------------------------------------------------//
//Function Name	: ToUper
//description	: ��Сд��ĸתΪ��д��ĸ
//Input			: u8 *str���ַ���ָ��
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
//description	: �Զ�̬����Ķ�����������
//Input			: u8 *pMem : ָ�����������ָ��
//				  u16 Size : ��������Ĵ�С
//Output		: None
//Return		: None
//-----------------------------------------------------------//
void MemSet(u8 *pMem,u16 Size)
{
	for(;Size > 0;Size--) *pMem++ = 0;
}
//-----------------------------------------------------------//
//Function Name	: MemCopy
//description	: ���ֽڽ���һ������Ŀ���
//Input			: u8 *pSourse : Ҫ���������ָ��
//				  u16 Size : �����ֽ���
//Output		: at_u8 *pDest : �������������ָ��
//Return		: None
//-----------------------------------------------------------//
void MemCopy(u8 *pDest,u8 *pSourse,u16 Size)
{
	for(;Size > 0;Size--) *pDest++ = *pSourse++;

}
//-----------------------------------------------------------//
//Function Name	: InitDIR
//description	: ��DIR��ʼ������ֵ
//Input			: DIR *pDir��DIRָ��
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
//description	: ���ַ����ļ���ת����8.3��ʽ�ļ���
//Input			: char *pFileName : �ַ����ļ���ָ��
//Output		: FILE_NAME *pFile_Name : 8.3��ʽ�ļ���ָ��
//Return		: 0:�ɹ�
//				  1:�Ƿ��ļ���
//-----------------------------------------------------------//
u8 FileNameConvert(u8 *pFileName,FILE_NAME *pFile_Name)
{
	u8 count = 0,*p = pFileName;

	ToUper(p);							//�ļ���תΪ��д

	do{
		if(*p == (u8)'.'){
			if(count <= 8){	           	//�ļ�������С�ڵ���8
				MemCopy(pFile_Name->Name,pFileName,count);
				for(;count < 8;count++) pFile_Name->Name[count] = ' ';
				MemCopy(pFile_Name->Type,p + 1,3);
				return TRUE;
			}
			else{						//�ļ�������8
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
//description	: �ж��������ݴ��Ƿ����
//Input			: u8 *pDataA : ���ݴ�A
//				  u8 *pDataB : ���ݴ�B
//				  u8 Size : ���ݴ�����
//Output		: None
//Return		: 0:���
//				  1:�����
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
//description	: ��FAT���ָ������Ϣ
//Input			: u16 Clus_Index : ������
//Output		: None
//Return		: 2~65535:��һ����Ϣ
//				  1:ʧ��
//-----------------------------------------------------------//
u16 ReadFAT(u16 Clus_Index)
{
	u8 ok;
	u8 *pBuf = malloc(BLOCK_SIZE);
	u32 SecAddr = Offset.FAT1 + Clus_Index / (BLOCK_SIZE / 2);
	u16 ClusAddr = Clus_Index % (BLOCK_SIZE / 2);

	if(pBuf == NULL) return FALSE;	//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);		//��������
	ok = ReadSec(SecAddr,pBuf);		//����ӦFAT������
	if(ok != TRUE){
		free(pBuf);					//�ͷ�������ڴ�
		return FALSE;
	}

	return (pBuf[2 * ClusAddr] + (pBuf[2 * ClusAddr + 1] << 8));
}
//-----------------------------------------------------------//
//Function Name	: FindEmptyFAT
//description	: Ѱ��ָ��FAT���һ���յ�FAT��
//Input			: None
//Output		: None
//Return		: 2~65535:�մ�
//				  1:ʧ��
//-----------------------------------------------------------//
u16 FindEmptyFAT(u16 Clus_Index)
{
	u16 i,j,Empty_Clus;
	u8 ok;
	u8 *pBuf = malloc(BLOCK_SIZE);
	u32 SecAddr = Offset.FAT1 + Clus_Index / (BLOCK_SIZE / 2);

	if(pBuf == NULL) return FALSE;		//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);			//��������
	for(i = SecAddr;i < Offset.FAT1 + Bpb.SecPerFAT;i++){
		ok = ReadSec(i,pBuf);			//����ӦFAT������
		if(ok != TRUE){
			free(pBuf);					//�ͷ�������ڴ�
			return FALSE;
		}
		for(j = 0;j < BLOCK_SIZE / 2;j++){
			Empty_Clus = (i - Offset.FAT1) * BLOCK_SIZE / 2 + j;
			if((pBuf[2 * j] + (pBuf[2 * j + 1] << 8)) == 0 && Empty_Clus > Clus_Index)	//�����δ��ռ���Ҵر�Ŵ��ڸ���������
				return  Empty_Clus;
		}
	}

	return FALSE;
}
//-----------------------------------------------------------//
//Function Name	: WriteFAT
//description	: дFAT���ָ������Ϣ
//Input			: u16 Clus_Index : ������
//				  u16 Next_Clus ��Ҫд������
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 WriteFAT(u16 Clus_Index,u16 Next_Clus)
{
	u8 ok,temp[2] = {Next_Clus,Next_Clus >> 8};
	u8 *pBuf = malloc(BLOCK_SIZE);
	u32 SecAddr = Offset.FAT1 + Clus_Index / (BLOCK_SIZE / 2);
	u16 ClusAddr = Clus_Index % (BLOCK_SIZE / 2);

	if(pBuf == NULL) return FALSE;			//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);				//��������
	ok = ReadSec(SecAddr,pBuf);				//����ӦFAT������
	if(ok != TRUE){
		free(pBuf);							//�ͷ�������ڴ�
		return FALSE;
	}
	MemCopy(&pBuf[2 * ClusAddr],temp,2);	//��Ҫд��FAT�����ݿ�����������
	WriteSec(SecAddr,pBuf);					//дָ��FAT�FAT1��FAT2
	WriteSec(SecAddr + Bpb.SecPerFAT,pBuf);
	free(pBuf);								//�ͷ�������ڴ�

	return TRUE;

}
//-----------------------------------------------------------//
//Function Name	: ReadMBR
//description	: ��ȡ������������Ϣ���õ��߼�������ַ
//Input			: None
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 ReadMBR(void)
{
	u8 ok;
	u8 *pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;		//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);			//��������
	ok = ReadSec(MBR_ADDR,pBuf);		//��MBR����
	if(ok != TRUE){						//�����MBRʧ�ܻ��ȡ��ǩ�����ԣ�����
		free(pBuf);						//�ͷ�������ڴ�
		return FALSE;
	}
	if( pBuf[0x01FE] + (pBuf[0x01FF] << 8) != 0xAA55 ){
		free(pBuf);						//�ͷ�������ڴ�
		return FALSE;
	}
	Offset.Logic = pBuf[0x01C6] + (pBuf[0x01C7] << 8)			//�õ��߼������������������0�������ĵ�ַ
					+ (pBuf[0x01C8] << 16) + (pBuf[0x01C9] << 24);
	free(pBuf);							//�ͷ�������ڴ�

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: ReadBPB
//description	: ��ȡ�߼�����0��BPB����Ϣ�����FAT1,FAT2,FDT,�صȲ���
//Input			: None
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 ReadBPB(void)
{
	xil_printf("goto here2!");
	u8 ok;
	u8 *pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;			//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);				//��������
	ok = ReadSec(Offset.Logic,pBuf);		//���߼�����0��BPB��
	if(ok != TRUE){
		free(pBuf);							//�ͷ�������ڴ�
		return FALSE;
	}

	Bpb.BytesPerSec = pBuf[0x0B] + (pBuf[0x0C] << 8);	//�õ�ÿ�����ֽ���
	Bpb.SecPerClus = pBuf[0x0D];						//�õ�ÿ��������
	Bpb.RsvdSecNum = pBuf[0x0E] + (pBuf[0x0F] << 8);	//�õ�����������
	Bpb.FATsNum = pBuf[0x10];							//�õ�FAT�����
	Bpb.RootEntNum = pBuf[0x11] + (pBuf[0x12] << 8);	//�õ���Ŀ¼����
	Bpb.SecPerFAT = pBuf[0x16] + (pBuf[0x17] << 8);		//�õ�ÿ��FAT��������
	Bpb.HiddSec = pBuf[0x1C] + (pBuf[0x1D] << 8)		//�õ�����������
					+ (pBuf[0x1E] << 16) + (pBuf[0x1F] << 24);
														//�õ�������������
	Bpb.SecNum = pBuf[0x13] + (pBuf[0x14] << 8) 		//FATSz16
					+ pBuf[0x20] + (pBuf[0x21] << 8) 	//��FATSz32
					+ (pBuf[0x22] << 16) + (pBuf[0x23] << 24);
	free(pBuf);								//�ͷ�������ڴ�

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: GetOffset
//description	: ��ô�����Ҫ����FAT1~2��FDT�����ݴ�0���������0������ƫ�Ƶ�ַ
//Input			: None
//Output		: None
//Return		: None
//-----------------------------------------------------------//
void GetOffset(void)
{

	Offset.FAT1 = Offset.Logic + Bpb.RsvdSecNum;			//���FAT1�ĵ�ַ
	if(Bpb.FATsNum == 2)									//���FAT2�ĵ�ַ�����û��FAT2�����FAT2�ĵ�ַ������FAT1һ������
		Offset.FAT2 = Offset.FAT1 + Bpb.SecPerFAT;
	else
		Offset.FAT2 = Offset.FAT1;
	Offset.FDT = Offset.FAT1 + Bpb.SecPerFAT * Bpb.FATsNum;	//��ø�Ŀ¼Ŀ¼��ַ
	Offset.Clus = Offset.FDT + 32 - Bpb.SecPerClus * 2;		//������ݴ�0�ĵ�ַ
}
//-----------------------------------------------------------//
//Function Name	: ReadDIR
//description	: ����Ŀ¼ָ����
//Input			: u16 Dir_Index : Ŀ¼������
//Output		: DIR *pDir : Ŀ¼����Ϣ
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 ReadDIR(u16 Dir_Index,DIR *pDir)
{
	u32 SecAddr = Offset.FDT + Dir_Index / 16;
	u16 DirAddr = (Dir_Index % 16) * 32;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;				//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);					//��������
	ok = ReadSec(SecAddr,pBuf);					//����Ӧ������Ŀ¼��
	if(ok != TRUE){
		free(pBuf);								//�ͷ�������ڴ�
		return FALSE;
	}
	MemCopy((u8*)pDir,pBuf + DirAddr,32);
	free(pBuf);									//�ͷ�������ڴ�

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: FindEmptyDIR
//description	: Ѱ�ҿյ�Ŀ¼��
//Input			: None
//Output		: None
//Return		: 2~65535:��Ŀ¼���
//				  1:ʧ��
//-----------------------------------------------------------//
u16 FindEmptyDIR()
{
	u8 i,j,ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;			//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);				//��������
	for(i = 0;i < Bpb.RootEntNum / 16;i++){
		ok = ReadSec(i + Offset.FDT,pBuf);	//����Ӧ������Ŀ¼��
		if(ok != TRUE){
			free(pBuf);						//�ͷ�������ڴ�
			return FALSE;
		}
		for(j = 0;j < 16;j++){
			if(pBuf[32 * j] == 0x00){
				free(pBuf);					//�ͷ�������ڴ�
				return i * 16 + j;
			}
		}
	}
	free(pBuf);								//�ͷ�������ڴ�

	return FALSE;
}
//-----------------------------------------------------------//
//Function Name	: WriteDIR
//description	: дָ��Ŀ¼��
//Input			: Ŀ¼������
//				      Ҫд��Ŀ¼��Ϣָ��
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 WriteDIR(u16 Dir_Index,DIR *pDir)
{
	u32 SecAddr = Offset.FDT + Dir_Index / 16;
	u16 DirAddr = (Dir_Index % 16) * 32;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;			//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);				//��������
	ok = ReadSec(SecAddr,pBuf);				//����Ӧ������Ŀ¼��
	if(ok != TRUE){
		free(pBuf);							//�ͷ�������ڴ�
		return FALSE;
	}
	MemCopy(pBuf + DirAddr,(u8*)pDir,32);//��Ҫд��DIR���ݸ��Ƶ�������
	WriteSec(SecAddr,pBuf);					//д��Ӧ�ĸ�Ŀ¼��
	free(pBuf);								//�ͷ�������ڴ�

	return TRUE;
}
//----------------------------------------------�ⲿ����--------------------------------------------------------//
//-----------------------------------------------------------//
//Function Name	: InitFAT
//description	: ��ʼ��FAT�ļ�ϵͳ
//Input			: None
//Output		: None
//Return		: 0:�ɹ�
//				  1:ʧ��
//-----------------------------------------------------------//
u8 InitFAT()
{
	if(InitSD() == FALSE) return FALSE;		//��ʼ��SD��

	if(ReadMBR()== FALSE) return FALSE;		//����������������������0��
	xil_printf("goto here1!");
	ReadBPB();								//���߼�����0
	GetOffset();							//���������Ҫ�����ƫ�Ƶ�ַ���������������0��

	return TRUE;
}
//-----------------------------------------------------------//
//Function Name	: FindFile
//description	: ��������Ѱ��ָ���ļ�
//Input			: u8 *pFile_Name : ҪѰ�ҵ��ļ���ָ��
//Output		: DIR *pDir : Ŀ¼����Ϣָ��
//				  u8 *pDir_Index : Ŀ¼������ָ��
//Return		: 0:�ɹ�
//				  1:ʧ��
//				  2:�ļ������Ϸ�
//-----------------------------------------------------------//
u8 FindFile(u8 *pFileName,DIR *pDir,u16 *pDir_Index)
{
	u8 i,j,DIRBlockNum = Bpb.RootEntNum * 32 / BLOCK_SIZE;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);
	FILE_NAME pFile_Name;

	if(pBuf == NULL) return FALSE;				//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);					//��������

	if(FileNameConvert(pFileName,&pFile_Name) == FALSE){	//�ļ���תΪ8.3��ʽ
		free(pBuf);								//�ͷ�������ڴ�
		return NAME_ERROR;
	}
	MemCopy((u8*)&pDir->FileName,(u8*)&pFile_Name,11);
	*pDir_Index = 0;

	for(i = 0;i < DIRBlockNum;i++){
		ok = ReadSec(Offset.FDT + i,pBuf);		//����Ӧ������Ŀ¼��
		if(ok != TRUE){
			free(pBuf);							//�ͷ�������ڴ�
			return FALSE;
		}
		for(j = 0;j < BLOCK_SIZE / 32;j++){		//�ж�ÿĿ¼��������Ƿ����
			if(pBuf[j * 32] == 0x00 && pBuf[(j + 1) * 32 == 0x00]) return FALSE;
			if(TRUE == IsEqual(pBuf + j * 32,(u8*)&pFile_Name,11)){
				MemCopy((u8*)pDir,pBuf + j * 32,32);
				free(pBuf);						//�ͷ�������ڴ�
				return TRUE;
			}
			(*pDir_Index)++;
		}
	}
	free(pBuf);									//�ͷ�������ڴ�

	return FALSE;
}
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
u8 ReadFile(u8 *pFileName,u32 Start,u32 Length,u8 *pFile)
{
	DIR Dir;
	u16 i,Dir_Index,Clus_Index,Next_Clus;
	u32 File_Size,Sec_Addr;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;			//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);				//��������
	if(FindFile(pFileName,&Dir,&Dir_Index) != TRUE){
		free(pBuf);							//�ͷ�������ڴ�
		return FALSE;
	}

	Clus_Index = (Dir.FilePositL[1] << 8)+ Dir.FilePositL[0];		//����ļ���ʼ�غ�
	File_Size = (Dir.FileSize[3] << 24) + (Dir.FileSize[2] << 16) 	//����ļ���С
				+ (Dir.FileSize[1] << 8) + Dir.FileSize[0];
	if(Start > File_Size){											//�������ʼ��ַ�����ļ���С�ͷ���
		free(pBuf);													//�ͷ�������ڴ�
		return FALSE;
	}
	if(Start + Length > File_Size) Length = File_Size - Start;		//��������Ͷ����ļ���β
	for(i = 0;i < Start / (Bpb.BytesPerSec * Bpb.SecPerClus);i++){	//���Ҫ������ʼ�غ�
		Clus_Index = ReadFAT(Clus_Index);
	}

	//--����һ������--//
										//���Ҫ���������ĵ�ַ
	Sec_Addr = Offset.Clus + Clus_Index * Bpb.SecPerClus + Start % (Bpb.SecPerClus * Bpb.BytesPerSec) / BLOCK_SIZE;
	ok = ReadSec(Sec_Addr,pBuf);
	if(ok != TRUE){
		free(pBuf);						//�ͷ�������ڴ�
		return FALSE;
	}
	for(i = Start % (Bpb.SecPerClus * Bpb.BytesPerSec) % BLOCK_SIZE;i < BLOCK_SIZE;i++){//����һ������Ҫ�������ݶ�Ӧ������
		*pFile++ = pBuf[i];
		Length--;
		if(Length == 0){

			free(pBuf);					//�ͷ�������ڴ�
			return TRUE;
		}
	}
	//--�����µ�����ֱ��Length�����ݶ�����--//
	while(1){
										//���д��һ���ؾ�Ѱ����һ���ص�ַ����֮д������ַSec_Addr��1
		if((double)((Sec_Addr - Offset.Clus + 1) / Bpb.SecPerClus) == ((double)(Sec_Addr - Offset.Clus + 1)) / Bpb.SecPerClus){
			Next_Clus = ReadFAT(Clus_Index);
			if(Next_Clus == FALSE){
				free(pBuf);				//�ͷ�������ڴ�
				return FALSE;
			}
			Clus_Index = Next_Clus;
										//���Ҫ������һ�����ĵ�ַ
			Sec_Addr = Offset.Clus + Clus_Index * Bpb.SecPerClus + Start % (Bpb.SecPerClus * Bpb.BytesPerSec) / BLOCK_SIZE;
		}
		else Sec_Addr++;
		if(Length < BLOCK_SIZE){		//�����һ����������
			ok = ReadSec(Sec_Addr,pBuf);
			if(ok != TRUE){
				free(pBuf);				//�ͷ�������ڴ�
				return FALSE;
			}
			for(i = 0;i < Length;i++)	//�����һ������Ҫ�������ݶ���
				*pFile++ = pBuf[i];
			free(pBuf);					//�ͷ�������ڴ�
			return TRUE;
		}
		ok = ReadSec(Sec_Addr,pFile);	//����ȥ��һ�������һ��������
		if(ok != TRUE){
			free(pBuf);					//�ͷ�������ڴ�
			return FALSE;
		}
		pFile += BLOCK_SIZE;
		Length -= BLOCK_SIZE;
	}

}
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
u8 CreatFile(u8 *pFileName,u32 Size,u8 *pFile)
{
	DIR Dir;
	InitDIR(&Dir);
	u8 i,j;
	u16 Dir_Index,Clus_Index,Next_Clus;
	u16 Clus_Num = Size / (Bpb.BytesPerSec * Bpb.SecPerClus) + 1;	//�ļ��ܹ�Ҫ�õ��Ĵ���Ŀ
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL){							//���붯̬�ڴ�ʧ�ܣ�����
		free(pBuf);								//�ͷ�������ڴ�
		return FALSE;
	}
	MemSet(pBuf,BLOCK_SIZE);					//��������

	//--�����ļ��Ƿ���ڣ����ҿ�Ŀ¼���FAT��--//
	ok = FindFile(pFileName,&Dir,&Dir_Index);	//�ļ����Ѿ����ڻ�Ƿ��ļ���
	if(ok == TRUE || ok == NAME_ERROR){
		free(pBuf);								//�ͷ�������ڴ�
		return FALSE;
	}
	Dir_Index = FindEmptyDIR();					//Ѱ�ҿյ��ļ�Ŀ¼��
	if(Dir_Index == FALSE){
		free(pBuf);								//�ͷ�������ڴ�
		return FALSE;
	}
	Clus_Index = FindEmptyFAT(0);				//Ѱ�ҿյ��ļ��������
	if(Clus_Index == FALSE){
		free(pBuf);								//�ͷ�������ڴ�
		return FALSE;
	}

	//--���ļ���λ�úʹ�С�ĸߵ�λ������д��--//
	Dir.FilePositL[1] = Clus_Index >> 8;
	Dir.FilePositL[0] = Clus_Index;
	Dir.FileSize[3] = Size >> 24;
	Dir.FileSize[2] = Size >> 16;
	Dir.FileSize[1] = Size >> 8;
	Dir.FileSize[0] = Size;
	Dir.FileAttrib = 0x20;
	ok = WriteDIR(Dir_Index,&Dir);
	if(ok != TRUE){
		//�ͷ�������ڴ�
		free(pBuf);
		return FALSE;
	}

	//--д�ļ��������ļ�����--//
	for(j = 0;j < Clus_Num;j++){
		if(j == Clus_Num - 1){							//д���һ����
			if(TRUE != WriteFAT(Clus_Index,0xFFFF)){	//д�ļ������
				free(pBuf);								//�ͷ�������ڴ�
				return FALSE;
			}
			for(i = 0;i < Bpb.SecPerClus;i++){			//д�ļ�����
				MemCopy(pBuf,pFile + (j * Bpb.SecPerClus + i) * Bpb.BytesPerSec,Size > BLOCK_SIZE ? BLOCK_SIZE : Size);
				ok = WriteSec(Offset.Clus + Clus_Index * Bpb.SecPerClus + i,pBuf);
				if(ok != TRUE){
					free(pBuf);							//�ͷ�������ڴ�
					return FALSE;
				}
				if(Size <= BLOCK_SIZE){					//���һ������д��ͷ���
					free(pBuf);							//�ͷ�������ڴ�
					return TRUE;
				}
				Size -= BLOCK_SIZE;
			}
		}
		else{
			Next_Clus = FindEmptyFAT(Clus_Index);		//Ѱ����һ���մ�
			if(TRUE != WriteFAT(Clus_Index,Next_Clus)){	//д�ļ������
				free(pBuf);								//�ͷ�������ڴ�
				return FALSE;
			}
			for(i = 0;i < Bpb.SecPerClus;i++){			//д�ļ�����
				ok = WriteSec(Offset.Clus + Clus_Index * Bpb.SecPerClus + i,pFile + Bpb.BytesPerSec * (j * Bpb.SecPerClus + i));
				if(ok != TRUE){
					free(pBuf);							//�ͷ�������ڴ�
					return FALSE;
				}
				Size -= BLOCK_SIZE;
			}
			Clus_Index = Next_Clus;
		}
	}
	free(pBuf);											//�ͷ�������ڴ�

	return TRUE;
}
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
u8 WriteFile(u8 *pFileName,u32 Start,u32 Length,u8 *pFile)
{
	DIR Dir;
	u16 i,Dir_Index,Clus_Index,Next_Clus;
	u32 File_Size,Sec_Addr,Length_Temp = Length;
	u8 ok,*pBuf = malloc(BLOCK_SIZE);

	if(pBuf == NULL) return FALSE;						//���붯̬�ڴ�ʧ�ܣ�����
	MemSet(pBuf,BLOCK_SIZE);							//��������
	if(FindFile(pFileName,&Dir,&Dir_Index) != TRUE){
		free(pBuf);										//�ͷ�������ڴ�
		return FALSE;
	}

	Clus_Index = (Dir.FilePositL[1] << 8)+ Dir.FilePositL[0];		//����ļ���ʼ�غ�
	File_Size = (Dir.FileSize[3] << 24) + (Dir.FileSize[2] << 16)	//����ļ���С
				+ (Dir.FileSize[1] << 8) + Dir.FileSize[0];
	if(Start > File_Size){											//���д��ʼ��ַ�����ļ���С�ͷ���
		free(pBuf);													//�ͷ�������ڴ�
		return FALSE;
	}
	for(i = 0;i < Start / (Bpb.BytesPerSec * Bpb.SecPerClus);i++){	//���Ҫд����ʼ�غ�
		Clus_Index = ReadFAT(Clus_Index);
	}
	//--д��һ������--//
													//���Ҫд�������ĵ�ַ
	Sec_Addr = Offset.Clus + Clus_Index * Bpb.SecPerClus + Start % (Bpb.SecPerClus * Bpb.BytesPerSec) / BLOCK_SIZE;
	ok = ReadSec(Sec_Addr,pBuf);
	if(ok != TRUE){
		free(pBuf);									//�ͷ�������ڴ�
		return FALSE;
	}
													//����һ������Ҫд�����ݶ�Ӧд�뻺��
	for(i = Start % (Bpb.SecPerClus * Bpb.BytesPerSec) % BLOCK_SIZE;i < BLOCK_SIZE;i++){
		pBuf[i] = *pFile++;
		Length--;
		if(Length == 0) break;
	}
	ok = WriteSec(Sec_Addr,pBuf);					//����һ����������д��
	if(ok != TRUE || Length == 0){
		free(pBuf);									//�ͷ�������ڴ�
		if(File_Size < Start + Length_Temp -Length){//����ļ�����޸ĸ�Ŀ¼����ļ���С
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
	//--д���µ�����ֱ��Length�����ݶ�д��--//
	while(1){													//���д��һ���ؾ�Ѱ����һ���ص�ַ����֮д������ַSec_Addr��1
		if((double)((Sec_Addr - Offset.Clus + 1) / Bpb.SecPerClus) == ((double)(Sec_Addr - Offset.Clus + 1)) / Bpb.SecPerClus){
			Next_Clus = ReadFAT(Clus_Index);
			if(Next_Clus == FALSE){
				free(pBuf);										//�ͷ�������ڴ�
				if(File_Size < Start + Length_Temp -Length){	//����ļ�����޸ĸ�Ŀ¼����ļ���С
					File_Size = Start + Length_Temp -Length;
					Dir.FileSize[0] = File_Size;
					Dir.FileSize[1] = File_Size >> 8;
					Dir.FileSize[2] = File_Size >> 16;
					Dir.FileSize[3] = File_Size >> 24;
					WriteDIR(Dir_Index,&Dir);
				}
				return FALSE;
			}
			if(Next_Clus == 0xFFFF){							//���д��ԭ�ļ����һ���أ������һ���´�
				Next_Clus = FindEmptyFAT(Clus_Index);
				ok = WriteFAT(Clus_Index,Next_Clus);
				if(ok != TRUE){
					free(pBuf);									//�ͷ�������ڴ�
					if(File_Size < Start + Length_Temp -Length){//����ļ�����޸ĸ�Ŀ¼����ļ���С
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
					free(pBuf);									//�ͷ�������ڴ�
					if(File_Size < Start + Length_Temp -Length){//����ļ�����޸ĸ�Ŀ¼����ļ���С
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
			Sec_Addr = Offset.Clus + Clus_Index * Bpb.SecPerClus;//���Ҫд�������ĵ�ַ
		}
		else Sec_Addr++;

		if(Length < BLOCK_SIZE){								//д���һ����������
			ok = ReadSec(Sec_Addr,pBuf);
			if(ok != TRUE){
				free(pBuf);										//�ͷ�������ڴ�
				if(File_Size < Start + Length_Temp -Length){	//����ļ�����޸ĸ�Ŀ¼����ļ���С
					File_Size = Start + Length_Temp -Length;
					Dir.FileSize[0] = File_Size;
					Dir.FileSize[1] = File_Size >> 8;
					Dir.FileSize[2] = File_Size >> 16;
					Dir.FileSize[3] = File_Size >> 24;
					WriteDIR(Dir_Index,&Dir);
				}
				return FALSE;
			}
			for(i = 0;i < Length;i++)							//�����һ������Ҫд�����ݶ�Ӧд�뻺��
				pBuf[i] = *pFile++;
			ok = WriteSec(Sec_Addr,pBuf);						//�����һ����������д��
			free(pBuf);											//�ͷ�������ڴ�
			if(ok == TRUE) Length = 0;
			if(File_Size < Start + Length_Temp -Length){		//����ļ�����޸ĸ�Ŀ¼����ļ���С
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

		ok = WriteSec(Sec_Addr,pFile);							//д��ȥ��һ�������һ��������
		if(ok != TRUE){
			free(pBuf);											//�ͷ�������ڴ�
			if(File_Size < Start + Length_Temp -Length){		//����ļ�����޸ĸ�Ŀ¼����ļ���С
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
