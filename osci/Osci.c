/*
 * Osci.c
 *
 *  Created on: 2017��6��30��
 *      Author: Peter
 */
#include"Osci.h"
#include"microblaze_sleep.h"
#include "../tft/lcd.h"

#define FIFO_CONTROL_mWriteReg OSCILLOSCOPE_mWriteReg
#define FIFO_CONTROL_mReadReg OSCILLOSCOPE_mReadReg

#define rd_data_clk_1 FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, rd_data_clk_Regs, 0x01)
#define rd_data_clk_0 FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, rd_data_clk_Regs, 0x00)



volatile u8 data[600]={0};
volatile u8 datalast[600]={0};

volatile u8 level=80;
volatile u8 clkdiv = 0;
volatile u8 yscale = 0;
volatile u8 yscale_last=0;

#define xdisplay 2
//��ʼ��ʾ����״̬��
//����1.������ƽ 2.ʱ�ӷ�Ƶ��С
void InitOsci(u8 level,u8 clkdiv)
{
	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, level_Regs, level);
	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, clkdiv_Regs, clkdiv);

	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, Precnt_Regs, 250);
	rd_data_clk_0;
}

//����һ�����ݲɼ�
//ʹStart�źű���1һ��ʱ��
void StartOsci()
{
	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, start_Regs, 0x01);
	MB_Sleep(1);
	FIFO_CONTROL_mWriteReg(Oscilloscope_ADDR, start_Regs, 0x00);
}

//�ɼ������жϺ���
//����Ϊ1��Ϊ׼�������ݶ�ȡ
//����Ϊ0��Ϊδ׼���ö�ȡ
u8 Readyread()
{
	return FIFO_CONTROL_mReadReg(Oscilloscope_ADDR, timeout_flg_Regs);
}

//�ɼ�һ�����ݣ����ж��Ƿ����Ч�������ѯ100�λ���û�����ݶ�ȡ����������βɼ�
//����fifo500�����ݸ�����
u8 SamplingFifo()
{
	int i=0;
	while((Readyread()!=1))
	{
		i++;
		if(i>100)return 0;
	}
		for(i=0;i<500;i++)
		{
			rd_data_clk_1;
			datalast[i]=data[i];
			rd_data_clk_0;
			data[i] = ((((char)(FIFO_CONTROL_mReadReg(Oscilloscope_ADDR, dout_Regs)))+128)>>yscale);
		}
		return 1;
}

//��480*800��TFT��Ļ�����������Ʋ���
void DrawWaveform()
{
	int i,j=0;

	//�����ϴλ��ƵĲ���
	POINT_COLOR=BLACK;
	for(i=0,j=0;i<400 && j<800;i++)
	{
		if(datalast[i]!=data[i] || datalast[i+1]!=data[i+1])
		{
			LCD_DrawLine(j,(datalast[i]+100), j+xdisplay,(datalast[i+1]+100));

		}
		j=j+xdisplay;
	}
	//���Ʋ���
	POINT_COLOR=YELLOW;
	for(i=0,j=0;i<400 && j<800;i++)
	{
		LCD_DrawLine(j,(data[i]+100), j+xdisplay, (data[i+1]+100));
		j=j+xdisplay;
	}

}

void oscilloscope(u8 level,u8 clkdiv)
{
	u8 i;
	InitOsci(level,clkdiv);	//��ʼ��ʾ��������״̬��
	StartOsci();			//��ʼ����
	MB_Sleep(10);
	i=SamplingFifo();			//��ȡ����
	MB_Sleep(200);
	if(i!=0)
		DrawWaveform();			//���Ʋ���
}
