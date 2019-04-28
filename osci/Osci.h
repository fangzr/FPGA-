/*
 * Osci.h
 *
 *  Created on: 2017��6��30��
 *      Author: Peter
 */

#ifndef SRC_OSCI_OSCI_H_
#define SRC_OSCI_OSCI_H_

#define rd_data_clk_Regs OSCILLOSCOPE_S00_AXI_SLV_REG0_OFFSET
#define level_Regs 		 OSCILLOSCOPE_S00_AXI_SLV_REG1_OFFSET
#define start_Regs		 OSCILLOSCOPE_S00_AXI_SLV_REG2_OFFSET
#define Precnt_Regs		 OSCILLOSCOPE_S00_AXI_SLV_REG3_OFFSET
#define clkdiv_Regs		 OSCILLOSCOPE_S00_AXI_SLV_REG4_OFFSET
#define timeout_flg_Regs OSCILLOSCOPE_S00_AXI_SLV_REG5_OFFSET
#define dout_Regs		 OSCILLOSCOPE_S00_AXI_SLV_REG6_OFFSET
#define Oscilloscope_ADDR 0x44A10000
#define AD_Regs			 OSCILLOSCOPE_S00_AXI_SLV_REG7_OFFSET

#include "Oscilloscope.h"

extern volatile u8 data[600];
extern volatile u8 datalast[600];
extern volatile u8 level;
extern volatile u8 clkdiv;
extern volatile u8 yscale;
extern volatile u8 yscale_last;

//��ʼ��ʾ����״̬��
//����1.������ƽ 2.ʱ�ӷ�Ƶ��С
void InitOsci(u8 level,u8 clkdiv);

//����һ�����ݲɼ�
//ʹStart�źű���1һ��ʱ��
void StartOsci();

//�ɼ������жϺ���
//����Ϊ1��Ϊ׼�������ݶ�ȡ
//����Ϊ0��Ϊδ׼���ö�ȡ
u8 Readyread();

//�ɼ�һ�����ݣ����ж��Ƿ����Ч�������ѯ100�λ���û�����ݶ�ȡ����������βɼ�
//����fifo500�����ݸ�����
u8 SamplingFifo();

//��480*800��TFT��Ļ�����������Ʋ���
void DrawWaveform();
void oscilloscope(u8 level,u8 clkdiv);

#endif /* SRC_OSCI_OSCI_H_ */
