/******************  STCBSP测试Demo：STC_DemoV3 *************************** 
STCBSP提供的底层支持和应用层函数支持以下"STC-B学习板"功能同时工作：
     1，数码管显示
		 2，LED指示灯显示
		 3，实时时钟
		 4，温度光照测量
		 5，音乐播放
		 6，FM收音机
		 7，EXT扩展接口（电子秤、超声波测距、旋转编码器、PWM控制，4选1工作）
		 8，振动传感器
		 9，霍尔传感器
		 10，步进电机控制
		 11，串口1通信
		 12，串口2通信（485、EXT扩展接口，2选1）
		 13，红外遥控
		 14，红外收发通信
		 15，非易失性NVM存储（DS1302提供31字节，M24C02提供256字节）
具体使用方法请参见各模块头文件。

本STC_DemoV3示例程序（源程序由main.c、main.h、function.c三部分组成）功能描述如下：		 
		1，按键Key2切换"显示"和"按键操作"模式。模式值在LED上显示（二进制数）。模式有：
					 模式1：实时时钟（年月日）
					 模式2：实时时钟（时分秒）
					 模式3：温度、光照测量
					 模式4：音乐播放
					 模式5：FM收音机
					 模式6：超声波测距（需要在EXT上接超声波测距模块）
					 模式7：电子秤    （需要在EXT上接电子秤模块）
    2，数码管显示、导航按键功能：
		       在模式1下：数码管显示实时时钟RTC   "年年-月月-日日"，导航按键 "中"键 进入或退出"设置年月日"；"上、下、左、右"设置
		       在模式2下：数码管显示实时时钟RTC   "时时-分分-秒秒"，导航按键 "中"键 进入或退出"设置时分秒"；"上、下、左、右"设置
		       在模式3下：数码管显示温度、光照AD值"温温温  光光光", 导航按键无操作
					 在模式4下：数码管显示      "节拍率  音调音高  音符"，导航按键 "中"键 暂停/播放，"上、下"调整音调，"左、右"调整节拍率
           在模式5下：数码管显示      "音量   收音频率（MHz）"，导航按键 "中"键 保存当前收音参数到NVM（下次上电时，用该参数收音）
					                                                               "上、下"调整音亮，"左、右"调整收音频率
					 在模式6下：数码管显示       "       距离值（cm）"  ，导航按键无操作 
           在模式7下：数码管显示       "称重ADC值（符号数）"  ，导航按键无操作  
     3，按键1（按下时）：红外向外发送 "大家好！"。其它"STC-B学习板"（如用本程序）用串口助手（设置波特率1200bps，文本接收方式）可看到接收到的"大家好！"文本		
        按键3（抬起时）：串口1向计算机发送 程序运行性能参数。性能参数描述：程序主循环每秒次数、每秒用户程序调度丢失数	 
		 4，振动传感器：（如果有音乐播放）控制音乐播放"暂停/继续"
		 5，霍尔传感器：有磁场接近时，（如果蜂鸣器空闲）发声报警（1350Hz，发1秒时间）
		 6，串口1：与计算机双向通信（波特率1200bps）
		           如果收到一个"AA 55"开头、大小8字节数据包，则将第7字节+1，再以波特率2400bps向串口2（485、EXT可选）转发此数据包
							 如果红外接口收到数据包（引导码+数据+结束码），则向串口1转发
		 7，串口2：可选择485（双向单工）、EXT（TTL Uart，双向双工）两个位置，工作波特率2400bps
		           如果收到一个"AA 55"开头、大小8字节数据包，则将第7字节+2，再向红外Ir口转发此数据包（NEC_R05d编码协议）
		 8，红外收发接口：
							 如果红外接口收到数据包（引导码+数据+结束码），则蜂鸣器发声（1000Hz，300mS），并向串口1转发；
               如果数据包包头为"AA 55"，并：第3字节为 F1，则调整收音频率（第4、5字节，BCD码，收音频率值，0.1MHz）（数据需有效：887～1080），
							                                                      音量（第6字节数据）（数据需有效：0～15）       ，第7字节+4
							                              第3字节为 F2，则调整RTC时分秒（第4、5、6字节数据，BCD码）（数据需有效），第7字节+4
																						第3字节为 F3，则调整RTC年月日（第4、5、6字节数据，BCD码）（数据需有效），第7字节+4
																				调整值存入NVM，
																				蜂鸣器发声（1000Hz，600mS）
																				并向串口1转发；

设计：徐成（电话：18008400450）    2021年9月5日
***************************************************************************************************************************/


