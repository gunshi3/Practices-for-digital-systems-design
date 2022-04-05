/**********************
STC-B_DEMO.c for 可控步进电机(正反转暂停)
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------宏定义---------*/
#define cstDigBegin   7         // 数码管显示位选起始,用户可修改
#define cstDigEnd     7         // 数码管显示位选终止位。8为发光二极管,大于8 可用于调节显示量度

#define cstDig0       11        //数码管各位上显示内容数组序号,用户可修改
#define cstDig1       11        //11代表段选无显示  
#define cstDig2       11
#define cstDig3       11
#define cstDig4       11
#define cstDig5       11
#define cstDig6       11
#define cstDig7       ucSpeed_Seg  //P0口显示步进电机速度，范围1-4,1档速度最快
#define cstDig8       ucLedTmp         //流水灯

#define cstKeyMaxNum      10                            //10次读取按键值
#define cstKeyCntMaxNum   (0x80 + cstKeyMaxNum / 3 * 2)  //按键消抖计数初值

#define cstSpeedMaxLevel  5   //上限范围，原代码4档速度，１档最快，
#define cstSpeed100usCnt  50  //原代码设想5ms*ucSpeed,本代码放在100us节拍处理
#define cstClockXmsMaxNum ((ucSpeed)*(cstSpeed100usCnt)) //Xms时钟

/*---------函数声明---------*/
void Init();                  //Initial
void T10us_Process();         //Function for 10us
void T100us_Process();        //Function for 100us
void T1ms_Process();          //Function for 1ms
void T10ms_Process();         //Function for 10ms
void T100ms_Process();        //Function for 100ms
void TXms_Process();          //Function for Xms,用户可修改

void Seg7LedDisplay( uchar s, uchar e );  //显示单个数码管或流水灯
void Key_Scan();                          //按键扫描检测
void Xstep_Process();                     //步进电机正反转暂停控制

/*---------引脚别名定义---------*/
sbit sbtKey1 = P3 ^ 2;            //按键1,根据学习板,用户可修改
sbit sbtKey2 = P3 ^ 3;            //按键2
sbit sbtKey3 = P1 ^ 7;            //按键3
sbit sbtS1  = P4 ^ 1;             //步进电机相1
sbit sbtS2  = P4 ^ 2;             //步进电机相2
sbit sbtS3  = P4 ^ 3;             //步进电机相3
sbit sbtS4  = P4 ^ 4;             //步进电机相4

/*---------变量定义---------*/
//定时器计数时间标志
bit btT10usFlag = 0;      //时基10uS
bit btT100usFlag = 0;     //50个＝5ms单相通电时间
bit btT1msFlag = 0;       //数码管扫描频率节拍
bit btT10msFlag = 0;      //按键消抖节拍
bit btT100msFlag = 0;     //数码管显示数据更新
bit btTXmsFlag = 0;       //步进电机输出 ucSpeed*1mS
bit btTXmsFlag_en = 1;    //步进电机启动开关,默认开启(0暂停1启动)

//定时器内部专用时间计数
uchar ucT10usCnt = 0;    //时基10uS
uchar ucT100usCnt = 0;   //原代码里设想的是5ms 单相通电时间
uchar ucT1msCnt = 0;
uchar ucT10msCnt = 0;
uchar ucT100msCnt = 0;

// Fun()用户功能实现所用变量
uchar ucKey1Cnt = 0;    //按键1消抖计数器
uchar ucKey2Cnt = 0;    //按键2消抖计数器
uchar ucKey3Cnt = 0;    //按键3消抖计数器
uchar ucKeyAllCnt = 0;   //按键消抖总延时计数器

uchar ucSpeed_Seg = 1;    //速度显示序号
uchar ucSpeed = 1;        //步进电机的速度参数

bit  btKey1Current = 1;   //按键1当前的状态
bit  btKey1Past = 1;      //按键1前一个状态
bit  btKey2Current = 1;   //按键2当前的状态
bit  btKey2Past = 1;      //按键2前一个状态
bit  btKey3Current = 1;   //按键3当前的状态
bit  btKey3Past = 1;      //按键3前一个状态
bit  btRotationFlag = 0;   //步进电机旋转方向0正转1反转

uchar ucClockXms = 0;     //步进电机产生信号控制正反转暂停所用计数器
uchar ucLedTmp = 0x00;    //流水灯显示值

/*---------主函数---------*/
void main()
{
    Init();
    ucLedTmp = 0x00;            //流水灯显示值
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
        if( ( btTXmsFlag && btTXmsFlag_en ) )  //Xms标志位且使能位有效
//才执行步进电机转动
        {
            btTXmsFlag = 0;  TXms_Process();
        }
    }
}

