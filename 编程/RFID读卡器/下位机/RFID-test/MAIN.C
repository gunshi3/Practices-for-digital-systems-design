#include "STC15F2K60S2.H"        
#include "mfrc522.h" 
#include "main.h"

unsigned char DefaultKey[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; //初始密码
unsigned char code administer_key[6]={0,0,0,0,0,0};//管理员卡密码
unsigned char code this=0x01;
unsigned char write_Data[16]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x80,0x69,0xff,0xff,0xff,0xff,0xff,0xff};
//*********全局变量定义*********************************//
unsigned char Rec_Buffer[REC_BUFFER_SIZE] = {0};		//串口接收缓冲区
unsigned char Recvd_Count = 0;				//串口已接收数据的个数
bit Uart1_Sendbusy=0;
unsigned char g_ucTempbuf1[4];  //返回卡号	
unsigned char g_ucTempbuf2[16]; //返回块中16位数据                     

char PcdReset_flag;				//初始化标志位
char PcdAntennaOn_flag;			//打开天线标志位
char PcdAntennaOff_flag;		//关闭天线标志位
char PcdRequest_flag;           //询卡标志位
char write_data_flag;			//写数据标志位
char read_date_flag;			//读数据标志位
char flag1;
																										 
/*****************************************************************************
*原型：void time0_init(void)
*功能：定时器0初始化
*input:无
*ouput:无
用于定时计费
******************************************************************************/
void time0_init(void)
{
	TMOD=0x01;//定时器0，方式1
	ET0=1;//开启定时器中断	
 	TH0=(65535-1000)/256;
	TL0=(65535-1000)%256;
	TR0=1;//启动定时器
}
void timer0() interrupt 1
{
  static uchar overflow=0;
  overflow++;
  if(overflow == 0xff){
// 	overflow = 0;
//	Sent_Byte(1);
  }
}
//-------------------------------- ------------------------------------------------------------------
// 函数名称： delay
// 入口参数： N
// 函数功能：延时子程序，实现(16*N+24)us的延时 
// 系统采用11.0592MHz的时钟时,延时满足要求,其它情况需要改动
//--------------------------------------------------------------------------------------------------
void Delay_ms(uint N)       
{
  uint i,j;
  for(j=0;j<1000;j++)
    for(i=0;i<=N;i++);

}
/*******************************************************************************
* Function Name  : UART_Init
* Description    : 初始化串行口和波特率发生器(用定时器1) 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART_Init(void)
{
    AUXR=0X80;	//辅助寄存器 此时定时器0的速度是传统的12倍，不分频
	SCON|=0X50; //允许接收
	TL1=(65536-(Machine_Focs/4/BAUD1));
	TH1=(65536-(Machine_Focs/4/BAUD1))>>8;
	AUXR|=0X40;	//辅助寄存器 此时定时器1的速度是传统的12倍，不分频
	RI=0;		 //接收中断标志位
	TI=0;		 //发送中断标志位
	TR1=1;		 //启动定时器1
	ES=1;		 //串口中断允许位
	EA=1;		 //总中断允许位
	PS = 1 ;	//串口1中断高优先级
}
/*******************************************************************************
* Function Name  : Sent_Byte
* Description    : 发送一个字节到主机
* Input          : Sdata:要发送的字节
* Output         : None
* Return         : None
*******************************************************************************/
void Sent_Byte(unsigned char Sdata)
{
	TI = 0;
	SBUF=Sdata;
 	while(TI==0) ;  //等待TI是否为1，TI=1说明发送数据完成
 	TI=0;      //手动TI清零
}
/*******************************************************************************
* Function Name  : Sent_String
* Description    : 发送字节串到主机
* Input          : pt_send:要发送的字符串
* Output         : None
* Return         : None
*******************************************************************************/
void Sent_String(unsigned char *pt_send)
{
  TI=0;
  while(*pt_send!='\0')
  {
  	Sent_Byte(*pt_send++);
  }
}
/*******************************************************************************
* Function Name  : Sent_Buffer
* Description    : 向串口1发送指定长度的数据
* Input          : string:发送缓冲区指针;len:发送长度.
* Output         : None
* Return         : None
*******************************************************************************/
void Sent_Buffer(uchar *string,uchar len)
{
  uchar i;
	ES	=	0;
  	for(i=0;i<len;i++)
	   {
	    Sent_Byte(*string++);
       }
	ES	=	1;
}
void Init()
{
  	P0M1=0x00;        //设置推挽输出
    P0M0=0xff;
    P4M0=0xff;
    P4M1=0x00;
    P2M1=0x00;
    P2M0=0x08;
    P0 = 0x00;
	time0_init();    // 定时器0初始化
	UART_Init();	 // 串口初始化

	//MF_RC522传输总线设置
	MF522_IIC = 1;        //  设置为SPI总线传输
	MF522_EA = 0;
}
//********************主函数******************************************
main()
{ 
	
    int i = 0;
    uchar status;
	unsigned long int display_data=0;
	Init();
	UART_Init();		//初始化串口
    time0_init();		//初始化定时器0,要在读mode以后
	//初始化射频芯片
    PcdReset();
	status = ReadRawRC(ModeReg);
	P0 =  status;
	//关闭天线
    PcdAntennaOff();
	//打开天线
	PcdAntennaOn();  
    Delay_ms(100); 
    while(1)
	{  
		 if(Recvd_Count==1 && Rec_Buffer[0] == 0x40){ //初始化
		 	  P0 = 0xf0;
			  PcdReset_flag = Rec_Buffer[0];
			  PcdReset();
			  for(i=0;i<Recvd_Count;++i)
			  	Rec_Buffer[0]=0x00;
			  Recvd_Count=0;
			  PcdReset_flag = -1;			  
		 }	  
		 if(Recvd_Count==1 && Rec_Buffer[0] == 0x41){ //询卡
		 	  P0 = 0xf0;
			  PcdRequest_flag = Rec_Buffer[0];
			  PcdRequest(PICC_REQALL, g_ucTempbuf1);
			  for(i=0;i<Recvd_Count;++i)
			  	Rec_Buffer[0]=0x00;
			  Recvd_Count=0;
			  PcdRequest_flag = -1;			  
		 }
		 if(Recvd_Count==1 && Rec_Buffer[0] == 0x42){ //打开天线
		 	  P0 = 0xf0;
			  PcdAntennaOn_flag = Rec_Buffer[0];
			  PcdAntennaOn();
			  for(i=0;i<Recvd_Count;++i)
			  	Rec_Buffer[0]=0x00;
			  Recvd_Count=0;
			  PcdAntennaOn_flag = -1;			  
		 }
		 if(Recvd_Count==1 && Rec_Buffer[0] == 0x43){ //关闭天线
		 	  P0 = 0xf0;
			  PcdAntennaOff_flag = Rec_Buffer[0];
			  PcdAntennaOff();
			  for(i=0;i<Recvd_Count;++i)
			  	Rec_Buffer[0]=0x00;
			  Recvd_Count=0;
			  PcdAntennaOff_flag = -1;			  
		 }
		 //寻卡,输出为卡类型	   
		 status = PcdRequest(PICC_REQALL, g_ucTempbuf1);
		 //*PICC_REQALL=0x52:寻天线区内所有符合14443A标准的卡  PICC_REQIDL=0x26:只寻未进入休眠状态的卡  	            
         if (status == MI_OK){
	        P0 = 0x01;
	        status = PcdAnticoll(g_ucTempbuf1);  //防冲撞处理，输出卡片序列号，4字节 
		 }
		 if(status == MI_OK){
		    P0 = 0x02;
		    status = PcdSelect(g_ucTempbuf1);    //选择卡片，输入卡片序列号，4字节
		 }	
		 if(status == MI_OK){		//包头为0xff，说明是读取卡号命令
		    P0 = 0x04;			
			if(Recvd_Count==1 && Rec_Buffer[0] == 0xff){   
				Sent_Byte(0xff);
				Sent_Byte(0x49);
				Sent_Byte(0x04);
				for(i=0;i<4;i++)
				Sent_Byte(g_ucTempbuf1[i]);  //读取卡号
				for(i=0;i<Recvd_Count;i++)
					Rec_Buffer[i]=0x00;
		        Recvd_Count=0;	
		    }	
			//包头为0xfe，说明是读取卡中一块数据   Rec_Buffer[0]为包头0xfe  Rec_Buffer[1]为块地址  Rec_Buffer[2~7]为秘钥A
		    if(Recvd_Count==8 && Rec_Buffer[0] == 0xfe){  
			    read_date_flag = 0x11;
				if(Rec_Buffer[1]>0x3F){   //最大块为64块  访问块地址超出最大值
					Sent_Byte(0xfe);Sent_Byte(0x02);
				}
				else{  //访问块地址正常
					for(i=0;i<6;i++){  //获取秘钥
						DefaultKey[i] = Rec_Buffer[i+2];
					}
					status = PcdAuthState(PICC_AUTHENT1A, Rec_Buffer[1], DefaultKey, g_ucTempbuf1);   //验证卡片密码
										 
					if(status == MI_OK){ //密码验证正确
						status = PcdRead(Rec_Buffer[1], g_ucTempbuf2);//读卡   返回块中16位数据 
						if(status == MI_OK){  //读卡数据成功
							Sent_Byte(0xfe);
							Sent_Byte(0x49);
							Sent_Byte(0x10);
							for(i=0;i<16;i++){
								Sent_Byte(g_ucTempbuf2[i]);	
							}
						}else{   //读卡数据失败
							Sent_Byte(0xfe);Sent_Byte(0x01);
						}
					
    				 }else{  //密码验证错误
						Sent_Byte(0xfe);Sent_Byte(0x00);
					 }
			   }
			   for(i=0;i<Recvd_Count;i++)
			      Rec_Buffer[i]=0x00;
			   Recvd_Count=0;
			   read_date_flag = 0x00;	
			}
			//包头为0xfd，说明是写卡中一块数据   Rec_Buffer[0]为包头0xfd  Rec_Buffer[1]为块地址  Rec_Buffer[2~7]为秘钥A  Rec_Buffer[8~23]为秘钥A
			if(Recvd_Count==24 && Rec_Buffer[0] == 0xfd){  
				write_data_flag =  0x12;
				if(Rec_Buffer[1]>0x3F||((Rec_Buffer[1]>0x04)&&((Rec_Buffer[1]-0x04)%0x03==0))||Rec_Buffer[1]==0x03){   
				    //超出最大地址块或者限制修改卡扇区密码
					Sent_Byte(0xfd);Sent_Byte(0x02);
				}
				else{  //访问块地址正常
					for(i=0;i<6;i++){  //获取秘钥
						DefaultKey[i] = Rec_Buffer[i+2];
					}
					status = PcdAuthState(PICC_AUTHENT1A, Rec_Buffer[1], DefaultKey, g_ucTempbuf1);   //验证卡片密码				 
					//密码验证正确
					if(status == MI_OK){
						for(i=0;i<16;i++){  //获取要写入的块数据
							write_Data[i] = Rec_Buffer[i+8];
						}
						status=PcdWrite(Rec_Buffer[1],write_Data);   //写入卡中一块数据
						if(status == MI_OK){  //写入卡数据成功
							Sent_Byte(0xfd);Sent_Byte(0xfd);
						}
						else{   //写卡数据失败
							Sent_Byte(0xfd);Sent_Byte(0x04);
						}
					}
				
					else{  //密码验证错误
						Sent_Byte(0xfd);Sent_Byte(0x00);
					}
				}
				for(i=0;i<Recvd_Count;i++)
			    	Rec_Buffer[i]=0x00;
				Recvd_Count=0;
				write_data_flag = 0x00;	
			}
		}
		else{
			for(i=0;i<Recvd_Count;i++)
				Rec_Buffer[i]=0x00;
			Recvd_Count=0;	
        }
  	}
}

/*******************************************************************************
* Function Name  : Serial () 
* Description    : 串口中断函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void Serial () interrupt  4  using 2
{
	 if(RI)
	 { 
	   RI=0;
       Rec_Buffer[Recvd_Count++]=SBUF;
	   P0 |= 0xf0;
	 }
	 if(TI)		    //发送完数据 TI自动置1
	 {
		TI=0;
		Uart1_Sendbusy=0;
	 }
}
