/**********************
myIr1  红外通信1，单字节
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------宏定义---------*/
#define cstKeyCheckTime 75          //按键消抖的周期
#define cstKeyMinTime 50            //按键识别被为按下时需检测到的最少次数

/*---------引脚别名定义---------*/
sbit sbtSel0 = P2 ^ 0;                              //位选信号
sbit sbtSel1 = P2 ^ 1;                              //位选信号
sbit sbtSel2 = P2 ^ 2;                              //位选信号
sbit sbtLedSel = P2 ^ 3;                            //数码管和LED的选择信号

sbit sbtPutIr = P3 ^ 5;                             //红外线发送引脚
sbit sbtGetIr = P3 ^ 7;                             //串口1发送引脚

sbit sbtKey1 = P3 ^ 2;                              //按键1  发送信号
sbit sbtKey2 = P3 ^ 3;                              //按键2  数据加1
sbit sbtKey3 = P1 ^ 7;                              //按键3  收发模式开关

/*---------变量定义---------*/
int time = cstKeyCheckTime;                 //按键消抖周计数(一个周期检测cstKeyCheckTime次)
int intKey1Cnt = 0;                         //周期中检测到sbtKey1=0的次数
int intKey2Cnt = 0;                         //周期中检测到sbtKey2=0的次数
int intKey3Cnt = 0;                         //周期中检测到sbtKey3=0的次数
int intKey1State = 1;       //sbtKey1状态：设置为0代表已按下，1代表未按下
int intKey2State = 1;       //sbtKey2状态：设置为0代表已按下，1代表未按下
int intKey3State = 1;       //sbtKey3状态：设置维0代表已按下，1代表未按下

uint  uiSeg7Num = 0;              //数码管显示的数字
uchar ucPutIrFlagN = 1;             //状态标记：0代表可发送，1和2代表不可发送
int intMyBuf = 11;                 //接收缓冲，用于收发标志0xca的判断
uchar arrSegSelect[] =              //段选，显示0-f
{
    0x3f, 0x06, 0x5b, 0x4f,
    0x66, 0x6d, 0x7d, 0x07,
    0x7f, 0x6f, 0x77, 0x7c,
    0x39, 0x5e, 0x79, 0x71
};

/*---------函数声明---------*/
//中断函数除外
void TimerInit();      //定时器设置
void Uart1Init();      //串口1设置
void Init();          //初始化，推挽，设置中断开关
void DigSelct( int );   //设置数码管位选
void CheckKey();     //消抖检测以及按键操作(定时器0中断中调用)

/*---------主函数---------*/
void main()
{
    TimerInit();        //设置定时器
    Uart1Init();            //设置串口1
    Init();                 //初始化
    while( 1 );
}

/*---------初始化函数---------*/
//设置推挽，中断开关设置
void Init()
{
    P0M1 = 0x00;
    P0M0 = 0xff;
    P2M1 = 0x00;
    P2M0 = 0x08;
    //中断开关设置，详见数据手册P458
    ET1 = 0;            //禁止T1中断
    ET0 = 1;            //打开定时器T0中断
    ES = 1;             //打开串口1中断
    IE2 = 0X04;         //打开定时器2中断
    EA = 1;             //打开总中断
}

/*---------串口1初始化函数---------*/
void Uart1Init()    //设置方法见数据手册P621(串口1设置)和P498(定时器T1设置)
{
    PCON &= 0x7F;       //波特率不倍速，SMOD=0
    SCON = 0x50;        //串口1使用工作方式1，REN=1(允许串行接收)
    AUXR &= 0xFE;       //串口1选择定时器T1作为波特率发生器，S1ST2=0
    AUXR1 = 0x40;       //串口1在P3.6接收，在P3.7发送
    PS = 1;             //设置串口中断为最高优先级
}

