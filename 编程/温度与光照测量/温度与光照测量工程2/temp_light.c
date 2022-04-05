/**********************
STC-B_DEMO.c for �¶ȹ��ղ�������
�ͺ�:STC15F2K60S2 ��Ƶ:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define ulint unsigned long
#define uint  unsigned int
#define uchar unsigned char

/*---------�궨��---------*/
#define cstDigBegin   0         // �������ʾλѡ��ʼ,�û����޸�
#define cstDigEnd     20        // �������ʾλѡ��ֹλ��8Ϊ���������,����8 �����ڵ�����ʾ����

#define cstDig0       ucDigtmp0 //����ܸ�λ����ʾ�����������,�û����޸�
#define cstDig1       ucDigtmp1 //11�����ѡ����ʾ  
#define cstDig2       ucDigtmp2
#define cstDig3       ucDigtmp3
#define cstDig4       ucDigtmp4
#define cstDig5       ucDigtmp5
#define cstDig6       ucDigtmp6
#define cstDig7       ucDigtmp7
#define cstDig8       ucDigtmp8 //��ˮ��

#define cstClockXmsMaxNum 40 //Xmsʱ��
#define cstSampleNum 4000  //ȡ���ֵ��ƽ��ֵ��С���

/*---------��������---------*/
void InitADC();        //��ʼ��ADC�л�
void InitAdcTherm();   //��ʼ���¶�ADC
void InitAdcPhoto();  //��ʼ����ADC
void ADC_Process();    //ADC�жϷ�����
void Date_ThermToDigit();  //�ֱ�ȡ���¶ȵİ�λ��ʮλ����λ
void Date_PhotoToDigit();  //�ֱ�ȡ�����յİ�λ��ʮλ����λ

void Init();                  //Initial
void T10us_Process();         //Function for 10us
void T100us_Process();        //Function for 100us
void T1ms_Process();          //Function for 1ms
void T10ms_Process();         //Function for 10ms
void T100ms_Process();        //Function for 100ms
void TXms_Process();          //Function for Xms,�û����޸�

void Seg7LedDisplay( uchar s, uchar e );  //��ʾ��������ܻ���ˮ��
void Seg7LedUpdate();                //����ʾ�����ݸ���

/*---------��������---------*/
//��ʱ������ʱ���־
bit btT10usFlag = 0;      //ʱ��10uS
bit btT100usFlag = 0;     //50����5ms����ͨ��ʱ��
bit btT1msFlag = 0;       //�����ɨ��Ƶ�ʽ���
bit btT10msFlag = 0;      //������������
bit btT100msFlag = 0;     //�������ʾ���ݸ���
bit btTXmsFlag = 0;       //4ms�¶ȹ��ղ����л�ģʽ

//��ʱ���ڲ�ר��ʱ�����
uchar ucT10usCnt = 0;    //ʱ��10uS
uchar ucT100usCnt = 0;   //ԭ�������������5ms ����ͨ��ʱ��
uchar ucT1msCnt = 0;
uchar ucT10msCnt = 0;
uchar ucT100msCnt = 0;

uchar ucClockXms = 0;     //Xms(�����Ǽ�40*100us)
uchar ucLedTmp = 0x00;    //��ˮ����ʾֵ

uchar ucDigtmp0 = 0;
uchar ucDigtmp1 = 0;
uchar ucDigtmp2 = 0;
uchar ucDigtmp3 = 0;
uchar ucDigtmp4 = 0;
uchar ucDigtmp5 = 0;
uchar ucDigtmp6 = 0;
uchar ucDigtmp7 = 0;
uchar ucDigtmp8 = 0;

uint uiSampleNum = 0;
uint uiState = 1;                      //״̬λ�����ֹ���¶�

uint uiPhoto = 0;                     //ִ�й�Ĵ���
uint uiTherm = 0;                     //ִ���¶ȵĴ���
ulint ulAdSumPhoto = 0;               //��ADֵ���ܺ�
ulint ulAdSumTherm = 0;              //�¶�ADֵ���ܺ�

uint uiThermTmp;
int intThermLookUp = 0;              //�¶�ֵ
uint uiThermAbs;                    //�¶Ⱦ���ֵ
uint uiPhotoTmp = 0;                 //��

//����������ʾ�¶ȵ���������
uint uiThermDig100 = 0;     //��λ
uint uiThermDig10 = 0;      //ʮλ
uint uiThermDig1 = 0;       //��λ

