/**********************
myIr1  ����ͨ��1�����ֽ�
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------�궨��---------*/
#define cstKeyCheckTime 75          //��������������
#define cstKeyMinTime 50            //����ʶ��Ϊ����ʱ���⵽�����ٴ���

/*---------���ű�������---------*/
sbit sbtSel0 = P2 ^ 0;                              //λѡ�ź�
sbit sbtSel1 = P2 ^ 1;                              //λѡ�ź�
sbit sbtSel2 = P2 ^ 2;                              //λѡ�ź�
sbit sbtLedSel = P2 ^ 3;                            //����ܺ�LED��ѡ���ź�

sbit sbtPutIr = P3 ^ 5;                             //�����߷�������
sbit sbtGetIr = P3 ^ 7;                             //����1��������

sbit sbtKey1 = P3 ^ 2;                              //����1  �����ź�
sbit sbtKey2 = P3 ^ 3;                              //����2  ���ݼ�1
sbit sbtKey3 = P1 ^ 7;                              //����3  �շ�ģʽ����

/*---------��������---------*/
int time = cstKeyCheckTime;                 //���������ܼ���(һ�����ڼ��cstKeyCheckTime��)
int intKey1Cnt = 0;                         //�����м�⵽sbtKey1=0�Ĵ���
int intKey2Cnt = 0;                         //�����м�⵽sbtKey2=0�Ĵ���
int intKey3Cnt = 0;                         //�����м�⵽sbtKey3=0�Ĵ���
int intKey1State = 1;       //sbtKey1״̬������Ϊ0�����Ѱ��£�1����δ����
int intKey2State = 1;       //sbtKey2״̬������Ϊ0�����Ѱ��£�1����δ����
int intKey3State = 1;       //sbtKey3״̬������ά0�����Ѱ��£�1����δ����

uint  uiSeg7Num = 0;              //�������ʾ������
uchar ucPutIrFlagN = 1;             //״̬��ǣ�0����ɷ��ͣ�1��2�����ɷ���
int intMyBuf = 11;                 //���ջ��壬�����շ���־0xca���ж�
uchar arrSegSelect[] =              //��ѡ����ʾ0-f
{
    0x3f, 0x06, 0x5b, 0x4f,
    0x66, 0x6d, 0x7d, 0x07,
    0x7f, 0x6f, 0x77, 0x7c,
    0x39, 0x5e, 0x79, 0x71
};

/*---------��������---------*/
//�жϺ�������
void TimerInit();      //��ʱ������
void Uart1Init();      //����1����
void Init();          //��ʼ�������죬�����жϿ���
void DigSelct( int );   //���������λѡ
void CheckKey();     //��������Լ���������(��ʱ��0�ж��е���)

/*---------������---------*/
void main()
{
    TimerInit();        //���ö�ʱ��
    Uart1Init();            //���ô���1
    Init();                 //��ʼ��
    while( 1 );
}

/*---------��ʼ������---------*/
//�������죬�жϿ�������
void Init()
{
    P0M1 = 0x00;
    P0M0 = 0xff;
    P2M1 = 0x00;
    P2M0 = 0x08;
    //�жϿ������ã���������ֲ�P458
    ET1 = 0;            //��ֹT1�ж�
    ET0 = 1;            //�򿪶�ʱ��T0�ж�
    ES = 1;             //�򿪴���1�ж�
    IE2 = 0X04;         //�򿪶�ʱ��2�ж�
    EA = 1;             //�����ж�
}

/*---------����1��ʼ������---------*/
void Uart1Init()    //���÷����������ֲ�P621(����1����)��P498(��ʱ��T1����)
{
    PCON &= 0x7F;       //�����ʲ����٣�SMOD=0
    SCON = 0x50;        //����1ʹ�ù�����ʽ1��REN=1(�����н���)
    AUXR &= 0xFE;       //����1ѡ��ʱ��T1��Ϊ�����ʷ�������S1ST2=0
    AUXR1 = 0x40;       //����1��P3.6���գ���P3.7����
    PS = 1;             //���ô����ж�Ϊ������ȼ�
}

/*---------��ʱ����ʼ������---------*/
void TimerInit()    //���÷����������ֲ�P498
{
    AUXR |= 0x40;       //��ʱ��T1Ϊ1Tģʽ���ٶ��Ǵ�ͳ8051��12��������Ƶ��
    TMOD &= 0x0F;     //���T1ģʽλ
    TMOD |= 0x20;      //����T1ģʽλ��ʹ��8λ�Զ���װģʽ
    TL1 = 0x70;         //���ó�ֵ
    TH1 = 0x70;         //����T1��װֵ
    TR1 = 1;            //T1���п���λ��1������T1����

    AUXR |= 0x80;       //��ʱ��T0Ϊ1Tģʽ�����ٶ��Ǵ�ͳ8051��12��������Ƶ��
    TMOD &= 0xF0;       //���T0ģʽλ
    TMOD |= 0x02;       //����T0ģʽλ��ʹ��8λ�Զ���װģʽ
    TL0 = 0x70;          //���ó�ֵ
    TH0 = 0x70;         //��T0��װֵ
    TF0 = 0;             //T0�����־λ����
    TR0 = 1;             //T0���п���λ��1������T0����

    //��ʱ��T2������ʾ�Ͱ���������500us��ʱ16λ�Զ���װ
    AUXR |= 0x04;       //��ʱ��T2Ϊ1Tģʽ
    T2L = 0x66;         //��λ��װֵ
    T2H = 0xEA;         //��λ��װֵ
    AUXR |= 0x10;       //��ʱ��2��ʼ��ʱ
}

