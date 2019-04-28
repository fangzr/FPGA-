/*
 * SD_spi_Solution.c
 *
 *  Created on: 2012-2-10
 *      Author: Ming
 */

#include"SD_spi_Solution.h"

u8 spi_speed = 10;//the spi speed(0-255),0 is fastest


//set CS low
void CS_Enable()
{
	//set CS low
	Xil_Out8(CS_BASE, 0x00);
}

//set CS high and send 8 clocks
void CS_Disable()
{
	//set CS high
	Xil_Out8(CS_BASE, 0x01);
	//send 8 clocks
	SDWriteByte(0xff);
}

//write a byte
void SDWriteByte(u8 data)
{
	u8 i;

	//write 8 bits(MSB)
	for(i = 0;i < 8;i ++)
	{
		Xil_Out8(SCLK_BASE, 0x00);
		usleep(spi_speed);
		if(data & 0x80) Xil_Out8(DI_BASE, 0x01);
		else Xil_Out8(DI_BASE, 0x00);
		data <<= 1;
		Xil_Out8(SCLK_BASE, 0x01);
		usleep(spi_speed);
	}

	//when DI is free,it should be set high
	Xil_Out8(DI_BASE, 0x01);
}

//read a byte
u8 SDReadByte()
{
	u8 data = 0x00,i;

	//read 8 bit(MSB)
	for(i = 0;i < 8;i ++)
	{
		Xil_Out8(SCLK_BASE, 0x00);
		usleep(spi_speed);
		Xil_Out8(SCLK_BASE, 0x01);
		data <<= 1;
		if(Xil_In8(DO_BASE)) data |= 0x01;
		usleep(spi_speed);
	}

	return data;
}

//send a command and send back the response
u8  SDSendCmd(u8 cmd,u32 arg,u8 crc)
{
	u8 r1,time = 0;

	//send the command,arguments and CRC
	SDWriteByte((cmd & 0x3f) | 0x40);
	SDWriteByte(arg >> 24);
	SDWriteByte(arg >> 16);
	SDWriteByte(arg >> 8);
	SDWriteByte(arg);
	SDWriteByte(crc);

	//read the respond until responds is not '0xff' or timeout
	do{
		r1 = SDReadByte();
		time ++;
		//if time out,return
		if(time > 254) break;
	}while(r1 == 0xff);

	return r1;
}

//reset SD card
u8 SDReset()
{
	u8 i,r1,time = 0;

	//set CS high
	CS_Disable();

	//send 128 clocks
	for(i = 0;i < 16;i ++)
	{
		SDWriteByte(0xff);
	}

	//set CS low
	CS_Enable();

	//send CMD0 till the response is 0x01
	do{
		r1 = SDSendCmd(CMD0,0,0x95);
		time ++;
		//if time out,set CS high and return r1
		if(time > 254)
		{
			//set CS high and send 8 clocks
			CS_Disable();
			return r1;
		}
	}while(r1 != 0x01);

	//set CS high and send 8 clocks
	CS_Disable();

	return 0;
}

//initial SD card(send CMD55+ACMD41 or CMD1)
u8 SDInit()
{
	u8 r1,time = 0;

	//set CS low
	CS_Enable();

	//check interface operating condition
	r1 = SDSendCmd(CMD8,0x000001aa,0x87);
	//if support Ver1.x,but do not support Ver2.0,set CS high and return r1
	if(r1 == 0x05)
	{
		//set CS high and send 8 clocks
		CS_Disable();
		return r1;
	}
	//read the other 4 bytes of response(the response of CMD8 is 5 bytes)
	r1=SDReadByte();
	r1=SDReadByte();
	r1=SDReadByte();
	r1=SDReadByte();

	do{
		//send CMD55+ACMD41 to initial SD card
		do{
			r1 = SDSendCmd(CMD55,0,0xff);
			time ++;
			//if time out,set CS high and return r1
			if(time > 254)
			{
				//set CS high and send 8 clocks
				CS_Disable();
				return r1;
			}
		}while(r1 != 0x01);

		r1 = SDSendCmd(ACMD41,0x40000000,0xff);

		//send CMD1 to initial SD card
		//r1 = SDSendCmd(CMD1,0x00ffc000,0xff);
		time ++;

		//if time out,set CS high and return r1
		if(time > 254)
		{
			//set CS high and send 8 clocks
			CS_Disable();
			return r1;
		}
	}while(r1 != 0x00);

	//set CS high and send 8 clocks
	CS_Disable();

	return 0;
}

//read a single sector
u8 SDReadSector(u32 addr,u8 * buffer)
{
	u8 r1;
	u16 i,time = 0;

	//set CS low
	CS_Enable();

	//send CMD17 for single block read
	r1 = SDSendCmd(CMD17,addr << 9,0x55);
	//if CMD17 fail,return
	if(r1 != 0x00)
	{
		//set CS high and send 8 clocks
		CS_Disable();
		return r1;
	}

	//continually read till get the start byte 0xfe
	do{
		r1 = SDReadByte();
		time ++;
		//if time out,set CS high and return r1
		if(time > 30000)
		{
			//set CS high and send 8 clocks
			CS_Disable();
			return r1;
		}
	}while(r1 != 0xfe);

	//read 512 Bits of data
	for(i = 0;i < 512;i ++)
	{
		buffer[i] = SDReadByte();
	}

	//read two bits of CRC
	SDReadByte();
	SDReadByte();

	//set CS high and send 8 clocks
	CS_Disable();

	return 0;
}

