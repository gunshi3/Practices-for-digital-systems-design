//******* 用户程序段1：用户程序包含文件 *************//
#include "main.H" //必须。编写应用程序时，仅需改写 main.h 和 main.c文件
#include "song.c" //举例。song.c中编写了音乐（同一首歌）编码
//******* 用户程序段2：用户自定义函数声明 *************//

//******* 用户程序段3：用户程序全局变量定义 *************//
unsigned char Music_tone, Music_PM; //举例。 音乐播放音调、节凑（每分钟节拍数）
unsigned int count;
unsigned char rxd[1] = {0x01};
unsigned char rtx[2];
unsigned char rxdhead[1] = {0xaa};

//******* 用户程序段4：用户自定义函数原型 *************//
#include "function.c"

void my1S_callback() //举例。1S事件回调函数
{
	count++;
	if (count == 60)
	{
		count = 0;
		if(GetPlayerMode() == enumModePlay)
			SetPlayerMode(enumModePause);
	 	SetDisplayerArea(0, 7);
		Seg7Print(23, 16, 34, 20, 31, 20, 5, 32);
		*(rxd + 0) = 8;
		Uart1Print(&rxd, sizeof(rxd)); //从串口1发送出去
		SetBeep(1000, 20);   
		Delay300ms();
		Seg7Print(10, 10, 10, 23, 20, 26, 26, 0);
	}
}

void myKN_callback() //举例。导航按键事件回调函数
{
	dealwithmyKN();
}

void mykey_callback() // 按键（Key1、Key2）事件回调函数
{
	dealwithmykey();
}

void myADC_callback() //举例。ADC事件回调函数
{
}

void mySV_callback() //示例: 振动事件回调函数：控制音乐播放/暂停
{
	if (GetVibAct())
		if (GetPlayerMode() == enumModePause)
			SetPlayerMode(enumModePlay);
		else
			SetPlayerMode(enumModePause);
}

void myUart1_callback()
{
	SetBeep(1000, 30);
	if(GetPlayerMode() == enumModePlay)
		SetPlayerMode(enumModePause);
	Seg7Print(34, 20, 31, 35, 22, 28, 28, 19);
	Delay1000ms();
	if(GetPlayerMode() == enumModePause)
		SetPlayerMode(enumModePlay);
}

//******* main()函数 *************//
void main()
{ //主函数 main() 开始         //此行必须！！！

	//******* 用户程序段5：用户main()函数内部局部变量定义 *************//

	//******* 用户程序段6：用户main()函数（初始化类程序） *************//
	//1,加载需要用的模块(由各模块提供加载函数)
	Key_Init();			//举例，需要用到的模块及其函数、方法，必须对其初始化（让其准备、就绪和响应做后台服务）
	VibInit();			//举例
	DisplayerInit();	//举例
	BeepInit();			//举例
	MusicPlayerInit();	//举例
	AdcInit(ADCexpEXT); //举例，ADC模块初始化，有参数，选择扩展接口EXT上P1.0、P1.1是否也做ADC功能
	Uart1Init(1200);	//举例，串口1初始化，有参数，设置Uart1通信波特率

	//2,设置事件回调函数(由sys提供设置函数SetEventCallBack())
	SetEventCallBack(enumEventKey, mykey_callback);	 //举例
	SetEventCallBack(enumEventSys1S, my1S_callback); //举例
	SetEventCallBack(enumEventNav, myKN_callback);	 //举例，设置导航按键回调函数
	SetEventCallBack(enumEventVib, mySV_callback);
	SetEventCallBack(enumEventXADC, myADC_callback); //扩展接口上新的AD值事件
	SetEventCallBack(enumEventUart1Rxd, myUart1_callback);

	//3,用户程序状态初始化
	SetDisplayerArea(0, 7);
	Seg7Print(10, 10, 10, 23, 20, 26, 26, 0);
	SetUart1Rxd(&rtx, sizeof(rtx), rxdhead, sizeof(rxdhead));
	//设置串口接收方式：数据包条件：接收数据包放置在rxd中，数据包大小rxd大小，数据包头需要与rxdhead匹配，匹配数量rxdhead大小

	//4,用户程序变量初始化
	Music_PM = 90;
	Music_tone = 0xFC;

	/*****************  MySTC_OS 初始化与加载开始  ********************************/
	MySTC_Init(); // MySTC_OS 初始化         //此行必须！！！
	while (1)	  // 系统主循环              //此行必须！！！
	{
		MySTC_OS(); // MySTC_OS 加载           //此行必须！！！
		/*****************  MySTC_OS 初始化与加载结束  ********************************/

		//******* 用户程序段7：用户main()函数（主循环程序） *************//

	} //主循环while(1)结束         //此行必须！！！
} //主函数 main() 结束         //此行必须！！！