/*---------�������ʾλ��ѡ����---------*/
void DigSelct( int x )
{
    sbtSel0 = x % 2;
    sbtSel1 = x % 4 / 2;
    sbtSel2 = x / 4;
}

/*---------������⺯��---------*/
//����������һ�ΰ����ļ�⣬�Լ����cstKeyCheckTime�κ�Ĳ�������Ҫ����ظ����ã�
void CheckKey()
{
    time--;
    if( sbtKey1 == 0 )
        intKey1Cnt++;
    if( sbtKey2 == 0 )
        intKey2Cnt++;
    if( sbtKey3 == 0 )
        intKey3Cnt++;
    if( time <= 0 )                                 //һ�����ڽ���
    {
        if( intKey1Cnt >= cstKeyMinTime )   //�ж�sbtKey1�����Ϊ���µĴ����Ƿ���ڰ���ʶ��Ϊ�������⵽�����ٴ���
        {
            if( intKey1State == 1 )             //�ж��Ƿ��Ѿ�������
            {
                if( ucPutIrFlagN == 0 )         //�ж��Ƿ���Է���
                {
                    ucPutIrFlagN = 1;         //��һ���ȷ���һ����־  PS����������������־�����չ���������ڵ�����ͷ���ܻ���յ������ź�
                    SBUF = 0xca;             //��־Ϊ0xca����������ڴ����ж����ٷ�������
                }
                intKey1State = 0;               //״̬�ı�Ϊ�Ѿ�������
            }
        }
        else
            intKey1State = 1;                   //״̬�ı�Ϊδ������

        if( intKey2Cnt >= cstKeyMinTime )
        {
            if( intKey2State == 1 )
            {
                uiSeg7Num++;                  //���ݼ�1
                uiSeg7Num %= 10;
                intKey2State = 0;
            }
        }
        else
            intKey2State = 1;

        if( intKey3Cnt >= cstKeyMinTime )
        {
            if( intKey3State == 1 )
            {
                ucPutIrFlagN = !ucPutIrFlagN;       //�ܷ��͵��л�
                REN = ~REN;                    //���������־λ��1����ɽ��գ�0�����ɽ���
                intKey3State = 0;
            }
        }
        else
            intKey3State = 1;

        time = cstKeyCheckTime;
        intKey1Cnt = 0;
        intKey2Cnt = 0;
        intKey3Cnt = 0;
    }
}

/*---------��ʱ��0�жϷ�����---------*/
void T0_Process() interrupt 1
{
    if( sbtGetIr == 0 )     //sbtPutIr����sbtGetIr���źŲ�������
    {
        sbtPutIr = ~sbtPutIr;
    }
    else                        //���P3.7=1��P3.5���0
        sbtPutIr = 0;
}

/*---------����1�жϷ�����---------*/
//�������TI�Զ���1�������ж�; �������RIֵ1�������ж�
void Uart1_Process() interrupt 4
{
    if( TI )                        //�ж��Ƿ����ж�
    {
        TI = 0;                     //�����ж������־λ��0
        if( ucPutIrFlagN == 1 )         //�ж��Ƿ��һ�η���
        {
            ucPutIrFlagN = 2;        //�ڶ��η�������
            SBUF = uiSeg7Num;       //����uiSeg7Num
        }
        if( ucPutIrFlagN == 2 )         //�ж��Ƿ�ڶ��η������
            ucPutIrFlagN = 0;         //������ucPutIrFlagN����
    }
    if( RI )                         //�ж��Ƿ�����ж�
    {
        RI = 0;                     //�����ж������־λ��0
        if( intMyBuf == 0xca )          //�ж���һ���Ƿ��յ�0xca��־
            uiSeg7Num = SBUF;       //��ʽ��������
        intMyBuf = SBUF;            //����ν��յ������ݴ����Զ���Ļ����У��ȴ���һ�εıȽ�
    }
}

/*---------��ʱ��2�жϷ�����---------*/
void T2_Process() interrupt 12
{

    P0 = 0;                              //P0����
    sbtLedSel = ~sbtLedSel;                 //�л���ʾ
    if( sbtLedSel == 0 )
    {
        DigSelct( 7 );                      //ѡ������ܵĵ���λ
        P0 = arrSegSelect[uiSeg7Num];       //�����������ʾ����
    }
    else
        P0 = !ucPutIrFlagN;                //���÷����LED��
    CheckKey();                         //�����������
}
