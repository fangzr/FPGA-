/*
 * SD_spi_selftest.c
 *
 *  Created on: 2017Äê7ÔÂ16ÈÕ
 *      Author: Peter
 */


/*
 * SD_spi_main.c
 *
 *  Created on: 2012-2-10
 *      Author: Ming
 */

#include"SD_spi_Solution.h"
#include"Fat16.h"
#include<stdlib.h>
#include<stdio.h>
int SD_Test()
{	u32 i;
	u8 ok;
	u8 buffer1[1024*600] = {'w','e','l','c','o','m','e','2','0','1','2','0','4','0','1'};
	u8 buffer2[1024*600] = {'h','e','l','l','o','w','o','r','l','d','2','0','1','2','0','4','0','1'};
	u8 buffer3[1024*600];

	if(InitFAT() == FALSE)
	{
		xil_printf("FALSE!\n\r");
		return 0;
	}
	xil_printf("goto here3!");
	ok = CreatFile("x9911.txt",512*65,buffer1);
	if(ok == TRUE) printf("Create succeed\n");
	else printf("Create fail\n");

	ok = CreatFile("x9811.txt",512*65,buffer1);
	if(ok == TRUE) printf("Create succeed\n");
	else printf("Create fail\n");

	ok = CreatFile("x9711.txt",512*65,buffer1);
	if(ok == TRUE) printf("Create succeed\n");
	else printf("Create fail\n");

	ok = WriteFile("x9811.txt",512*64,512*65,buffer2);
	if(ok == TRUE) printf("write succeed\n");
	else printf("write fail\n");

	ok = WriteFile("x9711.txt",0,512*65,buffer2);
	if(ok == TRUE) printf("write succeed\n");
	else printf("write fail\n");

	ok = ReadFile("x9911.txt",0,512*65,buffer3);
	if(ok == TRUE) printf("read succeed\n");
	else printf("read fail\n");
	for(i = 0;i < 1024;i ++)
	{
		printf("%c ",buffer3[i]);
	}
	printf("\n");

	ok = ReadFile("x9811.txt",0,512*65,buffer3);
	if(ok == TRUE) printf("read succeed\n");
	else printf("read fail\n");
	for(i = 512*64;i < 512*65;i ++)
	{
		printf("%c ",buffer3[i]);
	}
	printf("\n");

	ok = ReadFile("x9711.txt",0,512*65,buffer3);
	if(ok == TRUE) printf("read succeed\n");
	else printf("read fail\n");
	for(i = 0;i < 1024;i ++)
	{
		printf("%c ",buffer3[i]);
	}
	printf("\n");

	return 0;
}
