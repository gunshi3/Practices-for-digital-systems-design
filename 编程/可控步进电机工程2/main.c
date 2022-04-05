/**********************
STC-B_DEMO.c for �ɿز������(����ת��ͣ)
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------�궨��---------*/
#define cstDigBegin   7         // �������ʾλѡ��ʼ,�û����޸�
#define cstDigEnd     7         // �������ʾλѡ��ֹλ��8Ϊ���������,����8 �����ڵ�����ʾ����

#define cstDig0       11        //����ܸ�λ����ʾ�����������,�û����޸�
#define cstDig1       11        //11�����ѡ����ʾ  
#define cstDig2       11
#define cstDig3       11
#define cstDig4       11
#define cstDig5       11
#define cstDig6       11
#define cstDig7       ucSpeed_Seg  //P0����ʾ��������ٶȣ���Χ1-4,1���ٶ����
#define cstDig8       ucLedTmp         //��ˮ��

#define cstKeyMaxNum      10                            //10�ζ�ȡ����ֵ
#define cstKeyCntMaxNum   (0x80 + cstKeyMaxNum / 3 * 2)  //��������������ֵ

#define cstSpeedMaxLevel  5   //���޷�Χ��ԭ����4���ٶȣ�������죬
#define cstSpeed100usCnt  50  //ԭ��������5ms*ucSpeed,���������100us���Ĵ���
#define cstClockXmsMaxNum ((ucSpeed)*(cstSpeed100usCnt)) //Xmsʱ��

/*---------��������---------*/
void Init();                  //Initial
void T10us_Process();         //Function for 10us
void T100us_Process();        //Function for 100us
void T1ms_Process();          //Function for 1ms
void T10ms_Process();         //Function for 10ms
void T100ms_Process();        //Function for 100ms
void TXms_Process();          //Function for Xms,�û����޸�

void Seg7LedDisplay( uchar s, uchar e );  //��ʾ��������ܻ���ˮ��
void Key_Scan();                          //����ɨ����
void Xstep_Process();                     //�����������ת��ͣ����

/*---------���ű�������---------*/
sbit sbtKey1 = P3 ^ 2;            //����1,����ѧϰ��,�û����޸�
sbit sbtKey2 = P3 ^ 3;            //����2
sbit sbtKey3 = P1 ^ 7;            //����3
sbit sbtS1  = P4 ^ 1;             //���������1
sbit sbtS2  = P4 ^ 2;             //���������2
sbit sbtS3  = P4 ^ 3;             //���������3
sbit sbtS4  = P4 ^ 4;             //���������4

/*---------��������---------*/
//��ʱ������ʱ���־
bit btT10usFlag = 0;      //ʱ��10uS
bit btT100usFlag = 0;     //50����5ms����ͨ��ʱ��
bit btT1msFlag = 0;       //�����ɨ��Ƶ�ʽ���
bit btT10msFlag = 0;      //������������
bit btT100msFlag = 0;     //�������ʾ���ݸ���
bit btTXmsFlag = 0;       //���������� ucSpeed*1mS
bit btTXmsFlag_en = 1;    //���������������,Ĭ�Ͽ���(0��ͣ1����)

//��ʱ���ڲ�ר��ʱ�����
uchar ucT10usCnt = 0;    //ʱ��10uS
uchar ucT100usCnt = 0;   //ԭ�������������5ms ����ͨ��ʱ��
uchar ucT1msCnt = 0;
uchar ucT10msCnt = 0;
uchar ucT100msCnt = 0;

// Fun()�û�����ʵ�����ñ���
uchar ucKey1Cnt = 0;    //����1����������
uchar ucKey2Cnt = 0;    //����2����������
uchar ucKey3Cnt = 0;    //����3����������
uchar ucKeyAllCnt = 0;   //������������ʱ������

uchar ucSpeed_Seg = 1;    //�ٶ���ʾ���
uchar ucSpeed = 1;        //����������ٶȲ���

bit  btKey1Current = 1;   //����1��ǰ��״̬
bit  btKey1Past = 1;      //����1ǰһ��״̬
bit  btKey2Current = 1;   //����2��ǰ��״̬
bit  btKey2Past = 1;      //����2ǰһ��״̬
bit  btKey3Current = 1;   //����3��ǰ��״̬
bit  btKey3Past = 1;      //����3ǰһ��״̬
bit  btRotationFlag = 0;   //���������ת����0��ת1��ת

uchar ucClockXms = 0;     //������������źſ�������ת��ͣ���ü�����
uchar ucLedTmp = 0x00;    //��ˮ����ʾֵ