/*---------T0定时器中断服务处理函数--------*/
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
/*---------初始化函数--------*/
void Init()
{
    //Hardware Init
    P0M0 = 0xFF;
    P0M1 = 0x00;
    P0 = 0x00;
    P2M0 = 0x0f; //设置P2.0-3为推挽:对应了LED的控制
    P2M1 = 0x00;
    P4M0 = 0xff;    //P4口设置推挽输出
    P4M1 = 0xe1;
    P4 = 0xe1;      //设置P4口为低电平，避免复位时步进电机5V电平接口的影响

    //Interrupt Init
    /*10微秒@11.0592MHz 16可重载  定时器0
      ISP软件生成，原来是1ms在11.592MHz 12T模式 */
    AUXR |= 0x80;   //定时器时钟1T模式
    TMOD &= 0xF0;   //设置定时器模式
    TL0 = 0x91;     //设置定时初值
    TH0 = 0xFF;     //设置定时初值
    EA = 1;         //打开总的中断
    ET0 = 1;        //开启定时器中断
    TF0 = 0;        //清除TF0标志
    TR0 = 1;        //启动定时器

    ucKey1Cnt = cstKeyCntMaxNum;
    ucKey2Cnt = cstKeyCntMaxNum;
    ucKey3Cnt = cstKeyCntMaxNum;
    ucKeyAllCnt = cstKeyMaxNum;
}

/*---------各时钟节拍处理函数--------*/
void T10us_Process()
{

}
void T100us_Process()           //100us适用步进电机每拍单相计数,计满产生标志
{
    ucClockXms++;
    if ( ucClockXms == ( cstClockXmsMaxNum ) )
    {
        ucClockXms = 0;   btTXmsFlag = 1;
    }
}
void T1ms_Process()             //1ms适用数码管扫描服务
{
    Seg7LedDisplay( cstDigBegin, cstDigEnd );
}
void T10ms_Process()           //10ms适用按键扫描服务
{
    Key_Scan();
}
void TXms_Process()            //Xms适用步进电机服务
{
    Xstep_Process();
}
void T100ms_Process()          //100ms适用数据显示更新
{
    ucSpeed_Seg = ucSpeed;
}

/*---------数码管与发光二极管显示函数--------*/
void Seg7LedDisplay( uchar s, uchar  e )
{
    unsigned char arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x40, 0x00}; //  数码管显示译码表
    /* 序号:   0    1    2    3     4    5    6      7     8     9     10   11          */
    /* 显示:   0    1    2    3     4    5    6      7     8     9     -   (无)      */
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

/*---------按键扫描检测函数--------*/
void Key_Scan()
{
    if( sbtKey1 == 0 )                          //按键按下时，按键计数器减1
        ucKey1Cnt--;
    if( sbtKey2 == 0 )
        ucKey2Cnt--;
    if( sbtKey3 == 0 )
        ucKey3Cnt--;
    ucKeyAllCnt--;                           //总的次数减1

    if( ucKeyAllCnt == 0 )                      //10次完了
    {
        if( ucKey2Cnt < 0x80 )
        {
            btKey2Current = 0;
            if( btKey2Past == 1 )
            {
                btKey2Past = 0;                 //下降沿
                ucSpeed++;     //步进电机的转速改变，ucSpeed值越大，转速越慢
                if( ucSpeed == cstSpeedMaxLevel )
                    ucSpeed = 1;
            }
        }
        if( ucKey2Cnt >= 0x80 )
        {
            btKey2Current = 1;
            if( btKey2Past == 0 )
                btKey2Past = 1;                //上升沿（假设不做动作那就继续）
        }
        if( ucKey1Cnt < 0x80 )
        {
            btKey1Current = 0;
            if( btKey1Past == 1 )
            {
                btKey1Past = 0;               //下降沿（按键改变步进电机转向）
                btRotationFlag = ~btRotationFlag;
            }
        }
        if( ucKey1Cnt >= 0x80 )
        {
            btKey1Current = 1;
            if( btKey1Past == 0 )
                btKey1Past = 1;                //上升沿（假设不做动作那就继续）
        }
        if( ucKey3Cnt < 0x80 )
        {
            btKey3Current = 0;
            if( btKey3Past == 1 )
            {
                btKey3Past = 0;                  //下降沿
                btTXmsFlag_en = ~btTXmsFlag_en;  //按键改变暂停或开启电机
            }
        }
        if( ucKey3Cnt >= 0x80 )
        {
            btKey3Current = 1;
            if( btKey3Past == 0 )
                btKey3Past = 1;                //上升沿（假设不做动作那就继续）
        }
        ucKey1Cnt = cstKeyCntMaxNum;          //新一轮判断装初值
        ucKey2Cnt = cstKeyCntMaxNum;
        ucKey3Cnt = cstKeyCntMaxNum;
        ucKeyAllCnt = cstKeyMaxNum;
    }
}

/*---------步进电机正反转暂停控制函数--------*/
void Xstep_Process()
{
    static uchar i = 1;                      //步进电机控制脉冲对应的IO口位置
    if( btRotationFlag == 0 )                 //控制步进电机正转
    {
        switch( i++ )                      //循环改变不同IO脚的脉冲电平
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
    }                                      //控制步进电机反转
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
