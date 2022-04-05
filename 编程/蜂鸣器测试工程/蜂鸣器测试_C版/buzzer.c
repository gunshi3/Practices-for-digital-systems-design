/**********************
myBeep ����������
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------���ű�������---------*/
sbit sbtBeep = P3 ^ 4;                  //����������
sbit sbtKey1 = P3 ^ 2;                  //����1����

/*---------��������---------*/
bit btBeepFlag;                        //���Ʒ��������صı�־λ

/*---------��ʼ������--------*/
void init()
{
    P3M1 = 0x00;
    P3M0 = 0x10;                  //����P3^4Ϊ����ģʽ

    TMOD = 0x00;                  //���ö�ʱ��0��������ʽ0��16λ�Զ���װ��ʱ��
    TH0 = 0xff;                   //�趨��ʱ��0�ĳ�ֵ
    TL0 = 0x03;
    EA = 1;                       //�����ж�
    ET0 = 1;                      //�򿪶�ʱ��0�ж�����λ
    TR0 = 1;

    btBeepFlag = 0;                //��־λ��1
    P0 = 0x00;                    //�ر�P0�˿�
    sbtBeep = 0;                  //������������0���Ա���������
}

/*---------��ʱ�Ӻ���--------*/
void delay( uint xms )
{
    uchar i;
    for( ; xms > 0; xms-- )
        for( i = 114; i > 0; i-- )
        {
            ;
        }
}

/*---------������--------*/
void main()
{
    init();
    while( 1 )
    {
        if( sbtKey1 == 0 )
        {
            delay( 10 );                     //��ʱ����
            if( sbtKey1 == 0 )
            {
                while( !sbtKey1 );
                btBeepFlag = ~btBeepFlag;    //���������ر�־λ��ת
            }
        }
    }
}

/*---------T0��ʱ���жϷ�������--------*/
void T0_Process() interrupt 1
{
    if( btBeepFlag )
    {
        sbtBeep = ~sbtBeep;            //��������ʹ�÷���������
    }
    else
    {
        sbtBeep = 0;                  //ֹͣ����������sbtBeep�˿����ڵ͵�ƽ
    }
}
