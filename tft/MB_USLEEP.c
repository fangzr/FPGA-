/*
 * MB_USLEEP.c
 *
 *  Created on: 2017��1��19��
 *      Author: Peter
 */
#include "MB_USLEEP.h"
/*����ʾ������֤���ˣ���1us��һ�㣬���Ϊ1.1us*/
void MB_USLEEP(unsigned int Microsecond)
{
	int i,time;
	time = Microsecond * 6;
	for(i = 0;i < time;i++);
}

