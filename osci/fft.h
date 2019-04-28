/*
 * fft.h
 *
 *  Created on: 2017年7月10日
 *      Author: Peter
 */
#include"Osci.h"
#ifndef SRC_OSCI_FFT_H_
#define SRC_OSCI_FFT_H_


/*********************************************************************
                         快速福利叶变换C程序包
函数简介：此程序包是通用的快速傅里叶变换C语言函数，移植性强，以下部分不依
          赖硬件。此程序包采用联合体的形式表示一个复数，输入为自然顺序的复
          数（输入实数是可令复数虚部为0），输出为经过FFT变换的自然顺序的
          复数.此程序包可在初始化时调用create_sin_tab()函数创建正弦函数表，
          以后的可采用查表法计算耗时较多的sin和cos运算，加快可计算速度
使用说明：使用此函数只需更改宏定义FFT_N的值即可实现点数的改变，FFT_N的
          应该为2的N次方，不满足此条件时应在后面补0。若使用查表法计算sin值和
          cos值，应在调用FFT函数前调用create_sin_tab()函数创建正弦表
函数调用：FFT(s);
时    间：2010-2-20
版    本：Ver1.1
参考文献：
**********************************************************************/

#define FFT_N 256                                                  //定义福利叶变换的点数
#define PI 3.1415926535897932384626433832795028841971            //定义圆周率值
#define xdisplay_fft 6
#define display_buttom 332

struct compx {float real,imag;};      //定义一个复数结构
struct compx s[FFT_N];           //FFT输入和输出：从S[0]开始存放，根据大小自己定义
struct compx s_last[FFT_N];       //上一次FFT输入和输出：从S[0]开始存放，根据大小自己定义
float SIN_TAB[FFT_N/2];         //定义正弦表的存放空间



/******************************************************************
函数原型：void create_sin_tab(float *sin_t)
函数功能：创建一个正弦采样表，采样点数与福利叶变换点数相同
输入参数：*sin_t存放正弦表的数组指针
输出参数：无
******************************************************************/
void create_sin_tab(float *sin_t);
/******************************************************************
函数原型：void sin_tab(float pi)
函数功能：采用查表的方法计算一个数的正弦值
输入参数：pi 所要计算正弦值弧度值，范围0--2*PI，不满足时需要转换
输出参数：输入值pi的正弦值
******************************************************************/
float sin_tab(float pi);
/******************************************************************
函数原型：void cos_tab(float pi)
函数功能：采用查表的方法计算一个数的余弦值
输入参数：pi 所要计算余弦值弧度值，范围0--2*PI，不满足时需要转换
输出参数：输入值pi的余弦值
******************************************************************/
float cos_tab(float pi);
/*****************************************************************
函数原型：void FFT(struct compx *xin,int N)
函数功能：对输入的复数组进行快速傅里叶变换（FFT）
输入参数：*xin复数结构体组的首地址指针，struct型
输出参数：无
*****************************************************************/
void FFT(struct compx *xin);
/************************************************************
函数原型：void Init_fftsintable()
函数功能：初始化FFT计算需要的正弦表
输入参数：无
输出参数：无
************************************************************/

void Init_fftsintable();
/************************************************************
函数原型：void Draw_FFT()
函数功能：画出FFT计算出的频谱图
输入参数：无
输出参数：无
************************************************************/

void Draw_FFT();
/************************************************************
函数原型：void FFT_Top()
函数功能：测试FFT变换，演示函数使用方法
输入参数：无
输出参数：无
************************************************************/
void FFT_Top();
/************************************************************
函数原型：void FFT_frame()
函数功能：画出FFT界面框架
输入参数：无
输出参数：无
************************************************************/
void FFT_frame();


#endif /* SRC_OSCI_FFT_H_ */
