/*
 * MB_USLEEP.c
 *
 *  Created on: 2017年1月19日
 *      Author: Peter
 */
#include "MB_USLEEP.h"
/*经过示波器验证过了，比1us长一点，大概为1.1us*/
void MB_USLEEP(unsigned int Microsecond)
{
	int i,time;
	time = Microsecond * 6;
	for(i = 0;i < time;i++);
}