/*---------������---------*/
void main()
{
    Init();
    ucLedTmp = 0x00;            //��ˮ����ʾֵ
    while( 1 )
    {
        if( btT10usFlag )
        {
            btT10usFlag = 0;  T10us_Process();
        }
        if( btT100usFlag )
        {
            btT100usFlag = 0;  T100us_Process();
        }
        if( btT1msFlag )
        {
            btT1msFlag = 0;  T1ms_Process();
        }
        if( btT10msFlag )
        {
            btT10msFlag = 0;  T10ms_Process();
        }
        if( btT100msFlag )
        {
            btT100msFlag = 0; T100ms_Process();
        }
        if( ( btTXmsFlag && btTXmsFlag_en ) )  //Xms��־λ��ʹ��λ��Ч
//��ִ�в������ת��
        {
            btTXmsFlag = 0;  TXms_Process();
        }
    }
}

/*---------T0��ʱ���жϷ�������--------*/
void T0_Process() interrupt 1
{
    btT10usFlag = 1;
    ucT10usCnt++;
    if( ucT10usCnt == 10 )
    {
        ucT10usCnt = 0;  btT100usFlag = 1;   ucT100usCnt++;
    }
    if( ucT100usCnt == 10 )
    {
        ucT100usCnt = 0;  btT1msFlag = 1;    ucT1msCnt++;
    }
    if( ucT1msCnt == 10 )
    {
        ucT1msCnt = 0;   btT10msFlag = 1;   ucT10msCnt++;
    }
    if( ucT10msCnt == 10 )
    {
        ucT10msCnt = 0;  btT100msFlag = 1;
    }
}
/*---------��ʼ������--------*/
void Init()
{
    //Hardware Init
    P0M0 = 0xFF;
    P0M1 = 0x00;
    P0 = 0x00;
    P2M0 = 0x0f; //����P2.0-3Ϊ����:��Ӧ��LED�Ŀ���
    P2M1 = 0x00;
    P4M0 = 0xff;    //P4�������������
    P4M1 = 0xe1;
    P4 = 0xe1;      //����P4��Ϊ�͵�ƽ�����⸴λʱ�������5V��ƽ�ӿڵ�Ӱ��

    //Interrupt Init
    /*10΢��@11.0592MHz 16������  ��ʱ��0
      ISP������ɣ�ԭ����1ms��11.592MHz 12Tģʽ */
    AUXR |= 0x80;   //��ʱ��ʱ��1Tģʽ
    TMOD &= 0xF0;   //���ö�ʱ��ģʽ
    TL0 = 0x91;     //���ö�ʱ��ֵ
    TH0 = 0xFF;     //���ö�ʱ��ֵ
    EA = 1;         //���ܵ��ж�
    ET0 = 1;        //������ʱ���ж�
    TF0 = 0;        //���TF0��־
    TR0 = 1;        //������ʱ��

    ucKey1Cnt = cstKeyCntMaxNum;
    ucKey2Cnt = cstKeyCntMaxNum;
    ucKey3Cnt = cstKeyCntMaxNum;
    ucKeyAllCnt = cstKeyMaxNum;
}

/*---------��ʱ�ӽ��Ĵ�����--------*/
void T10us_Process()
{

}
void T100us_Process()           //100us���ò������ÿ�ĵ������,����������־
{
    ucClockXms++;
    if ( ucClockXms == ( cstClockXmsMaxNum ) )
    {
        ucClockXms = 0;   btTXmsFlag = 1;
    }
}
void T1ms_Process()             //1ms���������ɨ�����
{
    Seg7LedDisplay( cstDigBegin, cstDigEnd );
}
void T10ms_Process()           //10ms���ð���ɨ�����
{
    Key_Scan();
}
void TXms_Process()            //Xms���ò����������
{
    Xstep_Process();
}
void T100ms_Process()          //100ms����������ʾ����
{
    ucSpeed_Seg = ucSpeed;
}

/*---------������뷢���������ʾ����--------*/
void Seg7LedDisplay( uchar s, uchar  e )
{
    unsigned char arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40, 0x00}; //  �������ʾ�����
    /* ���:   0    1    2    3     4    5    6      7     8     9     10   11          */
    /* ��ʾ:   0    1    2    3     4    5    6      7     8     9     -   (��)      */
    static  int  i;
    P2 = ( P2 & 0xf0 ) | i;
    switch( i )
    {
        case 0:
            P0 = arrSegSelect[cstDig0];   break;
        case 1:
            P0 = arrSegSelect[cstDig1];   break;
        case 2:
            P0 = arrSegSelect[cstDig2];   break;
        case 3:
            P0 = arrSegSelect[cstDig3];   break;
        case 4:
            P0 = arrSegSelect[cstDig4];   break;
        case 5:
            P0 = arrSegSelect[cstDig5];   break;
        case 6:
            P0 = arrSegSelect[cstDig6];   break;
        case 7:
            P0 = arrSegSelect[cstDig7];   break;
        case 8:
            P0 = cstDig8;           break;
        default:
            P0 = 0x00;            break;
    }
    if( ++i > e ) i = s;
}

