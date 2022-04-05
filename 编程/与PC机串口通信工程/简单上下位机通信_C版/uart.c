/**********************
myUsbUart ���ڲ���
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include<STC15F2K60S2.h>
#include<intrins.h>
#define uchar unsigned char
#define uint unsigned int

/*---------�궨��---------*/
#define cstFocs      11059200L  //����Ƶ��
#define cstBaud1     9600       //������
#define cstKeyMaxNum 100        //������������

/*---------���ű�������---------*/
sbit sbtKey1 = P3 ^ 2 ;             //��������
sbit sbtKey2 = P3 ^ 3 ;             //���ּ���
sbit sbtKey3 = P1 ^ 7 ;             //��������
sbit sbtLedSel = P2 ^ 3;

/*---------��������---------*/
uchar ucT100usTimes;

uint uiKey1Cnt;            //����1����
uint uiKey2Cnt;            //����2����
uint uiKey3Cnt;            //����3����
uint uiKeyAllCnt;          //�����ܵĶ�������
bit btT1msFlag;            //1ms�ı�־
bit btKey1Current;/*key1��ǰ��״̬*/
bit btKey1Past;   /*key1ǰһ��״̬*/
bit btKey2Current;/*key2��ǰ��״̬*/
bit btKey2Past;   /*key2ǰһ��״̬*/
bit btKey3Current;/*key3��ǰ��״̬*/
bit btKey3Past;   /*key3ǰһ��״̬*/

/*�շ���ʾ�������*/
bit btUart1SendBusy = 0 ;
uchar ucDateTmp;               //���������ݴ�
uchar ucDateDigState;
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71}; //��ʾ0-f

/*---------��ʱ��T0�жϴ�����--------*/
void T0_Process() interrupt 1
{
    TH0 = ( 65535 - 1000 ) / 256; //��ʱ����ʼֵ
    TL0 = ( 65535 - 1000 ) % 256;
    ucT100usTimes++;
    if( ucT100usTimes == 10 )        //�ж�10�ζ�Ӧ1ms
    {
        ucT100usTimes = 0;
        btT1msFlag = 1;
    }
    ucDateDigState++;
    if( ucDateDigState == 2 )
        ucDateDigState = 0;
    P0 = 0;
    switch( ucDateDigState )
    {
        case 0:
            P2 = 0x00;  P0 = arrSegSelect[ucDateTmp / 16];  
break;
        case 1:
            P2 = 0x01;  P0 = arrSegSelect[ucDateTmp % 16];  break;
    }
}

/*---------����1��ʼ������--------*/
void Uart1_Init( void )
{
    AUXR = 0X80; //�����Ĵ��� ��ʱ��ʱ��0���ٶ��Ǵ�ͳ��12��������Ƶ
    SCON |= 0X50; //�������
    TL1 = ( 65536 - ( cstFocs / 4 / cstBaud1 ) );
    TH1 = ( 65536 - ( cstFocs / 4 / cstBaud1 ) ) >> 8;
    AUXR |= 0X40; //�����Ĵ��� ��ʱ��ʱ��1���ٶ��Ǵ�ͳ��12��������Ƶ
    RI = 0;      //�����жϱ�־λ
    TI = 0;      //�����жϱ�־λ
    TR1 = 1;     //������ʱ��1
    ES = 1;      //�����ж�����λ
    EA = 1;      //���ж�����λ
    PS = 1 ;    //����1�жϸ����ȼ�
}

/*---------�������ݺ���--------*/
void SendData( unsigned char dat )
{
    while( btUart1SendBusy ); //���͵����ַ���UART1�Է��͵�PC��
    btUart1SendBusy = 1;
    SBUF = dat;
}

/*---------����1�жϴ�����--------*/
void Uart1_Process() interrupt 4 using 1
{
    if( RI )         //���������ݺ� RI�Զ���1
    {
        RI = 0;   ucDateTmp = SBUF;
    }
    if( TI )        //���������ݺ� RI�Զ���1
    {
        TI = 0;   btUart1SendBusy = 0;
    }
}

/*---------��ʼ������--------*/
void Init()
{
    P3M0 = 0x00;
    P3M1 = 0x00;
    P2M0 = 0xff;
    P2M1 = 0x00;
    P0M0 = 0xff;
    P0M1 = 0x00;

    TMOD = 0x01; //��ʱ��0����ʽ1
    ET0 = 1; //������ʱ���ж�
    TH0 = ( 65535 - 1000 ) / 256;
    TL0 = ( 65535 - 1000 ) % 256;
    TR0 = 1; //������ʱ��

    Uart1_Init();   //�ⲿ�жϣ������ȼ�

    ucDateTmp = 0x00;
    sbtLedSel = 0;
    btT1msFlag = 0;

    /*��ʼ�����а����ĵ�ǰ״̬��ǰһ��״̬*/
    btKey1Current = 1; /*key1��ǰ��״̬*/
    btKey1Past = 1; /*key1ǰһ��״̬*/
    btKey2Current = 1; /*key2��ǰ��״̬*/
    btKey2Past = 1; /*key2ǰһ��״̬*/
    btKey3Current = 1; /*key3��ǰ��״̬*/
    btKey3Past = 1; /*key3ǰһ��״̬*/

    uiKey1Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKey2Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKey3Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
    uiKeyAllCnt = cstKeyMaxNum;
}

/*---------������--------*/
void main()
{
    Init();
    while( 1 )
    {
        if( btT1msFlag )
        {
            btT1msFlag = 0;
            if( sbtKey1 == 0 )
                uiKey1Cnt--;
            if( sbtKey2 == 0 )
                uiKey2Cnt--;
            if( sbtKey3 == 0 ) //�����ǰ���״̬
                uiKey3Cnt--;
            uiKeyAllCnt--;//�ܵĴ�����1

            if( uiKeyAllCnt == 0 ) //100������
            {

                if( uiKey1Cnt < 0x80 )
                {
                    btKey1Current = 0;
                    if( btKey1Past == 1 ) //�½��أ�������������
                    {
                        btKey1Past = 0;
                        SendData( ucDateTmp ) ;
                    }
                }
                if( uiKey1Cnt >= 0x80 )
                {
                    btKey1Current = 1;
                    if( btKey1Past == 0 )
                        btKey1Past = 1;  //�����أ����費�������Ǿͼ�����
                }
                if( uiKey2Cnt < 0x80 )
                {
                    btKey2Current = 0;
                    if( btKey2Past == 1 ) //�½��أ�������������
                    {
                        btKey2Past = 0;
                        ucDateTmp--;
                    }
                }
                if( uiKey2Cnt >= 0x80 )
                {
                    btKey2Current = 1;
                    if( btKey2Past == 0 )
                        btKey2Past = 1;  //�����أ����費�������Ǿͼ�����
                }

                if( uiKey3Cnt < 0x80 )
                {
                    btKey3Current = 0;
                    if( btKey3Past == 1 ) //�½��أ�������������
                    {
                        btKey3Past = 0;
                        ucDateTmp++;
                    }
                }
                if( uiKey3Cnt >= 0x80 )
                {
                    btKey3Current = 1;
                    if( btKey3Past == 0 )
                        btKey3Past = 1;  //�����أ����費�������Ǿͼ�����
                }

                /*��һ�ֵ��ж�*/
                uiKey1Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
                uiKey2Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
                uiKey3Cnt = 0x80 + cstKeyMaxNum / 3 * 2;
                uiKeyAllCnt = cstKeyMaxNum;
            }
        }
    }
}
