//
// Created by Hasee on 7/11/2019.
//
/**
 * 扫描频率的电子钟
 * @author: 吴剑兵
 */
#include "STC15F2K60S2.h"
#include "intrins.h"

#define uint unsigned int
#define uchar unsigned char
#define i1 interrupt 1
sbit ledSel = P2 ^3;    // led or digital tube
sbit key1 = P3 ^2; //控制数码管扫描频率
uchar baseSevenSegment[] = {0x3f, 0x06, 0x5b, 0x4f, 0x66,
                            0x6d, 0x7d, 0x07, 0x7f, 0x6f}; //七段码
unsigned char const line = 0x40; //横杠的七段码
char timeAddOneFlag = 0;    //标记为1，此时需要秒+1
char key1ActionFlag = 0;     //值为1时表示按键操作需要执行
unsigned int ledOnFlag = 0; //led灯亮
char tubeOnFlag = 0;        //数码管亮
int ledValue = 1;           //led显示的数值
int myDisplay[8] = {0};     //数码管显示的内容
unsigned int timeCount = 1; //时间计时器
unsigned int currHour = 0;
unsigned int currMinute = 0;
unsigned int currSecond = 0;
unsigned int interruptCount = 0;
unsigned int keyDownTime = 0;
unsigned int scanTime = 1;  //扫描时间
unsigned int currBit = 0;   //当前显示的位
void Delay5us()       //@11.0592MHz
{
    unsigned char i;

    _nop_();
    i = 11;
    while (--i);
}

/**
 * 设置ledSel,转为数码管显示
*/
void switchToTube() {
    ledSel = 0;
}

/**
 * 设置ledSel,转为led设置
 */
void switchToLed() {
    P0 = 0;
    ledSel = 1;
}

/**
 * 改变其中一位的内容
 *
 * @param bitNum 位数（第几位）（例如最左那位，则调用1）
 * @param value 改变后的数字，需确保大于0小于10
 */
void change1Bit(int bitNum, int value) {
    myDisplay[bitNum - 1] = baseSevenSegment[value];
}

/**
 * 改变其中一位的内容（设置七段码）
 *
 * @param bitNum 位数（第几位）（例如最左那位，则调用1）
 * @param sevenSegCode 目标七段码
 */
void change1Bit_seven(int bitNum, int sevenSegCode) {
    myDisplay[bitNum - 1] = sevenSegCode;
}

/**
 * 数码管设置为显示指定的数值
 *
 * @param num 数字
 */
void changeAll(long num) {
    int i;
    for (i = 7; i >= 0; --i) {
        int foo = num % 10;
        myDisplay[i] = baseSevenSegment[foo];
        num /= 10;
    }
}

/**
 * led亮
 */
void displayLed() {
    if (ledOnFlag) {
        switchToLed();
        P0 = ledValue;
    }
}

/**
 * 显示数码管
 */
void showTube() {
    if (tubeOnFlag) {
        switchToTube();
        P0 = 0;
        P2 = currBit;
        P0 = myDisplay[currBit];
        Delay5us();
    }
}

/**
 * 初始化定时器
 */
void timer0Initialize()        //0.1毫秒@12MHz
{

    AUXR |= 0x80;        //定时器时钟1T模式
    TMOD &= 0xF0;        //设置定时器模式
    TL0 = 0xAE;        //设置定时器初值
    TH0 = 0xFB;        //设置定时器初值
    TF0 = 0;        //清除TF0标志
    TR0 = 1;        //定时器0开始计时

    EA = 1; //打开总的中断
    ET0 = 1; //打开定时器0中断
}

/**
 * ???
 */
void initialize() {
    P0M0 = 0xFF;
    P0M1 = 0x00;
    P2M0 = 0x0f; //设置P2.0-3为推挽工作状态
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    ledSel = 0;

    timer0Initialize();
    changeAll(0);
    /*
     * 第三位和第六位设置为 '-'
     */
    change1Bit_seven(3, line);
    change1Bit_seven(6, line);
}

/**
 * 时间自增1秒
 */
void addTime() {
    ++currSecond;
    if (currSecond == 60) {
        ++currMinute;
        currSecond = 0;
        if (currMinute == 60) {
            ++currHour;
            currMinute = 0;
            if (currHour == 24)
                currHour = 0;
            change1Bit(2, currHour % 10);
            change1Bit(1, currHour / 10);
        }
        change1Bit(5, currMinute % 10);
        change1Bit(4, currMinute / 10);
    }
    change1Bit(8, currSecond % 10);
    change1Bit(7, currSecond / 10);
}

/**
 * 按下key1要做的事
 */
void key1Action() {
    if (key1ActionFlag) {
        if (ledValue == 0x80)
            ledValue = 0x01;
        else
            ledValue <<= 1;
        if (ledValue == 1)
            scanTime = 1;
        else if (ledValue == 2)
            scanTime = 50;
        else if (ledValue == 4)
            scanTime = 100;
        else if (ledValue == 8)
            scanTime = 200;
        else if (ledValue == 16)
            scanTime = 500;
        else if (ledValue == 32)
            scanTime = 1000;
        else if (ledValue == 64)
            scanTime = 2000;
        else if (ledValue == 128)
            scanTime = 5000;
        key1ActionFlag = 0;
    }
}

/**
 * 收到中断的信号（是时候自增时间了）
 */
void timeSignalHandler() {
    if (timeAddOneFlag) {
        addTime();
        timeAddOneFlag = 0;
    }
}

/**
 * 单片机运行
 */
void run() {
    while (1) {
        timeSignalHandler();
        displayLed();
        showTube();
        key1Action();
    }
}

/**
 * 每0.1毫秒进入一次定时器中断
 */
void interruptFunction() i1 {
    static const int KEY_TIME_THRESHOLD = 500;
    interruptCount = (interruptCount + 1) % 20000;
    //led显示的频率应该低些才有好的显示效果
    if (interruptCount % 12 > 10) {
        ledOnFlag = 1;
        tubeOnFlag = 0;
    } else {
        tubeOnFlag = 1;
        ledOnFlag = 0;
    }
    // 数码管的扫描
    if (interruptCount % scanTime == 0)
        currBit = (currBit + 1) % 8;
    //读秒
    if (interruptCount % 10000 == 0)
        timeAddOneFlag = 1;
    // 按键功能设置
    if (key1 == 0) {
        if (keyDownTime < KEY_TIME_THRESHOLD)
            ++keyDownTime;
    } else {
        if (keyDownTime >= KEY_TIME_THRESHOLD)
            key1ActionFlag = 1;
        keyDownTime = 0;
    }
}

int main() {
    initialize();
    run();
    return 0;
}