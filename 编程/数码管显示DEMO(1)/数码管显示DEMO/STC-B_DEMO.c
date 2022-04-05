#include "STC15F2K60S2.H" 

#define Dis_B 0
#define Dis_E 8

unsigned char Dis_flag=0;     //��¼��ʾλ
unsigned char count_1mS=0;
unsigned char count_10mS=0;
           
bit flag_1mS=0;           //=1     1mSʱ�̱��
bit flag_10mS=0;
bit flag_100mS=0;

char disp[9];          //8  ����  ��Ӧ��ˮ�ƣ�  >8  ����
//
//void GPIO_Init();
//void Timer0_Init();
void Func_1mS();	//Function for 1ms
void Func_10mS();
void Func_100mS();
//void Display(char, char);
//void LED_print(char,char,char,char,char,char,char,char);
//void L_print(char);

void Display(char Dis_min, char Dis_max) 
{ char selData[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40,0x00}; //  �������ʾ����� 
	      /* ���:   0  1    2	  3    4	 5    6	   7   8	 9	 10	  11		    */
	      /* ��ʾ:   0  1    2    3    4   5    6    7   8   9   -   (��)      */ 
 // while(Dis_flag<8 && disp[Dis_flag] == 11) ++Dis_flag; //���L7����L0��������ʾ��������
  P0 =0;                                                  //�رն�ѡ����Ӱ
  P2 = (P2 & 0xf0) | (Dis_flag & 0x0f);                   //λѡ����:����P2��4λ���䣬��4λȡDis_flag
  if   (Dis_flag<9)
    {if(Dis_flag == 8) P0 = disp[Dis_flag];                  //��ѡ��LED��ʱ
		else           P0 = selData[disp[Dis_flag]]; 			 //��ѡ�������ʱ
	}       
  if( ++Dis_flag > Dis_max ) Dis_flag = Dis_min;
}

void LED_print(char d0,char d1,char d2,char d3,char d4,char d5,char d6,char d7)
{
	disp[0]=d0;
	disp[1]=d1;
	disp[2]=d2;
	disp[3]=d3;
	disp[4]=d4;
	disp[5]=d5;
	disp[6]=d6;
	disp[7]=d7;
}
//void L_print(char d)
//{
//	disp[8]=d;
//}

void GPIO_Init(void)
{
	P0M1=0x00;						//����P0Ϊ����ģʽ�����������
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//��P2^3����Ϊ����ģʽ������Ϊ׼˫���ģʽ
}

void Timer0_Init(void)             //��Ƶѡ��@12.000MHz 
{	
	AUXR &= 0x7f;	          //12Tģʽ
	TMOD &= 0xf0;						//ʹ�ö�ʱ��0��16λ�Զ���װ��ģʽ��TH0��TL0ȫ��
	TH0=(65535-999)/256; //��8λ����ֵ����ʱ1000����
	TL0=(65535-999)%256; //��8λ����ֵ
	TR0=1;								//������ʱ��0	
	ET0=1;   //������ʱ��0�ж�
}

void Timer0_isr() interrupt 1
{
	flag_1mS = 1;
	count_1mS++;
	if( count_1mS == 10 )  { count_1mS = 0;    flag_10mS = 1;  count_10mS++; }
    if( count_10mS == 10 )  { count_10mS = 0;    flag_100mS = 1; }
}

void Func_1mS()
{
  Display(Dis_B,Dis_E);	
}

void Func_10ms()
{
   
}

void Func_100ms()
{

}

void main()
{
	GPIO_Init();
	Timer0_Init();
	EA = 1;		             //���ж�����λ	
    
    LED_print(3,3,4,1,5,6,7,10);
	disp[8]=9;
  
	Dis_flag=0;

	while(1)
	{
		if( flag_1mS )	 { flag_1mS = 0;   Func_1mS();  }
		if( flag_10mS )	 { flag_10mS = 0;  Func_10mS(); }
		if( flag_100mS ) { flag_100mS = 0; Func_100mS();}
	}	
}
