/**********************
myNavKey 导航按键+数字按键控制数码管
型号:STC15F2K60S2 主频:11.0592MHz
************************/
#include <STC15F2K60S2.H>
#include <intrins.h>
#define uint  unsigned int
#define uchar unsigned char

/*---------宏定义---------*/
#define cstAdcPower 0X80     /*ADC电源开关*/
#define cstAdcFlag 0X10      /*当A/D转换完成后，cstAdcFlag要软件清零*/
#define cstAdcStart 0X08     /*当A/D转换完成后，cstAdcStart会自动清零，所以要开始下一次转换，则需要置位*/
#define cstAdcSpeed90 0X60   /*ADC转换速度 90个时钟周期转换一次*/
#define cstAdcChs17 0X07     /*选择P1.7作为A/D输入*/

/*---------引脚别名定义---------*/
sbit sbtLedSel = P2 ^ 3;     /*数码管和发光二极管选择位*/
sbit sbtSel0 = P2 ^ 0;
sbit sbtSel1 = P2 ^ 1;
sbit sbtSel2 = P2 ^ 2;
sbit sbtKey1 = P3 ^ 2;
sbit sbtKey2 = P3 ^ 3;

/*---------变量定义---------*/
uchar ucSegSelectState;         /*段选标志*/
uchar ucDigSelectState;         /*位选标志*/
bit btKey3Flag;             /*key3键按下标志*/
//0123456789ABCDEF
uchar arrSegSelect[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x00};
//选择哪一位数码管
uchar arrDigSelect[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};

/*---------初始化函数--------*/
void Init()
{
    P0M1 = 0x00;
    P0M0 = 0xff;
    P2M1 = 0x00;
    P2M0 = 0xff;

    sbtLedSel = 0;      //选择数码管作为输出
    P1ASF = 0x80;       //P1.7作为模拟功能A/D使用
    ADC_RES = 0;        //转换结果清零
    ADC_CONTR = 0x8F;   //cstAdcPower = 1
    CLK_DIV = 0X00;     //ADRJ = 0    ADC_RES存放高八位结果

    btKey3Flag = 0;
    ucDigSelectState = 0;
    ucSegSelectState = 0;

    IT0 = 0;            //设置IT0上升沿触发
    IT1 = 0;
    EA = 1;             //CPU开放中断
}

/*---------延时5ms子函数--------*/
void Delay5ms()     //@11.0592MHz  延时5ms
{
    unsigned char i, j;
    i = 54;
    j = 199;
    do
    {
        while ( --j );
    }
    while ( --i );
}

/*---------延时100ms子函数--------*/
void Delay100ms()       //@11.0592MHz  延时100ms
{
    unsigned char i, j, k;
    _nop_();
    _nop_();
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

/*---------获取AD值子函数--------*/
unsigned char GetADC()
{
    uchar ucAdcRes;
    ADC_CONTR = cstAdcPower | cstAdcStart | cstAdcSpeed90 | cstAdcChs17;//没有将cstAdcFlag置1，用于判断A/D是否结束
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    while( !( ADC_CONTR & cstAdcFlag ) ); //等待直到A/D转换结束
    ADC_CONTR &= ~cstAdcFlag;           //cstAdcFlagE软件清0
    ucAdcRes = ADC_RES;                 //获取AD的值
    return ucAdcRes;
}

/*---------获取导航按键值子函数--------*/
uchar NavKeyCheck()
{
    unsigned char key;
    key = GetADC();     //获取AD的值
    if( key != 255 )    //有按键按下时
    {
        Delay5ms();
        key = GetADC();
        if( key != 255 )            //按键消抖 仍有按键按下
        {
            key = key & 0xE0;       //获取高3位，其他位清零
            key = _cror_( key, 5 ); //循环右移5位 获取A/D转换高三位值，减小误差
            return key;
        }
    }
    return 0x07;        //没有按键按下时返回值0x07
}

/*---------导航按键处理子函数--------*/
void NavKey_Process()
{
    uchar ucNavKeyCurrent;  //导航按键当前的状态
    uchar ucNavKeyPast;     //导航按键前一个状态

    ucNavKeyCurrent = NavKeyCheck();    //获取当前ADC值
    if( ucNavKeyCurrent != 0x07 )       //导航按键是否被按下 不等于0x07表示有按下
    {
        ucNavKeyPast = ucNavKeyCurrent;
        while( ucNavKeyCurrent != 0x07 )        //等待导航按键松开
            ucNavKeyCurrent = NavKeyCheck();

        switch( ucNavKeyPast )
        {
            case 0x00 :                     //K3
                if( btKey3Flag == 0 )
                {
                    btKey3Flag = 1;
                }
                else
                {
                    btKey3Flag = 0;
                }
                break;
            case 0x05 :                     //上键：显示的数字加1
                if( ucSegSelectState == 15 )
                {
                    ucSegSelectState = 0;
                }
                else
                    ucSegSelectState++;
                break;
            case 0x02 :                     //下键：显示的数字减1
                if( ucSegSelectState == 0 )
                    ucSegSelectState = 15;
                else
                    ucSegSelectState--;
                break;
        }
    }

    Delay100ms();
}

/*---------主函数--------*/
void main()
{
    Init();
    P0 = 0x00;
    while( 1 )
    {
        NavKey_Process();         //获取按键按下情况
        P2 = arrDigSelect[ucDigSelectState];           //显示位
        P0 = arrSegSelect[ucSegSelectState];       //显示数字
        if( btKey3Flag == 0 )   //KEY3=0则K1,K2键解锁，否则K1,K2键锁死
        {
            if( sbtKey1 == 0 )    //sbtKey1按下，显示位右移一位
            {
                Delay5ms();
                if( sbtKey1 == 0 )
                {
                    while( !sbtKey1 );

                    if( ucDigSelectState == 7 )
                        ucDigSelectState = 0;
                    else
                        ucDigSelectState++;
                }
            }
            if( sbtKey2 == 0 )   //sbtKey2按下，显示位左移一位
            {
                Delay5ms();
                if( sbtKey2 == 0 )
                {
                    while( !sbtKey2 );
                    if( ucDigSelectState == 0 )
                        ucDigSelectState = 7;
                    else
                        ucDigSelectState--;
                }
            }
        }
    }
}
