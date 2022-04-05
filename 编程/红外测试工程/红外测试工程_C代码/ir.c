/**********************
myIr  ����ͨ�Ų���
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uchar unsigned char
/*---------���ű�������---------*/
sbit sbtLedSel = P2 ^ 3;            //LED�����������ʾ�л�
sbit sbtKey1 = P3 ^ 2;              //����1 ��Ӧ�ⲿ�ж�
sbit sbtGetIr = P3 ^ 6;             //P3.6���Ӻ������ͷ
sbit sbtPutIr = P3 ^ 5;             //P3.5���Ӻ����߷��������

/*---------��������---------*/
uchar ucPutIrFlagN = 1;       //���λ������Ƿ������塣0�����͡�   1��������

/*--------��ʼ����ʱ��T0����---------*/
void InitT0()               //��ʱ��T0��ʼ��
{
    AUXR |= 0x80;                   //1T
    TMOD &= 0xF0;                  //���֮ǰ������
    TMOD |= 0x02;                  //����Ϊ8λ�Զ���װ
    TL0 = 0x70;                     //��ʱ����ʼֵ
    TH0 = 0x70;                     //�Զ���װ��
    TF0 = 0;                        //���TF0���
    TR0 = 1;                        //T0��ʼ����
}

/*--------��ʱ����---------*/
void Delay()  //��ʱ100ms
{
    unsigned char i, j, k;
    i = 5;
    j = 52;
    k = 195;
    do
    {
        do
        {
            while ( --k );
        }
        while ( --j );
    }
    while ( --i );
}

/*--------��ʱ��0�жϺ���---------*/
void T0_Processs() interrupt 1
{
    if( ucPutIrFlagN == 0 )    //���ucPutIrFlagN=0
        sbtPutIr = ~sbtPutIr; //P3.5���ݶ�ʱ���жϵ�Ƶ�ʲ�����ת��Ƶ����Ϊ38kHz��
    else
        sbtPutIr = 0;       //������
}

/*--------������---------*/
void main()
{
    //�����������
    P0M0 = 0XFF;
    P0M1 = 0X00;
    P2M0 = 0XFF;
    P2M1 = 0X00;

    P0 = 0;             //��ʼ��P0
    sbtLedSel = 1;       //ѡ��LED����

    InitT0();
    ET0 = 1;            //�򿪶�ʱ��T0�ж�
    EA = 1;             //�����ж�

    while( 1 )
    {
        P0 = !sbtGetIr;      //P0��ʾֵ
        while( !sbtKey1 )     //���sbtKey1���水��
        {
            ucPutIrFlagN = 0;                   //�������������
            Delay();
            ucPutIrFlagN = 1;                   //���������������
            Delay();
        }
    }
}
