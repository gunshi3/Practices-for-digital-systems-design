#include "STC15F2K60S2.H"

sbit SEL0=P2^0;				//定义引脚
sbit SEL1=P2^1;
sbit SEL2=P2^2;
sbit SEL3=P2^3;

#define uint unsigned int
#define ulint unsigned long

uint time=0;					//延时
uint flag=1;					//标志位，区分P1_0和P1_1

uint count_0=0;				//执行P1_0的次数
uint count_1=0;				//执行P1_1的次数
ulint sum_0=0;				//P1_0接口AD值得总和
ulint sum_1=0;				//P1_1接口AD值得总和

uint P1_0=0;					//P1_0
uint P1_1=0;					//P1_1 

//设置用于显示P1_0电压的三个变量(电压范围在0~5之间，显示小数点后两位)
uint U0_bai=0;				//百位(电压个位整数位)
uint U0_shi=0;				//十位(小数点后十分位)
uint U0_ge =0;				//个位(小数点后百分位)

//设置用于显示P1_1电压U1的三个变量
uint U1_bai=0;				//百位
uint U1_shi=0;				//十位
uint U1_ge =0;				//个位

//定义中间转换变量
float f0=0.0,f1=0.0;
int i0=0,i1=0;

//数码管上显示0-F
char segtable[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
								 0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
//显示小数点的数组(0~5V)
char segtabletwo[]={0xbf,0x86,0xdb,0xcf,0xe6,0xed};

void Delay(int n);
void weixuan(char i);
void SEG_Display();
void InitADC_P1_0();
void InitADC_P1_1();
void date_processP1_0();
void date_processP1_0();

void Delay(int n)			//延时函数
{
	int x;
	while(n--)
	{
		x=60;
		while(x--);
	}
}

void weixuan(char i)	//数码管位的选择
{
	SEL2=i/4;
	SEL1=i%4/2;
	SEL0=i%2;
}
void SEG_Display()
{
  //用于显示P1_0电压值
	P0=0;
	weixuan(0);
	P0=segtabletwo[U0_bai];
	Delay(10);

	P0=0;
	weixuan(1);
	P0=segtable[U0_shi];
	Delay(10);
	
	P0=0;
	weixuan(2);
	P0=segtable[U0_ge];
	Delay(10);

  //用于显示P1_1电压值
	P0=0;
	weixuan(5);
	P0=segtabletwo[U1_bai];
	Delay(10);
	
	P0=0;
	weixuan(6);
	P0=segtable[U1_shi];
	Delay(10);
	
	P0=0;
	weixuan(7);
	P0=segtable[U1_ge];
	Delay(10);
}

/**********************
函数名称：void U_T()
功能描述：用定时器1使得AD分时采集P1.0和P1.1口的AD值，并显示在数码管上，左侧显示P1.0，右侧显示P1.1
入口参数：无
出口参数：无
备注：
***********************/

void U0_U1()
{
	P0M1=0x00;						//设置P0为推挽模式，点亮数码管
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//将P2^3设置为推挽模式，其余为准双向口模式
	SEL3=0;								//熄灭LED灯
	
	IE=0xa8;							//EA=1打开总中断，EADC=1允许A/D转化中断，ET1=1允许T1中断	
	TMOD=0x10;						//使用定时器1，16位不可重装载模式，TH1、TL1全用
	TH1=(65535-40000)/256;//高8位赋初值，定时40000周期
	TL1=(65535-40000)%256;//低8位赋初值
	TR1=1;								//启动定时器1
	
	while(1)
	{
		SEG_Display();
	}
}

void InitADC_P1_0()			//初始化P1_0
{
	P1ASF=0xff;						//将P1口作为模拟功能A/D使用
	ADC_RES=0;						//寄存器ADC_RES和ADC_RESL保存A/D转化结果
	ADC_RESL=0;						//初始赋值0
	ADC_CONTR=0x88;				//ADC_POWER=1打开A/D转换器电源;ADC_START=1启动模拟转换器ADC;CHS=000选择P1^0作为A/D输入使用
	CLK_DIV=0x20;					//ADRJ=1：ADC_RES[1:0]存放高2位ADC结果，ADC_RESL[7:0]存放低8位ADC结果
}
void InitADC_P1_1()			//初始化P1_1
{
	P1ASF=0xff;
	ADC_RES=0;
	ADC_RESL=0;
	ADC_CONTR=0x89;				//CHS=001选择P1^1作为A/D输入使用
	CLK_DIV=0x20;
}

//分别取出U0和U1的电压值(保留两位小数)
void date_processP1_0()
{
			i0=f0;
			U0_bai=i0;

			i0=f0*10;
			U0_shi=i0%10;

			i0=f0*100;
			U0_ge=i0%10;
}
void date_processP1_1()
{
			i1=f1;
			U1_bai=i1;

			i1=f1*10;
			U1_shi=i1%10;

			i1=f1*100;
			U1_ge=i1%10;
}

void Timer1_Routine() interrupt 3	//3为定时器1中断编号
{
	IE=0x00;							//关闭总中断
	TR1=0;								//定时器1停止
	TH1=(65535-40000)/256;//重新赋值
	TL1=(65535-40000)%256;

	if(flag==1)
	{
		InitADC_P1_1();			//初始化P1_1
	}
	else
	{
		InitADC_P1_0();			//初始化P1_0
	}
	flag=-flag;

	IE=0xa8;							//打开总中断
	TR1=1;								//启动定时器1
}

//AD中断
void adc_isr() interrupt 5 using 1
{
  time++;
	IE=0x00;							//关闭中断
	
	if(time>2000)
	{
		if(flag==1)												 //此时flag=1执行温度部分
		{	
			P1_0=(sum_0+count_0/2)/count_0;	 //四舍五入
			f0=(5*P1_0)/1024.0;							 //转换成电压
			sum_0=0;
			count_0=0;
			time=0;
			date_processP1_0();	
		}


		if(flag==-1)												//此时flag=-1执行光部分
		{
			P1_1=(sum_1+count_1/2)/count_1;		//四舍五入
			f1=(5*P1_1)/1024.0;		 						//转换成电压
			sum_1=0;
			count_1=0;
			time=0;
	   	date_processP1_1();
		}    
	}

	if(flag==1)
	{
		//对应P1_0处理
		count_0++;
		sum_0+=ADC_RES*256+ADC_RESL;					//求count_0次AD值的和

	}
	if(flag==-1)
	{
		//对应P1_1处理
		count_1++;
		sum_1+=ADC_RES*256+ADC_RESL;					//求count_1次AD值的和
	}

	ADC_CONTR&=~0X10;  					 //转换完成后，ADC_FLAG清零
	ADC_CONTR|=0X08;	 					 //转换完成后，ADC_START赋1
	IE=0xa8;					 					 //打开中断
}

void main()
{
	U0_U1();
}