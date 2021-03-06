/***********************
基于STC15F2K60S2系列单片机C语言编程实现
使用如下头文件，不用另外再包含"REG51.H"
***********************/
#include <STC15F2K60S2.h>
#define uint unsigned int
uint i=1;														//控制步进电机不同IO口脉冲位置
/**********************
引脚别名定义
***********************/
sbit s1 =P4^1;												    //步进电机IO口
sbit s2 =P4^2;
sbit s3 =P4^3;
sbit s4 =P4^4;
//sbit LED=P2^3;													//LED与数码管切换
/**********************
函数名称：void time0()
功能描述：定时器中断,控制脉冲转换
入口参数：无
出口参数：无
***********************/
void time0() interrupt 1
{
	 switch(i++)												//控制步进电机不同IO口脉冲电平
	 {
	 case 1: s1=1;s2=0;s3=0;s4=0;break;
	 case 2: s1=0;s2=1;s3=0;s4=0;break;
	 case 3: s1=0;s2=0;s3=1;s4=0;break;
	 case 4: s1=0;s2=0;s3=0;s4=1;break;
	 }
	 if(i==5)
	 i=1;
}

/**********************
函数名称：init_sys()
功能描述：系统初始化，功能是配置IO口
入口参数：无
出口参数：无
***********************/
void init_sys()			  
{																//P4口推挽输出
    P4M0=0Xff;
	P4M1=0X00;
//	LED =0;														//设置为数码管显示
	P4  =0X00;												    //P4口设置低电平，避免复位时对步进电机5V电平接口的影响

	TMOD = 0x00;		                                        //设置定时器0 16位自动重装模式
	TH0  = (65536-10000)/256;	                                //设置定时5ms
	TL0  = (65536-10000)%256;
	TCON = 0X10;		                                        //定时器0开始计时
	IE   = 0x82;				                                //开启CPU中断，开启定时器0中断
}
/**********************
函数名称：main()
功能描述：调用函数初始化
入口参数：无
出口参数：无
***********************/
void main()
{
	init_sys();
	while(1)
	{
	}
}