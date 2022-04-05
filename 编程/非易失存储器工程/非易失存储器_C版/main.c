#include <STC15F2K60S2.h>
#define uint unsigned int
#define uchar unsigned char
#define FOSC 11059200L  //晶振频率
#define T_ms 0.1        //定时时间为0.1ms
#define NMAX_KEY 10
/*位声明*/
sbit Key1 = P3 ^ 2;     //按下key1，向存储器写入数据并读取该地址的数据，显示在数码管上。
sbit Key2 = P3 ^ 3;     //按下key2，要写入的数据加1
sbit Key3 = P1 ^ 7;     //按下key3，要写入的地址加1
sbit led = P2 ^ 3;      //LED灯与数码管切换引脚
sbit SDA = P4 ^ 0;      //I2C总线的数据线
sbit SCL = P5 ^ 5;      //I2C总线的时钟线
sbit Beep = P3 ^ 4;
/*变量定义*/
uchar duan[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71}; //数码管段选，显示0-f
uchar wei[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}; //数码管位选
uchar flag1;                //数码管循环扫描变量
uchar write_addr;           //写入地址
uchar write_date;           //写入数据
uchar read_date;            //读出数据
/*按键计数消抖变量*/
uchar G_count;              //定时器0中断计数值
uint Key1_count;            //KEY1键在1ms内达到低电平的次数
uint Key2_count;            //KEY2键在1ms内达到低电平的次数
uint Key3_count;            //KEY3键在1ms内达到低电平的次数
uint Key_count;             //由100递减到0
bit flg_1ms;                //表示1ms时间到
bit Key1_C;/*key1当前的状态*/
bit Key1_P;/*key1前一个状态*/
bit Key2_C;/*key2当前的状态*/
bit Key2_P;/*key2前一个状态*/
bit Key3_C;/*key3当前的状态*/
bit Key3_P;/*key3前一个状态*/

void SMG1( uchar date1, uchar date2, uchar date3 ) //数码管显示函数
{
    flag1++;
    if( flag1 == 8 )
    {
        flag1 = 0;
    }
    P0 = 0x00;
    P2 = wei[flag1];
    switch( flag1 )
    {
        case 0:
            P0 = duan[date1 / 16];   break;
        case 1:
            P0 = duan[date1 % 16];   break;
        case 2:
            P0 = 0x40;   break;
        case 3:
            P0 = duan[date2 / 16];   break;
        case 4:
            P0 = duan[date2 % 16];   break;
        case 5:
            P0 = 0x40;   break;
        case 6:
            P0 = duan[date3 / 16];    break;
        default:
            P0 = duan[date3 % 16];   break;
    }
}

void KEY_init() //按键消抖模块初始化
{
    G_count = 0;
    flg_1ms = 0;
    Key1_C = 1; /*key1当前的状态*/
    Key1_P = 1; /*key1前一个状态*/
    Key2_C = 1; /*key2当前的状态*/
    Key2_P = 1; /*key2前一个状态*/
    Key3_C = 1; /*key3当前的状态*/
    Key3_P = 1; /*key3前一个状态*/
    Key1_count = 0x80 + NMAX_KEY / 3 * 2;
    Key2_count = 0x80 + NMAX_KEY / 3 * 2;
    Key3_count = 0x80 + NMAX_KEY / 3 * 2;
    Key_count = NMAX_KEY;
}

void delay()        //延时4us
{
    ;;
}

void IIC_init()     //I2C总线初始化
{
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}
void start()        //主机启动信号
{
    SDA = 1;
    delay();
    SCL = 1;
    delay();
    SDA = 0;
    delay();
}
void stop()         //停止信号
{
    SDA = 0;
    delay();
    SCL = 1;
    delay();
    SDA = 1;
    delay();
}
void respons()      //从机应答信号
{
    uchar i = 0;
    SCL = 1;
    delay();
    while( SDA == 1 && ( i < 255 ) ) //表示若在一段时间内没有收到从器件的应答则
        i++;                //主器件默认从期间已经收到数据而不再等待应答信号。
    SCL = 0;
    delay();
}
void writebyte( uchar date ) //对24C02写一个字节数据
{
    uchar i, temp;
    temp = date;
    for( i = 0; i < 8; i++ )
    {
        temp = temp << 1;
        SCL = 0;
        delay();
        SDA = CY;
        delay();
        SCL = 1;
        delay();
    }
    SCL = 0;
    delay();
    SDA = 1;
    delay();
}
uchar readbyte()            //从24C02读一个字节数据
{
    uchar i, k;
    SCL = 0;
    delay();
    SDA = 1;
    delay();
    for( i = 0; i < 8; i++ )
    {
        SCL = 1;
        delay();
        k = ( k << 1 ) | SDA;
        delay();
        SCL = 0;
        delay();
    }
    delay();
    return k;
}
void write_add( uchar addr, uchar date ) //对24C02的地址addr，写入一个数据date
{
    start();
    writebyte( 0xa0 );
    respons();
    writebyte( addr );
    respons();
    writebyte( date );
    respons();
    stop();
}
uchar read_add( uchar addr )      //从24C02的addr地址，读一个字节数据
{
    uchar date;
    start();
    writebyte( 0xa0 );
    respons();
    writebyte( addr );
    respons();
    start();
    writebyte( 0xa1 );
    respons();
    date = readbyte();
    stop();
    return date;
}