/*---------����ɨ���⺯��--------*/
void Key_Scan()
{
    if( sbtKey1 == 0 )                          //��������ʱ��������������1
        ucKey1Cnt--;
    if( sbtKey2 == 0 )
        ucKey2Cnt--;
    if( sbtKey3 == 0 )
        ucKey3Cnt--;
    ucKeyAllCnt--;                           //�ܵĴ�����1

    if( ucKeyAllCnt == 0 )                      //10������
    {
        if( ucKey2Cnt < 0x80 )
        {
            btKey2Current = 0;
            if( btKey2Past == 1 )
            {
                btKey2Past = 0;                 //�½���
                ucSpeed++;     //���������ת�ٸı䣬ucSpeedֵԽ��ת��Խ��
                if( ucSpeed == cstSpeedMaxLevel )
                    ucSpeed = 1;
            }
        }
        if( ucKey2Cnt >= 0x80 )
        {
            btKey2Current = 1;
            if( btKey2Past == 0 )
                btKey2Past = 1;                //�����أ����費�������Ǿͼ�����
        }
        if( ucKey1Cnt < 0x80 )
        {
            btKey1Current = 0;
            if( btKey1Past == 1 )
            {
                btKey1Past = 0;               //�½��أ������ı䲽�����ת��
                btRotationFlag = ~btRotationFlag;
            }
        }
        if( ucKey1Cnt >= 0x80 )
        {
            btKey1Current = 1;
            if( btKey1Past == 0 )
                btKey1Past = 1;                //�����أ����費�������Ǿͼ�����
        }
        if( ucKey3Cnt < 0x80 )
        {
            btKey3Current = 0;
            if( btKey3Past == 1 )
            {
                btKey3Past = 0;                  //�½���
                btTXmsFlag_en = ~btTXmsFlag_en;  //�����ı���ͣ�������
            }
        }
        if( ucKey3Cnt >= 0x80 )
        {
            btKey3Current = 1;
            if( btKey3Past == 0 )
                btKey3Past = 1;                //�����أ����費�������Ǿͼ�����
        }
        ucKey1Cnt = cstKeyCntMaxNum;          //��һ���ж�װ��ֵ
        ucKey2Cnt = cstKeyCntMaxNum;
        ucKey3Cnt = cstKeyCntMaxNum;
        ucKeyAllCnt = cstKeyMaxNum;
    }
}

/*---------�����������ת��ͣ���ƺ���--------*/
void Xstep_Process()
{
    static uchar i = 1;                      //����������������Ӧ��IO��λ��
    if( btRotationFlag == 0 )                 //���Ʋ��������ת
    {
        switch( i++ )                      //ѭ���ı䲻ͬIO�ŵ������ƽ
        {
            case 1:
                sbtS1 = 1;  sbtS2 = 0;  sbtS3 = 0;  sbtS4 = 0;  break;
            case 2:
                sbtS1 = 0;  sbtS2 = 1;  sbtS3 = 0;  sbtS4 = 0;  break;
            case 3:
                sbtS1 = 0;  sbtS2 = 0;  sbtS3 = 1;  sbtS4 = 0;  break;
            case 4:
                sbtS1 = 0;  sbtS2 = 0;  sbtS3 = 0;  sbtS4 = 1;  break;
            default:
                sbtS1 = 0;  sbtS2 = 0;  sbtS3 = 0;  sbtS4 = 0;  break;
        }
        if( i == 5 )
            i = 1;
    }                                      //���Ʋ��������ת
    else
    {
        switch( i++ )
        {
            case 1:
                sbtS1 = 0;  sbtS2 = 0;  sbtS3 = 0;  sbtS4 = 1;  break;
            case 2:
                sbtS1 = 0;  sbtS2 = 0;  sbtS3 = 1;  sbtS4 = 0;  break;
            case 3:
                sbtS1 = 0;  sbtS2 = 1;  sbtS3 = 0;  sbtS4 = 0;  break;
            case 4:
                sbtS1 = 1;  sbtS2 = 0;  sbtS3 = 0;  sbtS4 = 0;  break;
            default:
                sbtS1 = 0;  sbtS2 = 0;  sbtS3 = 0;  sbtS4 = 0;  break;
        }
        if( i == 5 )
            i = 1;
    }
}
