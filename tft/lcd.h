/*
 * TFT.h
 *
 *  Created on: 2017��1��19��
 *      Author: Peter
 */

#ifndef TFT_H_
#define TFT_H_

/*
 * lcd.h
 *
 *  Created on: 2016��3��31��
 *      Author: YWT
 */

#ifndef LCD_H_
#define LCD_H_




#include "stdlib.h"
#include <xil_io.h>
#include "xil_types.h"

/*����GPIO����ַ����*/
#define TFT_RS_BASE       0x40020000
#define TFT_CS_BASE       0x40030000
#define TFT_WR_BASE       0x40040000
#define TFT_RD_BASE       0x40050000
#define TFT_DATA_BASE     0x40080000
#define TFT_DATA_TRI_BASE 0x40080004//+0x04
#define TFT_RST_BASE      0x40060000

//LCD��Ҫ������
typedef struct
{
	u16 width;			//LCD ����
	u16 height;			//LCD �߶�
	u16 id;				//LCD ID
	u8  dir;			//���������������ƣ�0��������1��������
	u16	wramcmd;		//��ʼдgramָ��
	u16 setxcmd;		//����x����ָ��
	u16  setycmd;		//����y����ָ��
}_lcd_dev;

//LCD����
extern _lcd_dev lcddev;	//����LCD��Ҫ����
//LCD�Ļ�����ɫ�ͱ���ɫ
extern u16  POINT_COLOR;//Ĭ�Ϻ�ɫ
extern u16  BACK_COLOR; //������ɫ.Ĭ��Ϊ��ɫ

////////////////////////////////////////////////////////////////////
//-----------------LCD�˿ڶ���----------------

#define	LCD_CS_SET  *(u8 *)(TFT_CS_BASE) = 0x01;//IOWR_ALTERA_AVALON_PIO_SET_BITS(LCD_CON_BASE, 0x01)    //Ƭѡ�˿�  		GPIO_0_0
#define	LCD_RS_SET	*(u8 *)(TFT_RS_BASE) = 0x01;//IOWR_ALTERA_AVALON_PIO_SET_BITS(LCD_CON_BASE, 0x02)    //����/���� 		GPIO_0_1
#define	LCD_WR_SET	*(u8 *)(TFT_WR_BASE) = 0x01;//IOWR_ALTERA_AVALON_PIO_SET_BITS(LCD_CON_BASE, 0x04)    //д����			GPIO_0_2
#define	LCD_RD_SET	*(u8 *)(TFT_RD_BASE) = 0x01;//IOWR_ALTERA_AVALON_PIO_SET_BITS(LCD_CON_BASE, 0x08)    //������			GPIO_0_3

#define	LCD_CS_CLR  *(u8 *)(TFT_CS_BASE) = 0x00;//IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(LCD_CON_BASE, 0x01)		//Ƭѡ�˿�
#define	LCD_RS_CLR	*(u8 *)(TFT_RS_BASE) = 0x00;//IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(LCD_CON_BASE, 0x02)     //����/����
#define	LCD_WR_CLR	*(u8 *)(TFT_WR_BASE) = 0x00;//IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(LCD_CON_BASE, 0x04)     //д����
#define	LCD_RD_CLR	*(u8 *)(TFT_RD_BASE) = 0x00;//IOWR_ALTERA_AVALON_PIO_CLEAR_BITS(LCD_CON_BASE, 0x08)     //������



#define LCD_DATA_O(x)  *(u16 *)(TFT_DATA_BASE) = x ;//IOWR_ALTERA_AVALON_PIO_DATA(LCD_DATA_BASE,x)		//�������
#define LCD_DATA_I     *(u16 *)(TFT_DATA_BASE)//IORD_ALTERA_AVALON_PIO_DATA(LCD_DATA_BASE)		//��������

#define LCD_DATA_OUT  *(u16 *)(TFT_DATA_TRI_BASE) = 0x0000;//IOWR_ALTERA_AVALON_PIO_DIRECTION(LCD_DATA_BASE,0xffff)		//����LCD_DATA��PIOΪ���
#define LCD_DATA_IN   *(u16 *)(TFT_DATA_TRI_BASE) = 0xffff;//IOWR_ALTERA_AVALON_PIO_DIRECTION(LCD_DATA_BASE,0x0)		////����LCD_DATA��PIOΪ����


/*ǿ�ҽ����ȸ�λ������ʼ��*/
#define LCD_RST		*(u8 *)(TFT_RST_BASE) = 0x00;\
*(u8 *)(TFT_RST_BASE) = 0x01;

//////////////////////////////////////////////////////////////////////
//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��

//ɨ�跽����
#define L2R_U2D  0 //������,���ϵ���
#define L2R_D2U  1 //������,���µ���
#define R2L_U2D  2 //���ҵ���,���ϵ���
#define R2L_D2U  3 //���ҵ���,���µ���

#define U2D_L2R  4 //���ϵ���,������
#define U2D_R2L  5 //���ϵ���,���ҵ���
#define D2U_L2R  6 //���µ���,������
#define D2U_R2L  7 //���µ���,���ҵ���

