/*
 * keys.c
 *
 *  Created on: 2016Äê9ÔÂ1ÈÕ
 *      Author: Administrator
 */
#include<xgpio.h>
#include<xparameters.h>
volatile int KeyAction;
volatile int KeyValue;

XGpio gpioKey;

void KeysInit()
{
	XGpio_Initialize (&gpioKey, XPAR_HIER_PERIPH_GPIO_KEYS_DEVICE_ID);
	//if(sts != XST_SUCCESS) printf("Gpio led initial error\n");
}
void KeysTick()
{
	static u8 keyLast = 0;
	KeyValue = XGpio_DiscreteRead (&gpioKey, 1);
	if(KeyValue !=keyLast)
	{
		KeyAction = 1;
	}
	keyLast = KeyValue;
}
