#include "hal_delay.h"
#include "cc2530_ioctl.h"
#include <stdio.h>
#include <stdint.h>
#define LED_PORT        1
#define LED_PIN         0
#define LED             P1_0

#define LED_ON          1
#define LED_OFF         0

#define uint  unsigned int
static void initLed(void);
static void initUart0(void);
static void uart0Send(uint8_t *pMsg, uint8_t msgLen);
void Delay(uint);



#pragma vector = URX0_VECTOR 
__interrupt void URX0_ISR(void)  
{  
    uint8_t rxChar;

    URX0IF = 0;//清零中断标志位

    rxChar = U0DBUF;//U0DBUF存放了从串口0接收到的数据

    if (rxChar != NULL){
      LED = (LED == LED_ON)?LED_OFF : LED_ON;
     Delay(250);
    }

    uart0Send(&rxChar, 1); //通过串口0发送数据，即把从串口0接收到的数据又从串口0发送回去
}


void main()
{
    initLed();
    setSystemClk32MHZ();//设置系统时钟频率为32MHz
    initUart0();//初始化串口0
    while(1) { }
}

/*
 * 初始化串口0
 */
static void initUart0(void)  
{  
    PERCFG = 0x00;//配置P0_2和P0_3用作串口0的TX（发送端）和RX（接收端）
    P0SEL  = 0x3c;//配置P0_2和P0_3用作外设功能，而不是GPIO
    P2DIR &= ~0xc0;//配置端口0优先用作USART0
  
    U0CSR |= 0x80;//设置USART0工作模式为UART0
  
    /* 设置波特率为115200 */  
    U0GCR  |= 11;//BAUD_E  
    U0BAUD |= 216;//BAUD_M  
  
    UTX0IF = 0;// 清零UART0发送中断标志位    
    URX0IF = 0;// 清零UART0接收中断标志位  
    URX0IE = 1;//使能串口0接收中断  
    EA = 1;//打开中断总开关
  
    U0CSR |= 0x40;//启用数据接收功能
}

static void initLed(void)
{
  CC2530_IOCTL(LED_PORT,LED_PIN,CC2530_OUTPUT);
  LED = LED_OFF;
}


/**
 * @fn      uart0Send
 * 
 * @brief	通过串口0发送数据
 *
 * @param pMsg - 待发送数据的地址
 * @param msgLen - 待发送数据的长度，以字节为单位

 *
 * @return 	none
 */
static void uart0Send(uint8_t *pMsg, uint8_t msgLen)  
{  
    uint8_t i;  
    for (i = 0; i < msgLen; i++) {  
        U0DBUF = pMsg[i];//把数据存放到串口0数据缓存寄存器U0DBUF中
        while (UTX0IF == 0);//等待发送完成
        UTX0IF = 0;//清零中断标志位
    }  
}

void Delay(uint16_t nMs)
{
    uint16_t i,j;
  
    for (i = 0; i < nMs; i++)
        //经由实际测试可以得出执行535次循环耗时最接近1ms

        for (j = 0; j < 535; j++);
}