//******* 用户程序段1：用户程序包含文件 *************//
#include "main.H"                              //必须。编写应用程序时，仅需改写 main.h 和 main.c文件 
#include "song.c"                              //举例。song.c中编写了音乐（同一首歌）编码
//******* 用户程序段2：用户自定义函数声明 *************//


//******* 用户程序段3：用户程序全局变量定义 *************//
struct_DS1302_RTC t={0x30,0,9,0x06,9,1,0x21};  //举例。 实时时钟数据结构：秒、分、时、日、月、周、年。初值：2021年9月6日，周一，9：00：30
struct_FMRadio    FM;                          //举例。 FM收音机数据结构：收音频率、音量、GP1、GP2、GP3。初值：95.5MHz，音量5，灭、灭、亮
struct_SysPerF    SysPer;                      //举例。 系统性能数据结构：每秒主循环次数4字节、每秒轮询丢失次数1字节
struct_ADC        ADCresult;                   //举例。 热敏、光敏测量AD值           
unsigned char Music_tone,Music_PM;             //举例。 音乐播放音调、节凑（每分钟节拍数）

unsigned char rxd[8];                          //举例。通信（串口1、串口2、红外共用）缓冲区8字节
unsigned char rxdhead[2]={0xaa,0x55};          //举例。通信（串口1、串口2）接收包头匹配字符2字节：（0xAA，0x55）    

unsigned char funcmode;                  //举例。定义显示、按键功能模式
enum funcname {RTC_YMD=1,                      //举例。功能模式命名。实时时钟：年月日
               RTC_HMS,                        //                    实时时钟：时分秒
               Rt_Rop,                         //                    热敏光敏测量
               Music,                          //                    音乐播放
               FMradio,                        //                    FM收音机
               UltroSonic,                     //                    超声波测距
               Weight};                        //                    电子秤
unsigned char tempadj;                   //举例。程序变量。调整时钟时用：=1 调年或时；=2 调月或分 =3 调日 或秒

//******* 用户程序段4：用户自定义函数原型 *************//
#include "function.c" 

void myUart1Rxd_callback()                    //举例。串口1收到合法数据包回调函数。（合法数据包：AA，55，......。共8字节）
{ if ( GetUart2TxStatus() == enumUart2TxFree )                  
	  {	(*(rxd+6)) += 1;                                          //第7字节加1
	     Uart2Print(&rxd, sizeof(rxd));                           //将数据包从串口2(485、或EXT扩展接口上)发送出去
    }			
}

void myUart2Rxd_callback()                    //举例。串口2收到合法数据包回调函数。（合法数据包：AA，55，......。共8字节）
{ if ( GetIrStatus() == enumIrFree ) 
	 { (*(rxd+6)) += 2;                                             //第7字节加2
	   IrPrint(&rxd, sizeof(rxd));                                  //将数据包从红外发送出去
	 }
}

void myIrRxd_callback()                      //举例。红外收到数据包回调函数。（有红外前导+数据+结束，NEC_R05d编码）  
{ dealwithIrRxd();	
}

void my1mS_callback()			                  //举例。1mS事件回调函数
{  
}
	
void my10mS_callback()			                //举例。10mS事件回调函数
{ 
}

void my100mS_callback()			                //举例。100mS事件回调函数
{ dealwithDisp();                          
}			 
							 
void my1S_callback()				               //举例。1S事件回调函数
{ 	
}

void myADC_callback()                      //举例。ADC事件回调函数
{ 
}

void myKN_callback()			               	                                                        //举例。导航按键事件回调函数
{ dealwithmyKN();		
}

void mykey_callback()		                            // 按键（Key1、Key2）事件回调函数
{	dealwithmykey();	  
}

void myhall_callback()		                                                                //示例。有hall事件回调函数：发声报警
{	if(GetHallAct() == enumHallGetClose) SetBeep(1350,100);		
}

void mySV_callback()		                                                                  //示例: 振动事件回调函数：控制音乐播放/暂停
{	if(GetVibAct())
	if (GetPlayerMode() == enumModePause) SetPlayerMode(enumModePlay); 
			else  SetPlayerMode(enumModePause);			
}