//read multiple sectors
u8 SDReadMultiSector(u32 addr,u8 sector_num,u8 * buffer)
{
	u16 i,time = 0;
	u8 r1;

	//set CS low
	CS_Enable();

	//send CMD18 for multiple blocks read
	r1 = SDSendCmd(CMD18,addr << 9,0xff);
	//if CMD18 fail,return
	if(r1 != 0x00)
	{
		//set CS high and send 8 clocks
		CS_Disable();
		return r1;
	}

	//read sector_num sector
	do{
		//continually read till get start byte
		do{
			r1 = SDReadByte();
			time ++;
			//if time out,set CS high and return r1
			if(time > 30000 || ((r1 & 0xf0) == 0x00 && (r1 & 0x0f)))
			{
				//set CS high and send 8 clocks
				CS_Disable();
				return r1;
			}
		}while(r1 != 0xfe);
		time = 0;

		//read 512 Bits of data
		for(i = 0;i < 512;i ++)
		{
			*buffer ++ = SDReadByte();
		}

		//read two bits of CRC
		SDReadByte();
		SDReadByte();
	}while( -- sector_num);
	time = 0;

	//stop multiple reading
	r1 = SDSendCmd(CMD12,0,0xff);

	//set CS high and send 8 clocks
	CS_Disable();

	return 0;
}

//write a single sector
u8 SDWriteSector(u32 addr,u8 * buffer)
{
	u16 i,time = 0;
	u8 r1;

	//set CS low
	CS_Enable();

	do{
		do{
			//send CMD24 for single block write
			r1 = SDSendCmd(CMD24,addr << 9,0xff);
			time ++;
			//if time out,set CS high and return r1
			if(time > 254)
			{
				//set CS high and send 8 clocks
				CS_Disable();
				return r1;
			}
		}while(r1 != 0x00);
		time = 0;

		//send some dummy clocks
		for(i = 0;i < 5;i ++)
		{
			SDWriteByte(0xff);
		}

		//write start byte
		SDWriteByte(0xfe);

		//write 512 bytes of data
		for(i = 0;i < 512;i ++)
		{
			SDWriteByte(buffer[i]);
		}

		//write 2 bytes of CRC
		SDWriteByte(0xff);
		SDWriteByte(0xff);

		//read response
		r1 = SDReadByte();
		time ++;
		//if time out,set CS high and return r1
		if(time > 254)
		{
			//set CS high and send 8 clocks
			CS_Disable();
			return r1;
		}
	}while((r1 & 0x1f)!= 0x05);
	time = 0;

	//check busy
	do{
		r1 = SDReadByte();
		time ++;
		//if time out,set CS high and return r1
		if(time > 60000)
		{
			//set CS high and send 8 clocks
			CS_Disable();
			return r1;
		}
	}while(r1 != 0xff);

	//set CS high and send 8 clocks
	CS_Disable();

	return 0;
}

//write several blocks
u8 SDWriteMultiSector(u32 addr,u8 sector_num,u8 * buffer)
{
	u16 i,time = 0;
	u8 r1;

	//set CS low
	CS_Enable();

	//send CMD25 for multiple block read
	r1 = SDSendCmd(CMD25,addr << 9,0xff);
	//if CMD25 fail,return
	if(r1 != 0x00)
	{
		//set CS high and send 8 clocks
		CS_Disable();
		return r1;
	}

	do{
		do{
			//send several dummy clocks
			for(i = 0;i < 5;i ++)
			{
				SDWriteByte(0xff);
			}

			//write start byte
			SDWriteByte(0xfc);

			//write 512 byte of data
			for(i = 0;i < 512;i ++)
			{
				SDWriteByte(*buffer ++);
			}

			//write 2 byte of CRC
			SDWriteByte(0xff);
			SDWriteByte(0xff);

			//read response
			r1 = SDReadByte();
			time ++;
			//if time out,set CS high and return r1
			if(time > 254)
			{
				//set CS high and send 8 clocks
				CS_Disable();
				return r1;
			}
		}while((r1 & 0x1f)!= 0x05);
		time = 0;

		//check busy
		do{
			r1 = SDReadByte();printf("n%d",r1);
			time ++;
			//if time out,set CS high and return r1
			if(time > 30000)
			{
				//set CS high and send 8 clocks
				CS_Disable();
				return r1;
			}
		}while(r1 != 0xff);
		time = 0;
	}while(-- sector_num);

	//send stop byte
	SDWriteByte(0xfd);

	//check busy
	do{
		r1 = SDReadByte();
		time ++;
		//if time out,set CS high and return r1
		if(time > 30000)
		{
			//set CS high and send 8 clocks
			CS_Disable();
			return r1;
		}
	}while(r1 != 0xff);

	//set CS high and send 8 clocks
	CS_Disable();

	return 0;
}

//get CID or CSD
u8 SDGetCIDCSD(u8 cid_csd,u8 * buffer)
{
	u8 r1;
	u16 i,time = 0;

	//set CS low
	CS_Enable();

	//send CMD10 for CID read or CMD9 for CSD
	do{
		if(cid_csd == CID)
			r1 = SDSendCmd(CMD10,0,0xff);
		else
			r1 = SDSendCmd(CMD9,0,0xff);
		time ++;
		//if time out,set CS high and return r1
		if(time > 254)
		{
			//set CS high and send 8 clocks
			CS_Disable();
			return r1;
		}
	}while(r1 != 0x00);
	time = 0;

	//continually read till get 0xfe
	do{
		r1 = SDReadByte();
		time ++;
		//if time out,set CS high and return r1
		if(time > 30000)
		{
			//set CS high and send 8 clocks
			CS_Disable();
			return r1;
		}
	}while(r1 != 0xfe);

	//read 512 Bits of data
	for(i = 0;i < 16;i ++)
	{
		*buffer ++ = SDReadByte();
	}

	//read two bits of CRC
	SDReadByte();
	SDReadByte();

	//set CS high and send 8 clocks
	CS_Disable();

	return 0;
}

