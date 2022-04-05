#include "STC15F2K60S2.H"
#include "sys.H" 
#include "displayer.H" 
#include "key.h"
#include "hall.h"
#include "Vib.h"
#include "beep.h"
#include "music.h"
#include "adc.h"
#include "uart.h"
#include "stepmotor.h"
#include "stdio.h"                            // Keil C51 库，示例中为应用其sprintf函数

code unsigned long SysClock=11059200L;        //必须。 （定义系统工作时钟频率，用户可以修改，且必须与实际工作频率（下载时选择的）一致

#ifdef _displayer_H_                          //显示模块选用时必须。（数码管显示译码表，用戶可修改、增加等） 
code char decode_table[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00,0x08,0x40,0x01 };   
	              /* 序号:   0   1    2	   3    4	    5    6	  7   8	   9	 10	   11		12   13*        /
                /* 显示:   0   1    2    3    4     5    6    7   8    9  (无)   下-  中-  上-       */  
#endif

void my1Sfunc()
{ static i=0;
	i++;
	LedPrint(i);    
}

void my100mSfunc()
{ 
	static unsigned char i;
	i++;
	Seg7Print(12,10,10,10,10,i/100,(i/10)%10,i%10);              //举例   
}



void main()
{  				
	  DisplayerInit();                                 //举例
// 	
    SetEventCallBack(enumEventSys1S , my1Sfunc);
  	SetEventCallBack(enumEventSys100mS , my100mSfunc);
// 	
		SetDisplayerArea(0,7);                           //举例

                                 //举例
	     MySTC_Init();	             //此行必须！！！   
	while(1)                         //此行必须！！！   
		{  MySTC_OS();     				     //此行必须！！！   
		}
}
