/*
 * fft.c
 *
 *  Created on: 2017��7��10��
 *      Author: Peter
 */
#include "fft.h"

/*����FFT�㷨*/
#include "math.h"
#include "stdio.h"
#include "../tft/lcd.h"
#include<math.h>

/*******************************************************************
����ԭ�ͣ�struct compx EE(struct compx b1,struct compx b2)
�������ܣ��������������г˷�����
��������������������嶨��ĸ���a,b
���������a��b�ĳ˻��������������ʽ���
*******************************************************************/
struct compx EE(struct compx a,struct compx b)
{
 struct compx c;
 c.real=a.real*b.real-a.imag*b.imag;
 c.imag=a.real*b.imag+a.imag*b.real;
 return(c);
}


/******************************************************************
����ԭ�ͣ�void create_sin_tab(float *sin_t)
�������ܣ�����һ�����Ҳ��������������븣��Ҷ�任������ͬ
���������*sin_t������ұ������ָ��
�����������
******************************************************************/
void create_sin_tab(float *sin_t)
{
  int i;
  for(i=0;i<FFT_N/2;i++)
  sin_t[i]=sin(2*PI*i/FFT_N);
}
/******************************************************************
����ԭ�ͣ�void sin_tab(float pi)
�������ܣ����ò��ķ�������һ����������ֵ
���������pi ��Ҫ��������ֵ����ֵ����Χ0--2*PI��������ʱ��Ҫת��
�������������ֵpi������ֵ
******************************************************************/
float sin_tab(float pi)
{
  int n;
  float a;
   n=(int)(pi*FFT_N/2/PI);

  if(n>=0&&n<FFT_N/2)
   a=SIN_TAB[n];
  else if(n>=FFT_N/2&&n<FFT_N)
    a=-SIN_TAB[n-FFT_N/2];
  return a;
}
/******************************************************************
����ԭ�ͣ�void cos_tab(float pi)
�������ܣ����ò��ķ�������һ����������ֵ
���������pi ��Ҫ��������ֵ����ֵ����Χ0--2*PI��������ʱ��Ҫת��
�������������ֵpi������ֵ
******************************************************************/
float cos_tab(float pi)
{
   float a,pi2;
   pi2=pi+PI/2;
   if(pi2>2*PI)
     pi2-=2*PI;
   a=sin_tab(pi2);
   return a;
}
/*****************************************************************
����ԭ�ͣ�void FFT(struct compx *xin,int N)
�������ܣ�������ĸ�������п��ٸ���Ҷ�任��FFT��
���������*xin�����ṹ������׵�ַָ�룬struct��
�����������
*****************************************************************/
void FFT(struct compx *xin)
{
  int f,m,nv2,nm1,i,k,l,j=0;
  struct compx u,w,t;

   nv2=FFT_N/2;             //��ַ���㣬������Ȼ˳���ɵ�λ�򣬲����׵��㷨
   nm1=FFT_N-1;
   for(i=0;i<nm1;i++)
   {
    if(i<j)                    //���i<j,�����б�ַ
     {
      t=xin[j];
      xin[j]=xin[i];
      xin[i]=t;
     }
    k=nv2;                    //��j����һ����λ��
    while(k<=j)               //���k<=j,��ʾj�����λΪ1
     {
      j=j-k;                 //�����λ���0
      k=k/2;                 //k/2���Ƚϴθ�λ���������ƣ�����Ƚϣ�ֱ��ĳ��λΪ0
     }
   j=j+k;                   //��0��Ϊ1
  }

  {
   int le,lei,ip;                            //FFT����ˣ�ʹ�õ����������FFT����
    f=FFT_N;
   for(l=1;(f=f/2)!=1;l++)              //����l��ֵ����������μ���
           ;
  for(m=1;m<=l;m++)                   // ���Ƶ��νἶ��
   {                               //m��ʾ��m�����Σ�lΪ���μ�����l=log��2��N
    le=2<<(m-1);                    //le���ν���룬����m�����εĵ��ν����le��
    lei=le/2;                         //ͬһ���ν��вμ����������ľ���
    u.real=1.0;                        //uΪ���ν�����ϵ������ʼֵΪ1
    u.imag=0.0;
    //w.real=cos(PI/lei);                  //�����ò������sinֵ��cosֵ
    // w.imag=-sin(PI/lei);
    w.real=cos_tab(PI/lei);             //wΪϵ���̣�����ǰϵ����ǰһ��ϵ������
    w.imag=-sin_tab(PI/lei);
    for(j=0;j<=lei-1;j++)              //���Ƽ��㲻ͬ�ֵ��νᣬ������ϵ����ͬ�ĵ��ν�
     {
      for(i=j;i<=FFT_N-1;i=i+le)       //����ͬһ���ν����㣬������ϵ����ͬ���ν�
       {
        ip=i+lei;                          //i��ip�ֱ��ʾ�μӵ�������������ڵ�
        t=EE(xin[ip],u);                   //�������㣬�����ʽ
        xin[ip].real=xin[i].real-t.real;
        xin[ip].imag=xin[i].imag-t.imag;
        xin[i].real=xin[i].real+t.real;
        xin[i].imag=xin[i].imag+t.imag;
       }
      u=EE(u,w);                          //�ı�ϵ����������һ����������
     }
   }
  }

}
/************************************************************
����ԭ�ͣ�void Init_fftsintable()
�������ܣ���ʼ��FFT������Ҫ�����ұ�
�����������
�����������
************************************************************/

