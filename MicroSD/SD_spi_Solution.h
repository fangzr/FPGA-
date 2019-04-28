/*
 * SD_spi_Solution.h
 *
 *  Created on: 2012-2-10
 *      Author: Ming
 */

#ifndef SD_SPI_SOLUTION_H_
#define SD_SPI_SOLUTION_H_

#include <stdio.h>
#include <xil_io.h>
#include<xil_types.h>
#include "sleep.h"

#define CMD0 	0	/* GO_IDLE_STATE */
#define CMD55 	55	/* APP_CMD */
#define ACMD41	41	/* SEND_OP_COND (ACMD) */
#define CMD1	1	/* SEND_OP_COND */
#define CMD17 	17	/* READ_SINGLE_BLOCK */
#define CMD8	8	/* SEND_IF_COND */
#define CMD18	18	/* READ_MULTIPLE_BLOCK */
#define CMD12	12	/* STOP_TRANSMISSION */
#define CMD24	24	/* WRITE_BLOCK */
#define CMD25	25	/* WRITE_MULTIPLE_BLOCK */
#define CMD13	13	/* SEND_STATUS */
#define CMD9	9	/* SEND_CSD */
#define CMD10	10	/* SEND_CID */

#define CSD		9
#define CID		10

#define SCLK_BASE 	0x44A30000 + 0x00
#define DI_BASE 	0x44A30000 + 0x04
#define CS_BASE 	0x44A30000 + 0x08
#define DO_BASE 	0x400C0000

//set CS low
void CS_Enable();

//set CS high and send 8 clocks
void CS_Disable();

//write a byte
void SDWriteByte(u8 data);

//read a byte
u8 SDReadByte();

//send a command and send back the response
u8  SDSendCmd(u8 cmd,u32 arg,u8 crc);

//reset SD card
u8 SDReset();

//initial SD card
u8 SDInit();

//read a single sector
u8 SDReadSector(u32 addr,u8 * buffer);

//read multiple sectors
u8 SDReadMultiSector(u32 addr,u8 sector_num,u8 * buffer);

//write a single sector
u8 SDWriteSector(u32 addr,u8 * buffer);

//write multiple sectors
u8 SDWriteMultiSector(u32 addr,u8 sector_num,u8 * buffer);

//get CID or CSD
u8 SDGetCIDCSD(u8 cid_csd,u8 * buffer);

//spi speed(0-255),0 is fastest
u8 spi_speed;
int SD_Test();

#endif /* SD_SPI_SOLUTION_H_ */