#define DFT_SCAN_DIR  L2R_U2D  //Ĭ�ϵ�ɨ�跽��

//������ɫ
#define WHITE         	 0xFFFF
#define BLACK         	 0x0000
#define BLUE         	 0x001F
#define BRED             0XF81F
#define GRED 			 0XFFE0
#define GBLUE			 0X07FF
#define RED           	 0xF800
#define MAGENTA       	 0xF81F
#define GREEN         	 0x07E0
#define CYAN          	 0x7FFF
#define YELLOW        	 0xFFE0
#define BROWN 			 0XBC40 //��ɫ
#define BRRED 			 0XFC07 //�غ�ɫ
#define GRAY  			 0X8430 //��ɫ
//GUI��ɫ

#define DARKBLUE      	 0X01CF	//����ɫ
#define LIGHTBLUE      	 0X7D7C	//ǳ��ɫ
#define GRAYBLUE       	 0X5458 //����ɫ
//������ɫΪPANEL����ɫ

#define LIGHTGREEN     	 0X841F //ǳ��ɫ
#define LGRAY 			 0XC618 //ǳ��ɫ(PANNEL),���屳��ɫ

#define LGRAYBLUE        0XA651 //ǳ����ɫ(�м����ɫ)
#define LBBLUE           0X2B12 //ǳ����ɫ(ѡ����Ŀ�ķ�ɫ)

void LCD_Init(void);													   	//��ʼ��
void LCD_DisplayOn(void);													//����ʾ
void LCD_DisplayOff(void);													//����ʾ
void LCD_Clear(u16 Color);	 												//����
void LCD_SetCursor(u16 Xpos, u16 Ypos);										//���ù��
void LCD_DrawPoint(u16 x,u16 y);											//����
void LCD_Fast_DrawPoint(u16 x,u16 y,u16 color);								//���ٻ���
u16  LCD_ReadPoint(u16 x,u16 y); 											//����
void LCD_Draw_Circle(u16 x0,u16 y0,u8 r);					    			//��Բ
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2);							//����
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2);		   				//������
void LCD_DrawTriangle_pos(u16 x,u16 y,u16 d);								//�����ȱ�������
void LCD_DrawTriangle_neg(u16 x,u16 y,u16 d);								//�����ȱ�������
void LCD_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 color);		   				//��䵥ɫ
void LCD_Color_Fill(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color);				//���ָ����ɫ
void LCD_ShowChar(u16 x,u16 y,u8 num,u8 size,u8 mode);						//��ʾһ���ַ�
void LCD_ShowNum(u16 x,u16 y,u32 num,u8 len,u8 size);  						//��ʾһ������
void LCD_ShowxNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode);				//��ʾ ����
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,u8 *p);		//��ʾһ���ַ���,12/16����
void LCD_showString(u16 x,u16 y,u8 *p);

/*Peter ����ĺ���*/
void LCD_CalDispaly();

void LCD_WR_REG(u16 data);
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_RegValue);
u16 LCD_ReadReg(u16 LCD_Reg);
u16 LCD_RD_DATA(void);
void LCD_WriteRAM_Prepare(void);
void LCD_WriteRAM(u16 RGB_Code);
void LCD_Scan_Dir(u8 dir);									//������ɨ�跽��
void LCD_Display_Dir(u8 dir);								//������Ļ��ʾ����
void LCD_Set_Window(u16 sx,u16 sy,u16 width,u16 height);	//���ô���
void LCD_WR_DATAX(u16 data);
void changechar(char test[],char num[],int ndigit,int decpt,int sign);

//д���ݺ���	�궨�����ʱ�ٶȿ�
#define LCD_WR_DATA(data){\
LCD_DATA_OUT;\
LCD_CS_CLR;\
LCD_RS_SET;\
LCD_DATA_O(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
}

//LCD�ֱ�������
#define SSD_HOR_RESOLUTION		800		//LCDˮƽ�ֱ���
#define SSD_VER_RESOLUTION		480		//LCD��ֱ�ֱ���
//LCD������������
#define SSD_HOR_PULSE_WIDTH		1		//ˮƽ����
#define SSD_HOR_BACK_PORCH		46		//ˮƽǰ��
#define SSD_HOR_FRONT_PORCH		210		//ˮƽ����

#define SSD_VER_PULSE_WIDTH		1		//��ֱ����
#define SSD_VER_BACK_PORCH		23		//��ֱǰ��
#define SSD_VER_FRONT_PORCH		22		//��ֱǰ��
//���¼����������Զ�����
#define SSD_HT	(SSD_HOR_RESOLUTION+SSD_HOR_BACK_PORCH+SSD_HOR_FRONT_PORCH)
#define SSD_HPS	(SSD_HOR_BACK_PORCH)
#define SSD_VT 	(SSD_VER_RESOLUTION+SSD_VER_BACK_PORCH+SSD_VER_FRONT_PORCH)
#define SSD_VPS (SSD_VER_BACK_PORCH)



#endif /* LCD_H_ */


#endif /* TFT_H_ */