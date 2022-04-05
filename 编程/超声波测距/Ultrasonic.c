#include "STC15F2K60S2.H" 

/**************************
引脚别名定义如下：
**************************/
sbit SEL0=P2^0;
sbit SEL1=P2^1;
sbit SEL2=P2^2;
sbit SEL3=P2^3;
sbit Echo=P1^0;						//发送
sbit Trig=P1^1;						//接收

/**************************
定义变量如下：
**************************/
#define uint unsigned int
float TIME;								//一个周期的时间
float receive;
uint receiveint; 					//距离
char qianwei=0;
char baiwei=0;
char shiwei=0;
char gewei=0;
char global=0;

char segtable[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x71};

void SEG_Display();
void date_process();
void Delay(int n);

/**********************
函数名称：void ultr()
功能描述：利用超声波模块HC-SR04把测得的距离显示在数码管上
***********************/
void main()
{
	TIME=12/11.0592;						//寄存器AUXR中T1x12=0,12分频

	P0M1=0x00;									//推挽
  P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;	
	SEL3=0;

	TMOD=0x11;									//同时使用定时器T0和T1

  EA=1;												//打开总的中断
	ET1=1;											//开启定时器1中断	
 	TH1=(65535-60000)/256;			//60ms
	TL1=(65535-60000)%256;		
	TR1=1;											//启动定时器

	while(1)
	{
	 SEG_Display();
	}
}

//定义定时器1中断，用于每隔至少60ms发一次信号
void Timer1_Routine() interrupt 3
{
	global++;
	if(global>10)
	{
		ET1=0;										 //关闭T1中断
		TR1=0;										 //关闭定时器1
		TH1=(65535-60000)/256;
		TL1=(65535-60000)%256;		//重新赋值
		
		Trig=1;				
		Delay(2);									//生成20us的脉冲宽度的触发信号
		Trig=0;

		while(!Echo);							//等待回响信号变高电平
		TR0=1;										//启动定时器0
		
		while(Echo);							//等待回响信号变低电平
		TR0=0;										//关闭定时器0
		
		//通过晶振频率，传播时间和声音传播速度可以测得距离
		receive=(340*TIME*((float)TH0*256+(float)TL0))/2000;
		receiveint=(int)receive;
		if(receiveint<30||receiveint>4000)
		{	
			receiveint=0;
		}
		date_process();
	
		TH0=0;										//定时器0赋值0
		TL0=0;

		ET1=1;										//打开定时器T1
		TR1=1;
		global=0;
	}
}

void date_process()
{
	qianwei=receiveint/1000;
	baiwei=receiveint%1000/100;
	shiwei=receiveint%100/10;
	gewei=receiveint%10;	
}
void weixuan(char i)
{
	SEL2=i/4;
	SEL1=i%4/2;
	SEL0=i%2;
}
void SEG_Display()
{
	P0=0;
	weixuan(4);
	P0=segtable[qianwei];
	Delay(30);
	P0=0;
	weixuan(5);
	P0=segtable[baiwei];
	Delay(30);
	P0=0;
	weixuan(6);
	P0=segtable[shiwei];
	Delay(30);
	P0=0;
	weixuan(7);
	P0=segtable[gewei];
	Delay(30);
	
}

void Delay(int n)
{
	int x;
	while(n--)
	{
		x=50;
		while(x--);
	}
}