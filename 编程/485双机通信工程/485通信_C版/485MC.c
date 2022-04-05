/**********************
myM485 485˫��ͨ�����̲���
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#include <intrins.h>
#define uint  unsigned int
#define uchar unsigned char
#define ulong unsigned long

/*---------�궨��---------*/
#define cstUart2Ri  0x01                        //�����ж������־λ
#define cstUart2Ti  0x02                        //�����ж������־λ

#define cstNoneParity           0           //��У��
#define PARITYBIT cstNoneParity         //����У��λ

/*���ڲ��������*/
#define cstFosc 11059200L               //ϵͳʱ��Ƶ�� 
#define cstBaud2    9600                    //���ڲ�����    
#define cstT2HL     (65536-(cstFosc/4/cstBaud2))    //��ʱ����ʼʱ��

/*---------���ű�������---------*/
sbit sbtKey1 = P3 ^ 2 ;                 //��������
sbit sbtKey2 = P3 ^ 3 ;                 //���ּ���
sbit sbtKey3 = P1 ^ 7 ;                 //��������

sbit sbtSel0 = P2 ^ 0 ;
sbit sbtSel1 = P2 ^ 1 ;
sbit sbtSel2 = P2 ^ 2 ;
sbit sbtLedSel = P2 ^ 3 ;

sbit sbtM485_TRN  = P3 ^ 7 ;   //����MAX485ʹ�����ţ�Ϊ1ʱ���ͣ�Ϊ0ʱ����

/*---------��������---------*/
bit btSendBusy ;                            //Ϊ1ʱæ���������ݣ���Ϊ0ʱ��
uchar ucGetDataTmp ;          //���������ݴ�
uchar ucPutDataTmp ;          //���������ݴ�
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
                        0x6d, 0x7d, 0x07, 0x7f, 0x6f,
                        0x77, 0x7c, 0x39, 0x5e, 0x79,
                        0x71, 0x40, 0x00
                       };   //��ѡ����ʾ0-f

/*---------����2��ʼ���������ʷ�������---------*/
void Uart2Init( void )
{
    S2CON = 0x10 ;      //������У��λ�������п�2����
    T2L = cstT2HL ;             //���ò�������װֵ
    T2H = cstT2HL >> 8 ;
    AUXR |= 0x14 ;      //T2Ϊ1Tģʽ����������ʱ��2
}

/*---------ϵͳӲ����������ʼ������---------*/
void Init()
{
    P0M0 = 0xff ;       //P0�����죨�Ŵ������
    P0M1 = 0x00 ;
    P2M0 = 0x0f ;       //P2.0��~P2.3������
    P2M1 = 0x00 ;
    P3M0 = 0x00 ;
    P3M1 = 0x00 ;
    P1M0 = 0x00 ;
    P1M1 = 0x00 ;       //P1��P3׼˫���
    //�ⲿ�ж�0
    IT0 = 1 ;           //�½��ش����жϣ�Ϊ0���½��غ������ؾ��ᴥ���жϣ�
    EX0 = 1 ;           //�����ⲿ�ж�0
    PX0 = 0 ;           //�ⲿ�жϣ������ȼ�
    //485��ʼ��  ����������
    sbtM485_TRN = 0 ;       //��ʼΪ����״̬
    P_SW2 |= 0x01 ;     //�л�����2�Ĺܽŵ�P4.6,P4.7
    Uart2Init() ;
    btSendBusy = 1 ;
    IE2 |= 0x01 ;       //�����п�2�ж�
    IP2 |= 0x01 ;       //���ô��п��жϣ������ȼ�
    EA = 1 ;            //�����ж�
    //�����ѡ��
    sbtLedSel = 0 ;     //�����������ʾ
    sbtSel0 = 1 ;
    sbtSel1 = 1 ;
    sbtSel2 = 1 ;           //ѡ��ڰ�λ�������ʾ
    ucPutDataTmp = 0 ;
}

/*---------��ʱ����---------*/
void delay( void )
{
    uchar i, j;
    for( i = 0; i < 255; i++ )
        for( j = 0; j < 255; j++ )
            ;
}

/*---------����2�жϴ������---------*/
void Uart2_Process( void ) interrupt 8 using 1
{
    if( S2CON & cstUart2Ri )
    {
        ucGetDataTmp = S2BUF ;
        ucPutDataTmp = ucGetDataTmp ;
        S2CON &= ~cstUart2Ri;   //�����жϱ�־λ��0
    }
    if( S2CON & cstUart2Ti )
    {
        btSendBusy = 0 ;            //���æ�ź�
        S2CON &= ~cstUart2Ti ;      //�����жϱ�־λ��0
    }
}

/*---------�ⲿ�ж�0�������---------*/
void ExInt0_Process() interrupt 0
{
    sbtM485_TRN = 1 ;
    S2BUF = ucPutDataTmp ;
    while( btSendBusy ) ;
    btSendBusy = 1 ;
    sbtM485_TRN = 0 ;
}

/*---------������---------*/
void main( void )
{
    Init() ;
    while( 1 )
    {
        ucPutDataTmp %= 16 ;
        P0 = arrSegSelect[ucPutDataTmp] ;

        if( sbtKey3 == 0 )
        {
            delay();
            if( sbtKey3 == 0 )
            {
                while( !sbtKey3 );
                ucPutDataTmp++ ;
            }
        }
        if( sbtKey2 == 0 )
        {
            delay();
            if( sbtKey2 == 0 )
            {
                while( !sbtKey2 );
                ucPutDataTmp-- ;
            }
        }
    }
}
