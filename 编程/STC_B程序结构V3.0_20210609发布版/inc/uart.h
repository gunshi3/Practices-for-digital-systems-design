/**********************************Uart1串行通信模块 V1.0 说明 ************************************************************************
Uart1模块提供Uart1模块初始化函数、3个应用函数,1个事件（enumEventRxd）：
(1)	Uart1Init(unsigned int band)：Uart1模块初始化函数。
    函数参数：unsigned int band定义通信（8个数据位、1个停止位，无奇偶校验位）的波特率（单位：bps）
(2)	void SetUart1Rxd(char *RxdPt, unsigned int Nmax, char *matchhead, unsigned int matchheadsize);	
    设置串口接收参数：数据包存放位置、大小，包头匹配字符、匹配字符个数。收到符合条件的数据包时将产生enumEventRxd事件。
	  函数参数： char *RxdPt：指定接收数据包存放区（首地址）
				       unsigned int Nmax：接收数据包大小（字节数），最大65535
               char *matchhead：  需要匹配的数据包头（首地址）
               unsigned int matchheadsize：需要匹配的字节数
     补充说明：
         Nmax=1：为单字节接收，即收到一个字节就产生enumEventRxd事件（如果定义了匹配，还需满足匹配条件）；
	       0 < matchheadsize < Nmax：要求接收数据中连续matchheadsize个字节与*matchhead处数据完全匹配，才在收到Nmax数据时产生enumEventRxd事件；
         matchheadsize = Nmax：设定接收数据包完全匹配
		     matchheadsize=0 或 matchheadsize > Nmax：将不做匹配，接收到任意Nmax数据时产生enumEventRxd事件；
		     在enumEventRxd事件发出后，接收到的数据包应及时使用或取出，收到下一个数据时将破坏和覆盖前面收到的数据包
(3)  char Uart1Print(void *pt, unsigned int num)：发送数据包，非阻塞函数（即函数不等到所设定任务全部完成才返回），该函数从被调用到返回大约1uS左右时间。
	 函数参数：  void *pt ：指定发送数据包位置
	             unsigned int num：发送数据包大小；
   函数返回值：enumTxOK：调用成功，即所设定的发送数据包请求已被系统sys正确接受，sys将尽硬件资源最大可能及时发送数据。	
			         enumTxFailure：调用失败（主要原因是：串口正忙（上一数据包未发完）
	 补充说明：串口上发送1个字节数据大约需要时间0.1mS～10mS（视所设定的波特率），对计算机来说，如果发送多个字节是一个很要时间才能完成的事。类似于                     前面用蜂鸣器演奏音乐，对这类事件与程序"异步"的问题，编程时不仅要注意程序逻辑性、还有注意程序时效性。（这个问题其实总是要注意、必须要注意的）
  (4)  char GetUart1TxStatus(void)： 获取Uart1发送状态
	 函数返回值：enumUart1TxFree:串口1发送空闲
				       enumUart1TxBusy,串口1发送正忙
(5) Uart1接收事件：enumEventRxd。表示收到了一个符合指定要求（数据包头匹配、数据包大小一致）的数据包。

  
编写：徐成（电话18008400450）   2021年3月28日设计
*/


#ifndef _uart_H_
#define _uart_H_

extern void Uart1Init(unsigned int band);						                                                   //串口初始化，参数：波特率
extern void SetUart1Rxd(void *RxdPt, unsigned int Nmax, void *matchhead, unsigned int matchheadsize);	 //设置接收条件：数据包存放位置、大小，包头匹配字符、匹配字符个数。符合条件的包将产生enumEventRxd事件
extern char Uart1Print(void *pt, unsigned int num);                                                    //发送数据包。非阻塞函数。数据包位置、大小。返回值enumTxOK调用成功，enumTxFailure失败（串口忙，上一数据包未发完）
extern char GetUart1TxStatus(void);                                                                    //获取串口1发送状态,enumUart1TxFree:串口1发送空闲, enumUart1TxBusy,串口1发送正忙
enum Uart1ActName {enumUart1TxFree=0,enumUart1TxBusy,enumUart1TxOK,enumUart1TxFailure};

#endif