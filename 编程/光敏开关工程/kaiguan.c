#include "STC15F2K60S2.H"
/**************************
引脚别名定义如下：
**************************/

sbit LED_SEL=P2^3;
/**************************
定义变量如下：
**************************/
#define uint unsigned int
#define ulint unsigned long
//设置光照变量
uint n=0;						//执行光的次数
ulint sumn=0;				//光AD值得总和
uint flag=0;				//开关标志位
uint light_old=1;		//上一次光强
uint light_new=1;		//当前光强
uint time=0;				//延时


//初始化光ADC
void InitADC_light()
{
	P1ASF=0xff;
	ADC_RES=0;//AD转换结果寄存器清零
	ADC_RESL=0;
	ADC_CONTR=0X8C;//10001100,打开AD转换电源，启动AD转换，后三位决定P1^4为AD转换输入
	CLK_DIV=0X20;//设置AD转换结果保存格式
}

//AD中断处理程序
void adc_isr() interrupt 5 using 1
{
  time++;
	IE=0x00;											//关闭中断
	
	if(time>2500)
	{
			light_new=(sumn+n/2)/n;		//四舍五入
			sumn=0;
			n=0;
			time=0;
			if((light_old/light_new)>1.30)
			{					  
			  flag=~flag; 
			}
			time=0;	 
			light_old=light_new; 
	}

	//处理光部分的数据
	n++;
	sumn+=ADC_RES*256+ADC_RESL; 	//求n次AD值的和

	ADC_CONTR&=~0X10;  					 //转换完成后，ADC_FLAG清零
	ADC_CONTR|=0X08;	 					 //转换完成后，ADC_START赋1
	IE=0xa0;					 					 //打开中断，EA=1打开总中断，EADC=1打开AD中断
}

void main()
{	
	P0M1=0x00;						//设置P0为推挽模式，点亮数码管
	P0M0=0xff;
	P2M1=0x00;
	P2M0=0x08;						//将P2^3设置为推挽模式，其余为准双向口模式
	LED_SEL=1;								//熄灭LED灯

	IE=0xa0;							//打开中断，EA=1打开总中断，EADC=1打开AD中断
	
	InitADC_light();
	while(1)
	{
		P0=0x00;
		if(flag)
		{
			P0=0x55;
		}
	}
}