//����������ʾ�����������
uint uiPhotoDig100 = 0;    //��λ
uint uiPhotoDig10 = 0;     //ʮλ
uint uiPhotoDig1 = 0;      //��λ

//���������ʾ0-F
char arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                       0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
                      };
//�¶�ֵ��Ӧ��
int code arrThermLUT[] = {239, 197, 175, 160, 150, 142, 135, 129, 124, 120, 116, 113, 109, 107, 104, 101,
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
                          7, 7, 6, 6, 5, 5, 54, 4, 3, 3, 3, 2, 2, 1, 1, 1, 0, 0, -1, -1, -1,
                          -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9,
                          -10, -10, -11, -11, -12, -13, -13, -14, -14, -15, -16, -16, -17,
                          -18, -19, -19, -20, -21, -22, -23, -24, -25, -26, -27, -28, -29,
                          -30, -32, -33, -35, -36, -38, -40, -43, -46, -50, -55, -63, 361
                         };

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
            btT1msFlag = 0;   T1ms_Process();
        }
        if( btT10msFlag )
        {
            btT10msFlag = 0;  T10ms_Process();
        }
        if( btT100msFlag )
        {
            btT100msFlag = 0;  T100ms_Process();
        }
        if( btTXmsFlag  )    //Xms��־λ��Ч����4ms����
        {
            btTXmsFlag = 0;   TXms_Process();
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
        ucT10usCnt = 0;  btT100usFlag = 1;  ucT100usCnt++;
    }
    if( ucT100usCnt == 10 )
    {
        ucT100usCnt = 0;  btT1msFlag = 1;  ucT1msCnt++;
    }
    if( ucT1msCnt == 10 )
    {
        ucT1msCnt = 0;   btT10msFlag = 1;  ucT10msCnt++;
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
    P0M1 = 0x00;                 //����P0Ϊ����ģʽ�����������
    P0M0 = 0xff;
    P2M1 = 0x00;
    P2M0 = 0x08;                 //��P2^3����Ϊ����ģʽ������Ϊ׼˫���ģʽ

    //Interrupt Init
    /*10΢��@11.0592MHz 16������  ��ʱ��0     ���޸�
      ISP������ɣ�ԭ����1ms��11.592MHz 12Tģʽ */
    AUXR |= 0x80;   //��ʱ��ʱ��1Tģʽ
    TMOD &= 0xF0;   //���ö�ʱ��ģʽ
    TL0 = 0x91;     //���ö�ʱ��ֵ
    TH0 = 0xFF;     //���ö�ʱ��ֵ
    EA = 1;         //���ܵ��ж�
    ET0 = 1;        //������ʱ���ж�
    TF0 = 0;        //���TF0��־
    TR0 = 1;        //������ʱ��
}

/*---------��ʱ�ӽ��Ĵ�����--------*/
void T10us_Process()
{
}
void T100us_Process()
{
    ucClockXms++;
    if ( ucClockXms == ( cstClockXmsMaxNum ) )
    {
        ucClockXms = 0;  btTXmsFlag = 1;
    }
}
void T1ms_Process()             //1ms���������ɨ�����
{
    Seg7LedDisplay( cstDigBegin, cstDigEnd );
}
void T10ms_Process()           //10ms���ð���ɨ�����
{
}
void TXms_Process()             //Xms����,���޸�
{
    InitADC();
}
void T100ms_Process()          //100ms����������ʾ����
{
    Seg7LedUpdate();
}

/*---------������뷢���������ʾ����--------*/
void Seg7LedDisplay( uchar s, uchar  e )
{
    unsigned char arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40, 0x00}; //  �������ʾ�����
    /* ���:   0    1    2    3     4    5    6      7     8     9     10   11          */
    /* ��ʾ:   0    1    2    3     4    5    6      7     8     9     -   (��)      */
    static  int  i;
	P0 = 0x00;
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
            P0 = cstDig8;   break;
        default:
            P0 = 0x00;     break;
    }
    if( ++i > e ) i = s;
}