void Init_fftsintable()
{
	create_sin_tab(SIN_TAB);
	POINT_COLOR=WHITE;
	FFT_frame();
}
/************************************************************
����ԭ�ͣ�void Draw_FFT()
�������ܣ�����FFT�������Ƶ��ͼ
�����������
�����������
************************************************************/

void Draw_FFT()
{
 	LCD_Fill(0,0,800,330,BLACK);
	int i,j=0;

//	//�����ϴλ��ƵĲ���
//	POINT_COLOR=BLACK;
//	for(i=0,j=0;i<FFT_N-1 && j<xdisplay_fft*256;i++)
//	{
//		if(	s_last[i].real!=s[i].real || s_last[i+1].real!=s[i+1].real)
//		{
//			LCD_DrawLine(j,(s_last[i].real+100), j+xdisplay_fft,(s_last[i+1].real+100));
//
//		}
//		j=j+xdisplay_fft;
//	}
	//���Ʋ���
	POINT_COLOR=RED;
	LCD_DrawLine(0,display_buttom-10, j+xdisplay_fft+8, (s[0].real+200));
	for(i=0,j=xdisplay_fft+8;i<FFT_N/2-1 && j<800;i++)
	{
		LCD_DrawLine(j,(s[i].real+200), j+xdisplay_fft, (s[i+1].real+200));
		j=j+xdisplay_fft;
	}
	LCD_DrawLine(j,(s[126].real+200), j+xdisplay_fft, display_buttom);

}

/************************************************************
����ԭ�ͣ�void FFT_Top()
�������ܣ�����FFT�任����ʾ����ʹ�÷���
�����������
�����������
************************************************************/
void FFT_Top()
{
  int i;

  for(i=0;i<FFT_N;i++)                        //���ṹ�帳ֵ
  {
     s[i].real=data[i]-128; //ʵ��Ϊ���Ҳ�FFT_N���������ֵΪ1
     s[i].imag=0;                             //�鲿Ϊ0
  }

  FFT(s);                                        //���п��ٸ���Ҷ�任

  for(i=0;i<FFT_N/2;i++)                     //��任������ģֵ�����븴����ʵ������
  {

        if(i==0)							//ֱ�������Ƿ��ֵ/N
        	s[i].real = -2*((sqrt(s[i].real*s[i].real+s[i].imag*s[i].imag))/FFT_N)+128;
        else								//����Ƶ������Ƿ��ֵ/(N/2)
        	s[i].real=-2*(sqrt(s[i].real*s[i].real+s[i].imag*s[i].imag)/(FFT_N/2))+128;

//        if(s[i].real<-100)s[i].real=-100;
//        if(s[i].real>124)s[i].real=120;

//	  	s_last[i].real = s[i].real;			//��ס��һ��FFT����
//	  	s_last[i].imag = s[i].imag;


  }
  Draw_FFT();								//����FFTƵ��


}
/************************************************************
����ԭ�ͣ�void FFT_frame()
�������ܣ�����FFT������
�����������
�����������
************************************************************/
void FFT_frame()
{
	u8 i,j=0;
	POINT_COLOR=WHITE;
	LCD_DrawLine(0,350, 800, 350);

	LCD_DrawLine(0,350, 0, 345);
	LCD_DrawLine(80,350, 80, 345);
	LCD_DrawLine(160,350, 160, 345);
	LCD_DrawLine(240,350, 240, 345);
	LCD_DrawLine(320,350, 320, 345);
	LCD_DrawLine(400,350, 400, 345);
	LCD_DrawLine(480,350, 480, 345);
	LCD_DrawLine(560,350, 560, 345);
	LCD_DrawLine(640,350, 640, 345);
	LCD_DrawLine(720,350, 720, 345);
	LCD_DrawLine(800,350, 800, 345);


}