void IO_init()       //IO口初始化，变量初始化
{
    P2M1 = 0x00;    //设置P0口和P2^3推挽输出
    P2M0 = 0x08;
    P0M1 = 0x00;
    P0M0 = 0xff;    //按键无需推挽
    led = 0;        //关LED显示
    P0 = 0x00;
	Beep=0;            //确保蜂鸣器关
    write_addr = 0x00;      //写入地址初始化
    write_date = 0x00;      //写入数据初始化
}

void Timer0_Init()       //计时器0初始化
{
    TMOD = 0x00;        //计时器0工作方式0，16位自动重装计数
    AUXR = 0x80;        //1T模式,T0x12=1,
    EA = 1;             //开总中断
    ET0 = 1;            //开定时器0中断
    TH0 = ( 65536 - T_ms * FOSC / 1000 ) / 256;  //给定时器赋初值
    TL0 = ( 65536 - T_ms * FOSC / 1000 );  
    TR0 = 1;                              //启动定时器
}

void Timer_T0() interrupt 1  //定时器0中断函数
{
    G_count++;
    if( G_count == 10 )
    {
        flg_1ms = 1;       //1ms时间到，fla_1ms=1
        G_count = 0x00;
    }
    SMG1( write_addr, write_date, read_date ); //在定时器中短中调用数码管显示函数
}

int main()                 //主函数
{
    IO_init();            //IO口初始化
    Timer0_Init();        //定时器0初始化
    KEY_init();           //按键消抖模块初始化
    IIC_init();            //IIC总线初始化
    while( 1 )
    {
        while( flg_1ms )  //1ms时间到 判断按键状态
        {
            flg_1ms = 0;
            read_date = read_add( write_addr ); //读出地址为write_addr的数据
            if( Key1 == 0 )
                Key1_count--;
            if( Key2 == 0 )
                Key2_count--;
            if( Key3 == 0 )           //按键是按下状态
                Key3_count--;
            Key_count--;           //总的次数减1
            if( Key_count == 0 )      //10次完了
            {
                if( Key1_count < 0x80 )
                {
                    Key1_C = 0;
                    if( Key1_P == 1 )       //下降沿（按键做动作）
                    {
                        Key1_P = 0;
                        write_add( write_addr, write_date ); //向地址write_addr写入数据
                    }
                }
                if( Key1_count >= 0x80 )
                {
                    Key1_C = 1;
                    if( Key1_P == 0 )
                        Key1_P = 1;          //上升沿（假设不做动作那就继续）
                }
                if( Key2_count < 0x80 )
                {
                    Key2_C = 0;
                    if( Key2_P == 1 )         //下降沿（按键做动作）
                    {
                        Key2_P = 0;
                        write_date++;           //按键数据加1
                        if( write_date == 0xff ) //假如输入的数据大于0xff，则变为0x00。
                            write_date = 0x00;
                    }
                }
                if( Key2_count >= 0x80 )
                {
                    Key2_C = 1;
                    if( Key2_P == 0 )
                        Key2_P = 1;  //上升沿（假设不做动作那就继续）
                }
                if( Key3_count < 0x80 )
                {
                    Key3_C = 0;
                    if( Key3_P == 1 ) //下降沿（按键做动作）
                    {
                        Key3_P = 0;
                        write_addr++;           //按键写入地址+1
                        if( write_addr == 0xff )
                            write_addr = 0;
                    }
                }
                if( Key3_count >= 0x80 )
                {
                    Key3_C = 1;
                    if( Key3_P == 0 )
                        Key3_P = 1;  //上升沿（假设不做动作那就继续）
                }
                Key1_count = 0x80 + NMAX_KEY / 3 * 2; //新一轮的判断
                Key2_count = 0x80 + NMAX_KEY / 3 * 2;
                Key3_count = 0x80 + NMAX_KEY / 3 * 2;
                Key_count = NMAX_KEY;
            }
        }//read_date=read_add(read_addr); 读数据函数放在此处数码管会无法及时显示
    }
} 
