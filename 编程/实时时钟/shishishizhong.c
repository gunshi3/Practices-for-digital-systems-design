#include"STC15F2K60S2.H" //头文件
#include"intrins.H" //头文件
#include"ctype.h"
//宏定义
#define uchar unsigned char 	   
#define uint unsigned int

//DS1302寄存器的定义
#define DS1302_second_write  0X80
#define DS1302_minutes_write  0X82
#define DS1302_hour_write  0X84
#define DS1302_date_write  0X86
#define DS1302_week_write  0X8A
#define DS1302_month_write  0X88
#define DS1302_year_write  0X8C

#define DS1302_second_read  0X81
#define DS1302_minutes_read  0X83
#define DS1302_hour_read  0X85
#define DS1302_date_read  0X87
#define DS1302_week_read  0X8B
#define DS1302_month_read  0X89
#define DS1302_year_read  0X8D

//位定义
sbit RTC_sclk=P1^5;//时钟控制引脚，控制数据的输入输出
sbit RTC_rst=P1^6;//CE引脚，读写时必须置高电平
sbit RTC_io=P5^4;//数据引脚

//显示的位定义
sbit led_sel=P2^3;
uchar wei[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};	  //数码管位选
uchar duan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71}; //显示0-f

//定义时间结构体
typedef struct _systemtime_
{
   uchar second;
	 uchar minute;
	 uchar hour;
	 uchar day;
	 uchar week;
	 uchar  month;
   uchar year;
}systemtime;

systemtime t;
	uchar i;
uchar temp;
int sec=0;
char flag_100mS=0;

//DS1302写一个字节的数据
void DS1302WriteByte(uchar dat)
{
	uchar i;
  RTC_sclk=0;//初始时钟线置0
	_nop_();
	_nop_();
	for(i=0;i<8;i++)//开始传输8位数据
	{
		RTC_io=dat&0x01;//取最低位
		_nop_();
	  _nop_();
		RTC_sclk=1;//时钟线拉高，制造上升沿，数据被传输 
		_nop_();
	  _nop_();
		RTC_sclk=0;//时钟线拉低，为下一个上升沿做准备
		dat>>=1;//数据右移一位，准备传输下一位数据
	}
}

////DS1302读一个字节的数据
uchar DS1302ReadByte()
{
	uchar i,dat;
		_nop_();
	  _nop_();
	for(i=0;i<8;i++)
	{
		dat>>=1;//要返回的数据右移一位
		if(RTC_io==1)//当数据线为高时，证明该位数据为1
		dat|=0x80;
		RTC_sclk=1;
		_nop_();
	  _nop_();
		RTC_sclk=0;//制造下降沿
		_nop_();
	  _nop_();
	}
	return dat;//返回读取出的数据
}

//读相应地址中写一个字节的数据
uchar DS1302Read(uchar cmd)
{
	uchar dat;
	RTC_rst=0;//初始CE置0
	RTC_sclk=0;//初始时钟置0
	RTC_rst=1;//初始CE置1，传输开始
	DS1302WriteByte(cmd);//传输命令字，
	dat=DS1302ReadByte();//读取得到的时间
	RTC_sclk=1;//时钟线拉高
	RTC_rst=0;//读取结束，CE置0，结束数据传输
	return dat;//返回得到的时间日期
}

//在相应地址中写数据
void DS1302Write(uchar cmd,uchar dat)
{
	RTC_rst=0;//初始CE置0
	RTC_sclk=0;//初始时钟置0
	RTC_rst=1;//置1，传输开始
	DS1302WriteByte(cmd);//传输命令字，要写入的时间的地址
	DS1302WriteByte(dat);//写入修改的时间
	RTC_sclk=1;//时钟线拉高
	RTC_rst=0;//读取结束，CE=0，结束数据的传输
}
//DS1302的时间值获取程序
systemtime GetDA1302()
{
	systemtime time;
	uchar realvalue;
	realvalue=DS1302Read(DS1302_second_read);
	time.second=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_minutes_read);
	time.minute=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_hour_read );
	time.hour=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_date_read );
	time.day=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_week_read);
	time.week=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
		realvalue=DS1302Read(DS1302_month_read);
	time.month=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
			realvalue=DS1302Read(DS1302_year_read);
	time.year=((realvalue&0x70)>>4)*10+(realvalue&0x0f);
	return time;
}
//DS1302初始化程序
void Init_DS1302()
{  	unsigned char hour,min,sec;
//    unsigned char code DataStr[]=__DATE__;	  //格式: "Jan 13 2017"   12字符（含结束符）
    unsigned char code DataStr[]=__TIME__;	  //格式："09:12:04"	  9字符（含结束符）

    hour=((toint(DataStr[0]))<<4)+toint(DataStr[1]);
	min=((toint(DataStr[3]))<<4)+toint(DataStr[4]);
	sec=((toint(DataStr[6]))<<4)+toint(DataStr[7]);

	DS1302Write(0X8E,0X00);//写保护关
	DS1302Write(DS1302_second_write,sec);
	DS1302Write(DS1302_minutes_write,min);
	DS1302Write(DS1302_hour_write,hour);
//    DS1302Write(DS1302_week_write,0X00);
//	DS1302Write(DS1302_month_write,0X00);
//	DS1302Write(DS1302_date_write,0X00);
//	DS1302Write(DS1302_year_write,0X00);
	temp=DS1302Read(DS1302_second_read)&0x7f;
	DS1302Write(DS1302_second_write,temp);
//	DS1302Write(0x90,0xa9);         //  充电设置：允许充电，2个二极管，2K电	
	DS1302Write(0X8E,0X80);//写保护置1
}

 void set_charge_DS1302()
{ 
	DS1302Write(0X8E,0X00); //写保护关
	DS1302Write(0x90,0xa9); //  充电设置：允许充电，2个二极管，2K电	
	DS1302Write(0X8E,0X80); //写保护置1
}

//系统初始化程序
void init()
{ 	P3=0xEF;				  //关蜂鸣器
	P2M0=0XFF;
	P2M1=0X00;
	P0M0=0XFF;
	P0M1=0X00;

	led_sel=0;//选通数码管
	TMOD=0X01;//定时器0，工作方式1
	EA=1;//打开总中断
	TH0=(65535-1000)/256;//设置定时初值
	TL0=(65535-1000)%256;
	TR0=1;//启动定时器
	ET0=1;//开启定时器中断
}

void time0() interrupt 1
{
	TH0=(65535-1000)/256;//设置定时初值
	TL0=(65535-1000)%256;
	EA=0;
	i++;
	if(++sec==100) {sec=0; flag_100mS=1; }
	if(i==8)
		i=0;
	led_sel=0;
	P0=0X00;
	P2=wei[i];
	
	switch(i)
	{
		case 0:P0=duan[t.hour/10];break;
		case 1:P0=duan[t.hour%10];break;
			case 3:P0=duan[t.minute/10];break;
		case 4:P0=duan[t.minute%10];break;
			case 6:P0=duan[t.second/10];break;
		case 7:P0=duan[t.second%10];break;
		default :P0=0x40;break;
	}
	EA=1;
}

void main()
{
	init();
	if(DS1302Read(DS1302_second_read)&0X80)	Init_DS1302();
	set_charge_DS1302();
	while(1)
	{
		if (flag_100mS==1)
		{
		 t=GetDA1302();
		 flag_100mS=0;
		}
	}
}