/*---------定时器初始化函数---------*/
void TimerInit()    //设置方法见数据手册P498
{
    AUXR |= 0x40;       //定时器T1为1T模式，速度是传统8051的12倍，不分频。
    TMOD &= 0x0F;     //清除T1模式位
    TMOD |= 0x20;      //设置T1模式位，使用8位自动重装模式
    TL1 = 0x70;         //设置初值
    TH1 = 0x70;         //设置T1重装值
    TR1 = 1;            //T1运行控制位置1，允许T1计数

    AUXR |= 0x80;       //定时器T0为1T模式，的速度是传统8051的12倍，不分频。
    TMOD &= 0xF0;       //清除T0模式位
    TMOD |= 0x02;       //设置T0模式位，使用8位自动重装模式
    TL0 = 0x70;          //设置初值
    TH0 = 0x70;         //设T0重装值
    TF0 = 0;             //T0溢出标志位清零
    TR0 = 1;             //T0运行控制位置1，允许T0计数

    //定时器T2用于显示和按键消抖，500us定时16位自动重装
    AUXR |= 0x04;       //定时器T2为1T模式
    T2L = 0x66;         //低位重装值
    T2H = 0xEA;         //高位重装值
    AUXR |= 0x10;       //定时器2开始计时
}

/*---------数码管显示位的选择函数---------*/
void DigSelct( int x )
{
    sbtSel0 = x % 2;
    sbtSel1 = x % 4 / 2;
    sbtSel2 = x / 4;
}

/*---------按键检测函数---------*/
//消抖周期中一次按键的检测，以及检测cstKeyCheckTime次后的操作（需要多次重复调用）
void CheckKey()
{
    time--;
    if( sbtKey1 == 0 )
        intKey1Cnt++;
    if( sbtKey2 == 0 )
        intKey2Cnt++;
    if( sbtKey3 == 0 )
        intKey3Cnt++;
    if( time <= 0 )                                 //一个周期结束
    {
        if( intKey1Cnt >= cstKeyMinTime )   //判断sbtKey1被检测为按下的次数是否大于按键识别被为按下需检测到的最少次数
        {
            if( intKey1State == 1 )             //判断是否已经被按下
            {
                if( ucPutIrFlagN == 0 )         //判断是否可以发送
                {
                    ucPutIrFlagN = 1;         //第一次先发送一个标志  PS：如果不增加这个标志，在日光灯下来回遮挡接收头可能会接收到干扰信号
                    SBUF = 0xca;             //标志为0xca，发送完后在串口中断中再发送数据
                }
                intKey1State = 0;               //状态改变为已经被按下
            }
        }
        else
            intKey1State = 1;                   //状态改变为未被按下

        if( intKey2Cnt >= cstKeyMinTime )
        {
            if( intKey2State == 1 )
            {
                uiSeg7Num++;                  //数据加1
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
                ucPutIrFlagN = !ucPutIrFlagN;       //能否发送的切换
                REN = ~REN;                    //接收允许标志位，1代表可接收，0代表不可接收
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

/*---------定时器0中断服务函数---------*/
void T0_Process() interrupt 1
{
    if( sbtGetIr == 0 )     //sbtPutIr根据sbtGetIr的信号产生脉冲
    {
        sbtPutIr = ~sbtPutIr;
    }
    else                        //如果P3.7=1则P3.5输出0
        sbtPutIr = 0;
}

/*---------串口1中断服务函数---------*/
//发送完毕TI自动置1，产生中断; 接收完毕RI值1，产生中断
void Uart1_Process() interrupt 4
{
    if( TI )                        //判断是否发送中断
    {
        TI = 0;                     //发送中断请求标志位清0
        if( ucPutIrFlagN == 1 )         //判断是否第一次发送
        {
            ucPutIrFlagN = 2;        //第二次发送数据
            SBUF = uiSeg7Num;       //发送uiSeg7Num
        }
        if( ucPutIrFlagN == 2 )         //判断是否第二次发送完毕
            ucPutIrFlagN = 0;         //发送完ucPutIrFlagN清零
    }
    if( RI )                         //判断是否接收中断
    {
        RI = 0;                     //接收中断请求标志位清0
        if( intMyBuf == 0xca )          //判断上一次是否收到0xca标志
            uiSeg7Num = SBUF;       //正式接收数据
        intMyBuf = SBUF;            //把这次接收到的数据存入自定义的缓存中，等待下一次的比较
    }
}

/*---------定时器2中断服务函数---------*/
void T2_Process() interrupt 12
{

    P0 = 0;                              //P0清零
    sbtLedSel = ~sbtLedSel;                 //切换显示
    if( sbtLedSel == 0 )
    {
        DigSelct( 7 );                      //选择数码管的第七位
        P0 = arrSegSelect[uiSeg7Num];       //设置数码管显示内容
    }
    else
        P0 = !ucPutIrFlagN;                //设置发光的LED灯
    CheckKey();                         //按键消抖检测
}
