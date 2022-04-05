#include "STC15F2K60S2.H"

sbit SEL0=P2^0;			//定义引脚
sbit SEL1=P2^1;
sbit SEL2=P2^2;
sbit SEL3=P2^3;
sbit beep=P3^4;		//蜂鸣器引脚
sbit Key1=P3^2;	   //Key1   切换标志位 

#define uint unsigned int
#define ulint unsigned long
#define uchar unsigned char	
#define NMAX_KEY 100  //100次读取按键值 

uint time=0;				//延时

uint l=0;						//执行光的次数
ulint suml=0;				//光AD值得总和

uint light=0;				//光
uint light_down=20; //警报下限
uint light_up=100;	//警报上限
uchar flag=0;				//切换模式，0代表下限，1代表上限

//设置用于显示光的三个变量
uint light_bai=0;		//百位
uint light_shi=0;		//十位
uint light_ge =0;		//个位
//警报值
uint down_bai=0;		//百位
uint down_shi=0;		//十位
uint down_ge =0;		//个位
//警报值
uint up_bai=0;		//百位
uint up_shi=0;		//十位
uint up_ge =0;		//个位
/**********************

变量定义
***********************/
uchar G_count;
uchar display;
uint Key1_count;
uint Key2_count;
uint Key3_count;
uint Key_count;

bit Key1_C;				//key1当前的状态
bit Key1_P;				//key1前一个状态

//数码管上显示0-F
char segtable[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
								 0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};

void Delay(int n);
void weixuan(char i);
void SEG_Display();
void check();
void InitADC_light();
void date_processlight();
void date_processlight_down();
void date_processlight_up();

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
	//用于设置光的位数
	P0=0;
	weixuan(5);
	P0=segtable[light_bai];
	Delay(10);
	
	P0=0;
	weixuan(6);
	P0=segtable[light_shi];
	Delay(10);
	
	P0=0;
	weixuan(7);
	P0=segtable[light_ge];
	Delay(10);
	
	if(flag==0)    //flag为0 显示下限值 020
	{
		P0=0;
		weixuan(0);
		P0=segtable[down_bai];
		Delay(10);
	
		P0=0;
		weixuan(1);
		P0=segtable[down_shi];
		Delay(10);
	
		P0=0;
		weixuan(2);
		P0=segtable[down_ge];
		Delay(10);
	} 
	if(flag==1)   //flag为1 显示上限值 100
	{
		P0=0;
		weixuan(0);
		P0=segtable[up_bai];
		Delay(10);
	
		P0=0;
		weixuan(1);
		P0=segtable[up_shi];
		Delay(10);
	
		P0=0;
		weixuan(2);
		P0=segtable[up_ge];
		Delay(10);
	}
}

void main()
{
	P1M1=0x00;            //将P1^7设置为推挽模式，其余为准双向口模式
	P1M0=0x80;         
	P0M1=0x00;						//设置P0为推挽模式，点亮数码管
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//将P2^3设置为推挽模式，其余为准双向口模式
	P3M1=0x00;
	P3M0=0x10;					  //设置P3^4为推挽模式
	SEL3=0;								//熄灭LED灯
	
	/*初始化所有按键的当前状态、前一个状态*/
	Key1_C=1;							//key1当前的状态
	Key1_P=1;							//key1前一个状态
	Key1_count=0x80+NMAX_KEY/3*2;
	Key_count=NMAX_KEY;
	
	IE=0xa8;							//EA=1打开总中断，EADC=1允许A/D转化中断，ET1=1允许T1中断	
	TMOD=0x10;						//使用定时器1，16位不可重装载模式，TH1、TL1全用
	TH1=(65535-100)/256;	//高8位赋初值，定时400周期
	TL1=(65535-100)%256;	//低8位赋初值
	TR1=1;								//启动定时器1
	
	InitADC_light();
	while(1)
	{
		SEG_Display();
	}
}

void InitADC_light()		//初始化光ADC
{
	P1ASF=0xff;           //将P1口作为模拟功能A/D使用
	ADC_RES=0;            //转换结果清零
	ADC_RESL=0;
	ADC_CONTR=0x8c;				//ADC_POWER=1打开A/D转换器电源;ADC_START=1启动模拟转换器ADC;CHS=100选择P1^4作为A/D输入使用
	CLK_DIV=0x20;
}

//分别取出温度和光照的百位、十位、个位
void date_processlight_down()
{
	down_bai=light_down%1000/100;
	down_shi=light_down%100/10;
	down_ge =light_down%10;
}
void date_processlight_up()
{
	up_bai=light_up%1000/100;
	up_shi=light_up%100/10;
	up_ge =light_up%10;
}
void date_processlight()
{
	light_bai=light%1000/100;
	light_shi=light%100/10;
	light_ge =light%10;
}

void check()
{
			if(Key1==0)
			Key1_count--;
	
			Key_count--;					//总的次数减1

			if(Key_count==0)			//100次完了
			{
	
				if(Key1_count<0x80)
				{
					Key1_C=0;
					if(Key1_P==1)			//下降沿（按键做动作）
					{
						Key1_P=0;
						flag=!flag;
					}
				}
				if(Key1_count>=0x80)
				{
					Key1_C=1;
					if(Key1_P==0)
					   Key1_P=1;			//上升沿（假设不做动作那就继续）
				}
	
				//新一轮的判断
				Key1_count=0x80+NMAX_KEY/3*2;
				Key_count=NMAX_KEY;
			}
}

void Timer1_Routine() interrupt 3	//3为定时器1中断编号
{
	IE=0x00;												//关闭总中断
	TR1=0;													//定时器1停止
	TH1=(65535-100)/256;						//重新赋值
	TL1=(65535-100)%256;

	check();
	if(flag==0)
	{
		if(light<light_down)
				beep=~beep;				  		 //产生方波使得蜂鸣器发声
		else beep=0;								 //不使用蜂鸣器时，讲引脚置0——处于低电平
	}
	if(flag==1)
	{
		if(light>light_up)
				beep=~beep;				 		  //产生方波使得蜂鸣器发声
		else beep=0;
	}

	IE=0xa8;											//打开总中断
	TR1=1;												//启动定时器1
}

//AD中断
void adc_isr() interrupt 5 using 1
{
  time++;
	IE=0x00;											//关闭中断
	
	if(time>2000)                 //取多次值求平均值减小误差
	{

			light=(suml+l/2)/l;				//四舍五入
			suml=0;
			l=0;
			time=0;
	   	date_processlight();
			date_processlight_down();
			date_processlight_up();
	}

		//处理光部分的数据
		l++;
		suml+=ADC_RES*256+ADC_RESL;//求l次AD值的和

	ADC_CONTR&=~0X10;  					 //转换完成后，ADC_FLAG清零
	ADC_CONTR|=0X08;	 					 //转换完成后，ADC_START赋1
	IE=0xa8;					 					 //打开中断
}