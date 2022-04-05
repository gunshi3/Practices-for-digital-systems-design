/**********************
myBeep2 可变调蜂的鸣器
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#define uint  unsigned int
#define uchar unsigned char

/*---------引脚别名定义---------*/
sbit sbtBeep = P3 ^ 4;                  //蜂鸣器引脚
sbit sbtKey1 = P3 ^ 2;                  //按键1引脚
sbit sbtKey2 = P3 ^ 3;                  //按键2引脚
sbit sbtSel0 = P2 ^ 0;                  //位选信号位
sbit sbtSel1 = P2 ^ 1;                  //位选信号位
sbit sbtSel2 = P2 ^ 2;                  //位选信号位
sbit sbtSel3 = P2 ^ 3;                  //LED与数码管显示的控制位

/*---------变量定义---------*/
uint sbtKey1_state = 0;         //0：Key1未按下         1：Key1已按下
uint sbtKey2_state = 0;         //0：Key2未按下         1：Key1已按下
bit  btBeepFlag;                //控制蜂鸣器开关的标志位
uint uiToneNum = 0;             //音调
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
                     0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
                       }; //段选0-f

/*---------初始化函数--------*/
void Init()
{
    P0M0 = 0xff;
    P0M1 = 0x00;
    P2M0 = 0x08;
    P2M1 = 0x00;
    //设置P3^4为推挽模式
    P3M1 = 0x00;
    P3M0 = 0x10;

    AUXR |= 0x80;                       //定时器时钟1T模式
    TMOD &= 0xF0;                       //设置定时器模式为16位自动重装
    TL0 = 0xCD;                         //设置定时初值
    TH0 = 0xF4;                         //设置定时初值
    TF0 = 0;                            //清除TF0标志
    TR0 = 1;                            //定时器0开始计时

    btBeepFlag = 0;
    P0 = 0x00;                          //数码管和LED显示清零

    sbtSel0 = 1;                         //位选设置为第七位
    sbtSel1 = 1;
    sbtSel2 = 1;

    sbtBeep = 0;                        //蜂鸣器引脚置0，以保护蜂鸣器
    ET0 = 1;
    EA = 1;
}

/*---------延时子函数--------*/
void DelayMs( uint xms )
{
    uchar i;
    for( ; xms > 0; xms-- )
        for( i = 114; i > 0; i-- )
        {
            ;
        }
}

/*---------显示子函数--------*/
void DisplaySeg7Led()
{
    P0 = 0;
    sbtSel3 = 0;
    P0 = arrSegSelect[uiToneNum];
    DelayMs( 1 );

    P0 = 0;
    sbtSel3 = 1;
    P0 = 0x08;
    DelayMs( 1 );
}

/*---------主函数--------*/
void main()
{
    Init();
    while( 1 )
    {
        if( sbtKey1 == 0 )
        {
            if( sbtKey1_state == 0 )           //判断按键1是否按下
            {
                DelayMs( 10 );              //延时消除抖动
                if( sbtKey1 == 0 )
                {
                    uiToneNum++;            //声调改变
                    if( uiToneNum == 10 )
                        uiToneNum = 0;
                    TH0 = 0xF4 - uiToneNum; //减小重装值，从而减小
//定时器中断（蜂鸣器振动）频率
                    sbtKey1_state = 1;      //设置按键1为已按下
                }
            }
        }
        else
            sbtKey1_state = 0;

        if( sbtKey2 == 0 )
        {
            if( sbtKey2_state == 0 )            //判断按键2是否按下 
            {
                DelayMs( 10 );               //延时消除抖动
                if( sbtKey2 == 0 )
                {
                    btBeepFlag = !btBeepFlag;   //蜂鸣器开关切换
                    sbtKey2_state = 1;          //设置按键1为已按下
                }
            }
        }
        else
            sbtKey2_state = 0;

        DisplaySeg7Led();
    }
}

/*---------T0定时器中断服务处理函数--------*/
void T0_Process() interrupt 1
{
    if( btBeepFlag )
    {
        sbtBeep = ~sbtBeep;            //产生方波使得蜂鸣器发声
    }
    else
        sbtBeep = 0;                  //如果开关关闭,则蜂鸣器断电以保护蜂鸣器

}