//******* main()函数 *************//	
void main() {         //主函数 main() 开始         //此行必须！！！ 
	
//******* 用户程序段5：用户main()函数内部局部变量定义 *************//
	
//******* 用户程序段6：用户main()函数（初始化类程序） *************//		
//1,加载需要用的模块(由各模块提供加载函数)
    Key_Init();                                      //举例，需要用到的模块及其函数、方法，必须对其初始化（让其准备、就绪和响应做后台服务）
	  HallInit();                                      //举例
	  VibInit();                                       //举例
	  DisplayerInit();                                 //举例
	  BeepInit();	                                     //举例
    MusicPlayerInit();	                             //举例
	  AdcInit(ADCexpEXT);                              //举例，ADC模块初始化，有参数，选择扩展接口EXT上P1.0、P1.1是否也做ADC功能
    StepMotorInit();	                               //举例，步进电机初始化
	  DS1302Init(t);                                   //举例，DS1302初始化
    	
		IrInit(NEC_R05d);                                //举例，红外接口设置
    /******* 以下可4选1：加载EXT接口 ********/    
//    EXTInit(enumEXTWeight);                          //举例，EXT初始化成电子秤功能
// 	  EXTInit(enumEXTPWM);                             //举例，EXT初始化成两路PWM功能
// 	  EXTInit(enumEXTDecode);                          //举例，EXT初始化成增量式编码器解码功能（如：旋转编码器）
// 	  EXTInit(enumEXTUltraSonic);                      //举例，EXT初始化成超声波测距功能
	  Uart1Init(1200);                                 //举例，串口1初始化，有参数，设置Uart1通信波特率
    /******* 以下可2选1：加载串口2 ********/    
    Uart2Init(2400,Uart2Usedfor485);                 //举例，串口2初始化到485接口（RS485工业现场总线，半双工）
// 	  Uart2Init(2400,Uart2UsedforEXT);               //举例，串口2初始化到EXT接口（TTL Uart串行通信口）
		
//2,设置事件回调函数(由sys提供设置函数SetEventCallBack())	
		SetEventCallBack(enumEventKey, mykey_callback);            //举例
		SetEventCallBack(enumEventSys1mS, my1mS_callback);         //举例
		SetEventCallBack(enumEventSys10mS, my10mS_callback);       //举例
		SetEventCallBack(enumEventSys100mS, my100mS_callback);     //举例
		SetEventCallBack(enumEventSys1S, my1S_callback);           //举例
		SetEventCallBack(enumEventHall, myhall_callback);          //举例
		SetEventCallBack(enumEventVib, mySV_callback);             //举例
		SetEventCallBack(enumEventNav, myKN_callback);             //举例，设置导航按键回调函数
    SetEventCallBack(enumEventUart1Rxd, myUart1Rxd_callback);  //举例，设置串口1接收回调函数
		SetEventCallBack(enumEventUart2Rxd, myUart2Rxd_callback);  //举例，设置串口2接收回调函数
		SetEventCallBack(enumEventXADC,myADC_callback);            //扩展接口上新的AD值事件
		SetEventCallBack(enumEventIrRxd,myIrRxd_callback);         //红外Ir上收到一个数据包

//3,用户程序状态初始化 	
		SetDisplayerArea(0,7);                           //举例
	  SetUart1Rxd(&rxd, sizeof(rxd), rxdhead, sizeof(rxdhead));   //设置串口接收方式：数据包条件：接收数据包放置在rxd中，数据包大小rxd大小，数据包头需要与rxdhead匹配，匹配数量rxdhead大小
	  SetUart2Rxd(&rxd, sizeof(rxd), rxdhead, sizeof(rxdhead)); 
		SetIrRxd(&rxd);                                     //举例
		
//4,用户程序变量初始化
    FM=FM_NVMread();                              //举例，FMRadio初始化。
		FMRadioInit(FM);	
		
    Music_PM=90;
		Music_tone=0xFC;
    funcmode = M24C02_Read(0x00);                               
    LedPrint(funcmode);	
		
/*****************  MySTC_OS 初始化与加载开始  ********************************/   
	  MySTC_Init();	    // MySTC_OS 初始化         //此行必须！！！   
	while(1)            // 系统主循环              //此行必须！！！   	
		{ MySTC_OS();     // MySTC_OS 加载           //此行必须！！！
/*****************  MySTC_OS 初始化与加载结束  ********************************/  
			
//******* 用户程序段7：用户main()函数（主循环程序） *************//
			
			
		}	              //主循环while(1)结束         //此行必须！！！ 
}                   //主函数 main() 结束         //此行必须！！！ 

