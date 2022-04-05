#include "KEY.H"
//#include "led.h"
#include "Delay.h"
#include "FM.h"
														  
unsigned char Radio_Write_Data[8]={					 
	0xc0,0x11,          //02H:��Ƶ������������ã�12MHZ������״̬
	0x1a,0x50,          //03H:97500KHZ,Ƶ��ʹ��87-108M(US/Europe)������100KHZ
	0x40,0x02,		    //04H:1-0ΪGPIO1(10Ϊ�ͣ�������11Ϊ�ߣ�����)��...
	0x88,0xa5 };	    //a5�е�5Ϊ��ʼ����
unsigned long frequency=0;
unsigned int chan=0;
unsigned char volume; 
/*ϵͳ��ʼ������
*��ʼ��ϵͳ��A/D,IO���Լ�������оƬ��RDA5807P��
*/
void Init()
{ 
	Init_KEY();		
	Config_LED();
		
	/*FMӲ����ʼ��*/
	 P4M0=0x00;
   P4M1=0x00;
	
	 P5M0=0xff;
	 P5M1=0x00;
	
	 Init_Radio();
	P2_3 = 0;
}

/*������
*��ʼ��ϵͳ��Ȼ��ѭ�����ð������������Լ��������ʾ����
*/
void main()
{
	Init();	
	while(1)
	{
		Frequency_Display();
		Search_Keyscan();
		KeyScan();			
	}
}