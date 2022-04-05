#include "STC15F2K60S2.H"   
#define uchar unsigned char
#define uint  unsigned int
#define X 1.05946

sbit beep=P3^4;						
sbit P3_3=P3^3;
sbit P3_2=P3^2;
uint y=956;
uint i=1;
uchar duanxuan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};		//段选 0-f


void delay(uint t)		                   //延时函数  延时tms
{
	uint j;  
	for(;t>0;t--)   
    	for(j=800;j>0;j--) 
		;								
}

void int0() interrupt 0			          
{
    EX0=0;								  //关闭外部中断0
	delay(10);							  //消抖
	if(P3_2==0)
	{									  //当按下k1时，改变段选与定时器0的初始值从而改变输入到蜂鸣器的方波频率
	    if(i==7)	   					  //循环输出音调1-7
		{y=956;
		 P0=duanxuan[1];
		 i=1;
		 }
		else if(i==3)					
		{y=y/X;i++;P0=duanxuan[i];}
		else
		{y=y/X/X;i++;P0=duanxuan[i];}
	}
	while(!P3_2);						 //中断检测扫描键盘
	EX0=1;								 //开启外部中断0
}

void int1() interrupt 2							
{
	EX1=0;							   //关闭外部中断1
    delay(10);						   //消抖
	if(P3_3==0)
	{
		if(TR0)
			{TR0=0;beep=0;}			  //若定时器0开启则关闭
		else
			{TR0=1;}				  //若定时器0关闭则开启
	}
	while(!P3_3);					  //中断检测扫描键盘
	EX1=1;							  //开启外部中断1
}

void timer0() interrupt 1
{
	beep=~beep;
	TH0=(65536-y)/256;				  //音调不同，定时器初始值不同
	TL0=(65536-y)%256;
} 

main()
{
	P3M0=0x10;		                  //推挽输出
	P3M1=0x00;
	P2M0=0x0f;
	P2M1=0x00;
	P1M0=0xff;
	P1M1=0x00;
	TMOD=0x01;						  //定时器0位1方式工作
	TH0=(65536-y)/256;
	TL0=(65536-y)%256;
	EA=1;                             //中断总开关
	ET0=1;							  //定时器0开启
	TR0=1;
	EX1=1;
	IT1=1;							  //外部中断开启
	EX0=1;
	IT0=1;
	IP=0x05;
	beep=0;
	P2=0x07;						  //位选设为最右边一位
	P0=duanxuan[1];					  
	while(1)
	{
	}
}