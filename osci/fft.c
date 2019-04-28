/*
 * fft.c
 *
 *  Created on: 2017年7月10日
 *      Author: Peter
 */
#include "fft.h"

/*基二FFT算法*/
#include "math.h"
#include "stdio.h"
#include "../tft/lcd.h"
#include<math.h>

/*******************************************************************
函数原型：struct compx EE(struct compx b1,struct compx b2)
函数功能：对两个复数进行乘法运算
输入参数：两个以联合体定义的复数a,b
输出参数：a和b的乘积，以联合体的形式输出
*******************************************************************/
struct compx EE(struct compx a,struct compx b)
{
 struct compx c;
 c.real=a.real*b.real-a.imag*b.imag;
 c.imag=a.real*b.imag+a.imag*b.real;
 return(c);
}


/******************************************************************
函数原型：void create_sin_tab(float *sin_t)
函数功能：创建一个正弦采样表，采样点数与福利叶变换点数相同
输入参数：*sin_t存放正弦表的数组指针
输出参数：无
******************************************************************/
void create_sin_tab(float *sin_t)
{
  int i;
  for(i=0;i<FFT_N/2;i++)
  sin_t[i]=sin(2*PI*i/FFT_N);
}
/******************************************************************
函数原型：void sin_tab(float pi)
函数功能：采用查表的方法计算一个数的正弦值
输入参数：pi 所要计算正弦值弧度值，范围0--2*PI，不满足时需要转换
输出参数：输入值pi的正弦值
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
函数原型：void cos_tab(float pi)
函数功能：采用查表的方法计算一个数的余弦值
输入参数：pi 所要计算余弦值弧度值，范围0--2*PI，不满足时需要转换
输出参数：输入值pi的余弦值
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
函数原型：void FFT(struct compx *xin,int N)
函数功能：对输入的复数组进行快速傅里叶变换（FFT）
输入参数：*xin复数结构体组的首地址指针，struct型
输出参数：无
*****************************************************************/
void FFT(struct compx *xin)
{
  int f,m,nv2,nm1,i,k,l,j=0;
  struct compx u,w,t;

   nv2=FFT_N/2;             //变址运算，即把自然顺序变成倒位序，采用雷德算法
   nm1=FFT_N-1;
   for(i=0;i<nm1;i++)
   {
    if(i<j)                    //如果i<j,即进行变址
     {
      t=xin[j];
      xin[j]=xin[i];
      xin[i]=t;
     }
    k=nv2;                    //求j的下一个倒位序
    while(k<=j)               //如果k<=j,表示j的最高位为1
     {
      j=j-k;                 //把最高位变成0
      k=k/2;                 //k/2，比较次高位，依次类推，逐个比较，直到某个位为0
     }
   j=j+k;                   //把0改为1
  }

  {
   int le,lei,ip;                            //FFT运算核，使用蝶形运算完成FFT运算
    f=FFT_N;
   for(l=1;(f=f/2)!=1;l++)              //计算l的值，即计算蝶形级数
           ;
  for(m=1;m<=l;m++)                   // 控制蝶形结级数
   {                               //m表示第m级蝶形，l为蝶形级总数l=log（2）N
    le=2<<(m-1);                    //le蝶形结距离，即第m级蝶形的蝶形结相距le点
    lei=le/2;                         //同一蝶形结中参加运算的两点的距离
    u.real=1.0;                        //u为蝶形结运算系数，初始值为1
    u.imag=0.0;
    //w.real=cos(PI/lei);                  //不适用查表法计算sin值和cos值
    // w.imag=-sin(PI/lei);
    w.real=cos_tab(PI/lei);             //w为系数商，即当前系数与前一个系数的商
    w.imag=-sin_tab(PI/lei);
    for(j=0;j<=lei-1;j++)              //控制计算不同种蝶形结，即计算系数不同的蝶形结
     {
      for(i=j;i<=FFT_N-1;i=i+le)       //控制同一蝶形结运算，即计算系数相同蝶形结
       {
        ip=i+lei;                          //i，ip分别表示参加蝶形运算的两个节点
        t=EE(xin[ip],u);                   //蝶形运算，详见公式
        xin[ip].real=xin[i].real-t.real;
        xin[ip].imag=xin[i].imag-t.imag;
        xin[i].real=xin[i].real+t.real;
        xin[i].imag=xin[i].imag+t.imag;
       }
      u=EE(u,w);                          //改变系数，进行下一个蝶形运算
     }
   }
  }

}
/************************************************************
函数原型：void Init_fftsintable()
函数功能：初始化FFT计算需要的正弦表
输入参数：无
输出参数：无
************************************************************/

void Init_fftsintable()
{
	create_sin_tab(SIN_TAB);
	POINT_COLOR=WHITE;
	FFT_frame();
}
/************************************************************
函数原型：void Draw_FFT()
函数功能：画出FFT计算出的频谱图
输入参数：无
输出参数：无
************************************************************/

void Draw_FFT()
{
 	LCD_Fill(0,0,800,330,BLACK);
	int i,j=0;

//	//擦除上次绘制的波形
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
	//绘制波形
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
函数原型：void FFT_Top()
函数功能：测试FFT变换，演示函数使用方法
输入参数：无
输出参数：无
************************************************************/
void FFT_Top()
{
  int i;

  for(i=0;i<FFT_N;i++)                        //给结构体赋值
  {
     s[i].real=data[i]-128; //实部为正弦波FFT_N点采样，赋值为1
     s[i].imag=0;                             //虚部为0
  }

  FFT(s);                                        //进行快速福利叶变换

  for(i=0;i<FFT_N/2;i++)                     //求变换后结果的模值，存入复数的实部部分
  {

        if(i==0)							//直流计算是峰峰值/N
        	s[i].real = -2*((sqrt(s[i].real*s[i].real+s[i].imag*s[i].imag))/FFT_N)+128;
        else								//交流频点计算是峰峰值/(N/2)
        	s[i].real=-2*(sqrt(s[i].real*s[i].real+s[i].imag*s[i].imag)/(FFT_N/2))+128;

//        if(s[i].real<-100)s[i].real=-100;
//        if(s[i].real>124)s[i].real=120;

//	  	s_last[i].real = s[i].real;			//记住上一次FFT数据
//	  	s_last[i].imag = s[i].imag;


  }
  Draw_FFT();								//画出FFT频谱


}
/************************************************************
函数原型：void FFT_frame()
函数功能：画出FFT界面框架
输入参数：无
输出参数：无
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
