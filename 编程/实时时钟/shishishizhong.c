#include"STC15F2K60S2.H" //ͷ�ļ�
#include"intrins.H" //ͷ�ļ�
#include"ctype.h"
//�궨��
#define uchar unsigned char 	   
#define uint unsigned int

//DS1302�Ĵ����Ķ���
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

//λ����
sbit RTC_sclk=P1^5;//ʱ�ӿ������ţ��������ݵ��������
sbit RTC_rst=P1^6;//CE���ţ���дʱ�����øߵ�ƽ
sbit RTC_io=P5^4;//��������

//��ʾ��λ����
sbit led_sel=P2^3;
uchar wei[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};	  //�����λѡ
uchar duan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71}; //��ʾ0-f

//����ʱ��ṹ��
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

//DS1302дһ���ֽڵ�����
void DS1302WriteByte(uchar dat)
{
	uchar i;
  RTC_sclk=0;//��ʼʱ������0
	_nop_();
	_nop_();
	for(i=0;i<8;i++)//��ʼ����8λ����
	{
		RTC_io=dat&0x01;//ȡ���λ
		_nop_();
	  _nop_();
		RTC_sclk=1;//ʱ�������ߣ����������أ����ݱ����� 
		_nop_();
	  _nop_();
		RTC_sclk=0;//ʱ�������ͣ�Ϊ��һ����������׼��
		dat>>=1;//��������һλ��׼��������һλ����
	}
}

////DS1302��һ���ֽڵ�����
uchar DS1302ReadByte()
{
	uchar i,dat;
		_nop_();
	  _nop_();
	for(i=0;i<8;i++)
	{
		dat>>=1;//Ҫ���ص���������һλ
		if(RTC_io==1)//��������Ϊ��ʱ��֤����λ����Ϊ1
		dat|=0x80;
		RTC_sclk=1;
		_nop_();
	  _nop_();
		RTC_sclk=0;//�����½���
		_nop_();
	  _nop_();
	}
	return dat;//���ض�ȡ��������
}

//����Ӧ��ַ��дһ���ֽڵ�����
uchar DS1302Read(uchar cmd)
{
	uchar dat;
	RTC_rst=0;//��ʼCE��0
	RTC_sclk=0;//��ʼʱ����0
	RTC_rst=1;//��ʼCE��1�����俪ʼ
	DS1302WriteByte(cmd);//���������֣�
	dat=DS1302ReadByte();//��ȡ�õ���ʱ��
	RTC_sclk=1;//ʱ��������
	RTC_rst=0;//��ȡ������CE��0���������ݴ���
	return dat;//���صõ���ʱ������
}

//����Ӧ��ַ��д����
void DS1302Write(uchar cmd,uchar dat)
{
	RTC_rst=0;//��ʼCE��0
	RTC_sclk=0;//��ʼʱ����0
	RTC_rst=1;//��1�����俪ʼ
	DS1302WriteByte(cmd);//���������֣�Ҫд���ʱ��ĵ�ַ
	DS1302WriteByte(dat);//д���޸ĵ�ʱ��
	RTC_sclk=1;//ʱ��������
	RTC_rst=0;//��ȡ������CE=0���������ݵĴ���
}
//DS1302��ʱ��ֵ��ȡ����
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
//DS1302��ʼ������
void Init_DS1302()
{  	unsigned char hour,min,sec;
//    unsigned char code DataStr[]=__DATE__;	  //��ʽ: "Jan 13 2017"   12�ַ�������������
    unsigned char code DataStr[]=__TIME__;	  //��ʽ��"09:12:04"	  9�ַ�������������

    hour=((toint(DataStr[0]))<<4)+toint(DataStr[1]);
	min=((toint(DataStr[3]))<<4)+toint(DataStr[4]);
	sec=((toint(DataStr[6]))<<4)+toint(DataStr[7]);

	DS1302Write(0X8E,0X00);//д������
	DS1302Write(DS1302_second_write,sec);
	DS1302Write(DS1302_minutes_write,min);
	DS1302Write(DS1302_hour_write,hour);
//    DS1302Write(DS1302_week_write,0X00);
//	DS1302Write(DS1302_month_write,0X00);
//	DS1302Write(DS1302_date_write,0X00);
//	DS1302Write(DS1302_year_write,0X00);
	temp=DS1302Read(DS1302_second_read)&0x7f;
	DS1302Write(DS1302_second_write,temp);
//	DS1302Write(0x90,0xa9);         //  ������ã������磬2�������ܣ�2K��	
	DS1302Write(0X8E,0X80);//д������1
}

 void set_charge_DS1302()
{ 
	DS1302Write(0X8E,0X00); //д������
	DS1302Write(0x90,0xa9); //  ������ã������磬2�������ܣ�2K��	
	DS1302Write(0X8E,0X80); //д������1
}

//ϵͳ��ʼ������
void init()
{ 	P3=0xEF;				  //�ط�����
	P2M0=0XFF;
	P2M1=0X00;
	P0M0=0XFF;
	P0M1=0X00;

	led_sel=0;//ѡͨ�����
	TMOD=0X01;//��ʱ��0��������ʽ1
	EA=1;//�����ж�
	TH0=(65535-1000)/256;//���ö�ʱ��ֵ
	TL0=(65535-1000)%256;
	TR0=1;//������ʱ��
	ET0=1;//������ʱ���ж�
}

void time0() interrupt 1
{
	TH0=(65535-1000)/256;//���ö�ʱ��ֵ
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