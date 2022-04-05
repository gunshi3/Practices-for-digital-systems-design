#include "STC15F2K60S2.H"

sbit SEL0=P2^0;			//��������
sbit SEL1=P2^1;
sbit SEL2=P2^2;
sbit SEL3=P2^3;

#define uint unsigned int
#define ulint unsigned long

uint time=0;				//��ʱ
uint flag=1;				//��־λ�����ֹ���¶�

uint l=0;						//ִ�й�Ĵ���
uint t=0;						//ִ���¶ȵĴ���
ulint suml=0;				//��ADֵ���ܺ�
ulint sumt=0;				//�¶�ADֵ���ܺ�

uint date_temp;
int temp=0;					//�¶�ֵ
uint abs_temp;			//�¶Ⱦ���ֵ
uint light=0;				//��

//����������ʾ�¶ȵ���������
uint temp_bai=0;		//��λ
uint temp_shi=0;		//ʮλ
uint temp_ge =0;		//��λ

//����������ʾ�����������
uint light_bai=0;		//��λ
uint light_shi=0;		//ʮλ
uint light_ge =0;		//��λ

//���������ʾ0-F
char segtable[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
								 0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
//�¶�ֵ��Ӧ��
int code tempdata[]={239,197,175,160,150,142,135,129,124,120,116,113,109,107,104,101, 
										  99, 97, 95, 93, 91, 90, 88, 86, 85, 84, 82, 81, 80, 78, 77, 76, 
										  75, 74, 73, 72, 71, 70, 69, 68, 67, 67, 66, 65, 64, 63, 63, 62, 
										  61, 61, 60, 59, 58, 58, 57, 57, 56, 55, 55, 54, 54, 53, 52, 52, 
										  51, 51, 50, 50, 49, 49, 48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 
										  43, 43, 42, 42, 41, 41, 41, 40, 40, 39, 39, 38, 38, 38, 37, 37, 
										  36, 36, 36, 35, 35, 34, 34, 34, 33, 33, 32, 32, 32, 31, 31, 31, 
										  30, 30, 29, 29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 26, 25, 25,
										  24, 24, 24, 23, 23, 23, 22, 22, 22, 21, 21, 21, 20, 20, 20, 19, 
										  19, 19, 18, 18, 18, 17, 17, 16, 16, 16, 15, 15, 15, 14, 14, 14, 
										  13, 13, 13, 12, 12, 12, 11, 11, 11, 10, 10, 9, 9, 9, 8, 8, 8, 7, 
										  7, 7, 6, 6,5, 5, 54,4, 3, 3,3, 2, 2, 1, 1, 1, 0, 0, -1, -1, -1, 
										  -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, 
										  -10, -10, -11, -11, -12, -13, -13, -14, -14, -15, -16, -16, -17, 
										  -18, -19, -19, -20, -21, -22, -23, -24, -25, -26, -27, -28, -29, 
										  -30, -32, -33, -35, -36, -38, -40, -43, -46, -50, -55, -63, 361};
void Delay(int n);     //��ʱ����
void weixuan(char i);  //�����λ��ѡ��
void SEG_Display();    //��ʾ�¶ȣ����
void InitADC_temp();   //��ʼ���¶�ADC
void InitADC_light();  //��ʼ����ADC
void date_processtemp();
void date_processlight();

void Delay(int n)			//��ʱ����
{
	int x;
	while(n--)
	{
		x=60;
		while(x--);
	}
}

void weixuan(char i)	//�����λ��ѡ��
{
	SEL2=i/4;
	SEL1=i%4/2;
	SEL0=i%2;
}
void SEG_Display()   //��ʾ�¶ȣ����
{
  //���������¶ȵ�λ��
	if(temp<0)
	{
		P0=0;
		weixuan(0);				//���¶�Ϊ��������λ��ʾ��-��
		P0=0x40;
		Delay(10);
	}
	else
	{
		P0=0;
		weixuan(0);
		P0=segtable[temp_bai];
		Delay(10);
	}
	
	P0=0;
	weixuan(1);
	P0=segtable[temp_shi];
	Delay(10);
	
	P0=0;
	weixuan(2);
	P0=segtable[temp_ge];
	Delay(10);

	//�������ù��λ��
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
}

void T_L()
{
	P0M1=0x00;						//����P0Ϊ����ģʽ�����������
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//��P2^3����Ϊ����ģʽ������Ϊ׼˫���ģʽ
	SEL3=0;								//Ϩ��LED��
	
	IE=0xa8;							//EA=1�����жϣ�EADC=1����A/Dת���жϣ�ET1=1����T1�ж�	
	TMOD=0x10;						//ʹ�ö�ʱ��1��16λ������װ��ģʽ��TH1��TL1ȫ��
	TH1=(65535-40000)/256;//��8λ����ֵ����ʱ40000����
	TL1=(65535-40000)%256;//��8λ����ֵ
	TR1=1;								//������ʱ��1
	
	while(1)
	{
		SEG_Display();
	}
}

void InitADC_temp()			//��ʼ���¶�ADC
{
	P1ASF=0xff;						//��P1����Ϊģ�⹦��A/Dʹ��
	ADC_RES=0;						//�Ĵ���ADC_RES��ADC_RESL����A/Dת�����
	ADC_RESL=0;						//��ʼ��ֵ0
	ADC_CONTR=0x8b;				//ADC_POWER=1��A/Dת������Դ;ADC_START=1����ģ��ת����ADC;CHS=011ѡ��P1^3��ΪA/D����ʹ��
	CLK_DIV=0x20;					//ADRJ=1��ADC_RES[1:0]��Ÿ�2λADC�����ADC_RESL[7:0]��ŵ�8λADC���
}
void InitADC_light()		//��ʼ����ADC
{
	P1ASF=0xff;
	ADC_RES=0;
	ADC_RESL=0;
	ADC_CONTR=0x8c;				//CHS=100ѡ��P1^4��ΪA/D����ʹ��
	CLK_DIV=0x20;
}

//�ֱ�ȡ���¶Ⱥ͹��յİ�λ��ʮλ����λ
void date_processtemp()
{
	if(temp<0)
		abs_temp=-temp;
	else
		abs_temp=temp;
	temp_bai=abs_temp%1000/100;
	temp_shi=abs_temp%100/10;
	temp_ge =abs_temp%10;
}
void date_processlight()
{
	light_bai=light%1000/100;
	light_shi=light%100/10;
	light_ge =light%10;
}

void Timer1_Routine() interrupt 3	//3Ϊ��ʱ��1�жϱ��
{
	IE=0x00;							//�ر����ж�
	TR1=0;								//��ʱ��1ֹͣ
	TH1=(65535-40000)/256;//���¸�ֵ
	TL1=(65535-40000)%256;

	if(flag==1)
	{
		InitADC_light();		//��ʼ����
	}
	else
	{
		InitADC_temp();			//��ʼ���¶�
	}
	flag=-flag;

	IE=0xa8;							//�����ж�
	TR1=1;								//������ʱ��1
}

//AD�ж�
void adc_isr() interrupt 5 using 1
{
  time++;
	IE=0x00;							//�ر��ж�
	
	if(time>2000)      //ȡ���ֵ��ƽ��ֵ��С���
	{
		if(flag==1)										//��ʱflag=1ִ���¶Ȳ���
		{	
			date_temp=(sumt+t/2)/t;			//��������
			temp=tempdata[date_temp-1]; //���Ҷ�Ӧ��AD���¶�ֵ
			sumt=0;
			t=0;
			time=0;
			date_processtemp();	
		}

		if(flag==-1)									//��ʱflag=-1ִ�йⲿ��
		{
			light=(suml+l/2)/l;					//��������
			suml=0;
			l=0;
			time=0;
	   	date_processlight();
		}    
	}

	if(flag==1)
	{
		//��Ӧ�¶ȵ����ݴ���
		t++;
		date_temp=(ADC_RES*256+ADC_RESL)/4;//��10λADֵת��Ϊ8λADֵ
		sumt+=date_temp;						//��t��ADֵ�ĺ�

	}
	if(flag==-1)
	{
		//����ⲿ�ֵ�����
		l++;
		suml+=ADC_RES*256+ADC_RESL;//��l��ADֵ�ĺ�
	}

	ADC_CONTR&=~0X10;  					 //ת����ɺ�ADC_FLAG����
	ADC_CONTR|=0X08;	 					 //ת����ɺ�ADC_START��1
	IE=0xa8;					 					 //���ж�
}

void main()
{
	T_L();
}