/*---------����ܼ���ˮ���ϵ���ʾ���ݸ��º���--------*/
void Seg7LedUpdate()
{
    if ( intThermLookUp < 0 )
        ucDigtmp0 = 10;
    else
        ucDigtmp0 = uiThermDig100; //����ܸ�λ����ʾ�����������,�û����޸�
    ucDigtmp1 = uiThermDig10;      //11�����ѡ����ʾ
    ucDigtmp2 = uiThermDig1;
    ucDigtmp3 = 11;
    ucDigtmp4 = 11;
    ucDigtmp5 = uiPhotoDig100;
    ucDigtmp6 = uiPhotoDig10;
    ucDigtmp7 = uiPhotoDig1;
    ucDigtmp8 = ucLedTmp;       //��ˮ��
}

/*---------��ʼ��ADC����--------*/
void InitADC()
{
    if( uiState == 1 )
    {
        InitAdcPhoto();        //��ʼ����
    }
    else
    {
        InitAdcTherm();         //��ʼ���¶�
    }
    uiState = -uiState;
}
void InitAdcTherm()             //��ʼ���¶�ADC
{
    P1ASF = 0xff;               //��P1����Ϊģ�⹦��A/Dʹ��
    ADC_RES = 0;               //�Ĵ���ADC_RES��ADC_RESL����A/Dת�����
    ADC_RESL = 0;              //��ʼ��ֵ0
    ADC_CONTR = 0x8b;         //ѡ��P1^3��ΪA/D����ʹ��
    CLK_DIV = 0x20;             //ADRJ=1���10λADC���
}
void InitAdcPhoto()              //��ʼ����ADC
{
    P1ASF = 0xff;
    ADC_RES = 0;
    ADC_RESL = 0;
    ADC_CONTR = 0x8c;           //CHS=100ѡ��P1^4��ΪA/D����ʹ��
    CLK_DIV = 0x20;
    EADC = 1;
}

/*---------ADC�ж��Ӻ���--------*/
void ADC_Process() interrupt 5 using 1
{
    uiSampleNum++;
    EA = 0;                          //�ر��ж�    
    if( uiSampleNum > cstSampleNum )  //ȡ���ֵ��ƽ��ֵ��С���
    {
        if( uiState == 1 )              //��ʱuiState=1ִ���¶Ȳ���
        {
            uiThermTmp = ( ulAdSumTherm + uiTherm / 2 ) / uiTherm; //��������
            intThermLookUp = arrThermLUT[uiThermTmp - 1]; //���ұ���AD���¶�ֵ
            ulAdSumTherm = 0;
            uiTherm = 0;
            uiSampleNum = 0;
            Date_ThermToDigit();
        }
        if( uiState == -1 )             //��ʱuiState=-1ִ�йⲿ��
        {
            uiPhotoTmp = ( ulAdSumPhoto + uiPhoto / 2 ) / uiPhoto;  //��������
            ulAdSumPhoto = 0;
            uiPhoto = 0;
            uiSampleNum = 0;
            Date_PhotoToDigit();
        }
    }

    if( uiState == 1 )
    {
        //��Ӧ�¶ȵ����ݴ���
        uiTherm++;
        uiThermTmp = ( ADC_RES * 256 + ADC_RESL ) / 4; //10λADֵת��Ϊ8λADֵ
        ulAdSumTherm += uiThermTmp;              //ADֵ�ĺ�
    }
    if( uiState == -1 )
    {
        //����ⲿ�ֵ�����
        uiPhoto++;
        ulAdSumPhoto += ADC_RES * 256 + ADC_RESL; // ADֵ�ĺ�
    }
    ADC_CONTR &= ~0X10;                  //ת����ɺ�ADC_FLAG����
    ADC_CONTR |= 0X08;                   //ת����ɺ�ADC_START��1
    EA = 1;
    EADC = 1; //���ж�
}

/*---------����ת���Ӻ���--------*/
//�ֱ�ȡ���¶Ⱥ͹��յİ�λ��ʮλ����λ
void Date_ThermToDigit()
{
    if( intThermLookUp < 0 )
        uiThermAbs = -intThermLookUp;
    else
        uiThermAbs = intThermLookUp;
    uiThermDig100 = uiThermAbs % 1000 / 100;
    uiThermDig10 = uiThermAbs % 100 / 10;
    uiThermDig1 = uiThermAbs % 10;
}
void Date_PhotoToDigit()
{
    uiPhotoDig100 = uiPhotoTmp % 1000 / 100;
    uiPhotoDig10 = uiPhotoTmp % 100 / 10;
    uiPhotoDig1 = uiPhotoTmp % 10;
}