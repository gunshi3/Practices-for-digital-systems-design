/**********************************Ver3.0 说明 ************************************************************************
  (1) sys.c构成STC程序基本架构。提供：
	      a, 系统初始化MySTC_Init(),系统调度函数MySTC_OS(),加载用户回调函数SetEventCallBack()
				b, 若干可设置和触发回调函数的事件；
					          enumEventSys1mS       : 1mS定时到
										enumEventSys10mS      : 10mS定时到
										enumEventSys100mS     : 100mS定时到
										enumEventSys1S        : 1S定时到
										enumEventKey          : 按键事件
										enumEventNav          : 导航按键事件“						
										enumEventHall         : hall传感器事件
										enumEventVib          : 振动传感器事件
										enumEventXADC         : 扩展接口上新的AD值事件
										enumEventRxd          : 串口上收到一个符合格式定义的事件
										enumEventSysWarnning} : 系统调度出现"失调"事件。出现了"未按时"完成任务的事件，可能但不代表出现逻辑错误或系统崩溃（需看更详细调度信息）
        c, 系统运行性能评价参数：struct SysPerF SysPerformance
				
	(2) MySTC_Init()：sys初始化函数，必须执行一次。
  (3) MySTC_OS()：  sys调度函数，应在while（1）循环中。	
  (4) SetEventCallBack(char event, void *(user_callback)):加载"事件"用户回调函数.事件有:
	(5) 系统运行性能评估参数struct SysPerF SysPerformance：
        a, SysPerformance.MainLoops          unsigned long  每秒主循环次数(正常应大于1000以上，越大越好) 
// 				b, SysPerformance..MainLoopSkew      unsigned char  主循环波动最大值（每mS之间）（暂未实现）
        c, SysPerformance.Pollingmisses      unsigned char  每秒轮询丢失次数丢失次数(正常应为0)（超过255只记到255）
				d, SysPerformance.SysAveClks         unsigned char  每循环sys程序平均消耗系统时钟数
				e，SysPerformance.UserAveClks        unsigned int   每循环用户程序平均消耗系统时钟数
				f, SysPerformance.SysSourceRemained  unsigned char  系统（计算能力）资源剩余评估（%）
			
	(6) 补充说明：
	      a, sys.c 占用定时器0给系统产生1mS定时中断。
				b, 系统时钟在12Mhz时，MySTC_OS()在每循环大约消耗系统时钟数115个（约9.58uS）（2021年3月29测试）
				b, 1mS为本架构基本要求(受STC单片机资源和功能限制的原因造成),因此要求用户的任何程序片段，其单次循环执行时间累加起来应小于1mS。
		
编写：徐成（电话18008400450）   2021年2月26日设计，2021年3月29日更新
*/

#ifndef	_sys_h_
#define _sys_h_

extern void MySTC_Init();
extern void MySTC_OS();                                                    
extern void SetEventCallBack(char event, void *(user_callback));
enum event{enumEventSys1mS,                      //系统1mS事件
	         enumEventSys10mS,                     //系统10mS事件
           enumEventSys100mS,                    //系统100mS事件
           enumEventSys1S,                       //系统1S事件
           enumEventKey,                         //按键事件
           enumEventHall,                        //hall传感器事件
           enumEventVib,                         //振动传感器事件
           enumEventNav,                         //导航按键事件“
           enumEventXADC,                        //扩展接口上新的AD值事件
           enumEventRxd,                         //串口上收到一个符合格式定义的事件
           enumEventSysWarnning};                //系统调度出现"失调"事件。出现了"未按时"完成任务的事件，可能但不代表出现逻辑错误或系统崩溃（需看更详细调度信息）

typedef struct                         //用于各模块加载驱动
{  void (*DrvDisplayer)() reentrant;  
	 void (*DrvKey)()       reentrant; 
	 char (*GetKey)()       reentrant; 	
	 void (*DrvHall)()      reentrant; 	
	 char (*GetHall)()      reentrant; 
	 void (*DrvVib)()       reentrant;   
	 char (*GetVib)()       reentrant; 
   void (*DrvBeep)()      reentrant;  
	 void (*DrvMusic)()     reentrant; 
   void (*DrvADC)()       reentrant;		
	 char (*GetNav)()       reentrant;            
	 char (*GetXADC)()      reentrant; 
	 char (*GetUart1Rxd)()  reentrant;
	 void (*DrvStepMotor)() reentrant; 
}  SysPt; 

extern xdata SysPt stcDrivers; 

typedef struct                   //系统性能评估参数,每秒更新一次
  { unsigned long MainLoops;            //SysPerformance.MainLoops            每秒主循环次数（应大于1000以上）
		unsigned char PollingMisses;        //SysPerformance.PollingMisses        每秒轮询丢失次数（理想值为0。如果有非0情况，说明用户程序或算法存在问题）
    unsigned char SysAveClks; 	        //SysPerformance.SysAveClks           每循环sys程序平均消耗系统时钟数
		unsigned int  UserAveClks;          //SysPerformance.UserAveClks          每循环用户程序平均消耗系统时钟数
		unsigned char SysSourceRemained;    //SysPerformance.SysSourceRemained    系统（计算能力）资源剩余（%）
	} SysPerF;	                        
extern xdata SysPerF SysPerformance;
	
#endif