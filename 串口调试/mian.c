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

    URX0IF = 0;//�����жϱ�־λ

    rxChar = U0DBUF;//U0DBUF����˴Ӵ���0���յ�������

    if (rxChar != NULL){
      LED = (LED == LED_ON)?LED_OFF : LED_ON;
     Delay(250);
    }

    uart0Send(&rxChar, 1); //ͨ������0�������ݣ����ѴӴ���0���յ��������ִӴ���0���ͻ�ȥ
}


void main()
{
    initLed();
    setSystemClk32MHZ();//����ϵͳʱ��Ƶ��Ϊ32MHz
    initUart0();//��ʼ������0
    while(1) { }
}

/*
 * ��ʼ������0
 */
static void initUart0(void)  
{  
    PERCFG = 0x00;//����P0_2��P0_3��������0��TX�����Ͷˣ���RX�����նˣ�
    P0SEL  = 0x3c;//����P0_2��P0_3�������蹦�ܣ�������GPIO
    P2DIR &= ~0xc0;//���ö˿�0��������USART0
  
    U0CSR |= 0x80;//����USART0����ģʽΪUART0
  
    /* ���ò�����Ϊ115200 */  
    U0GCR  |= 11;//BAUD_E  
    U0BAUD |= 216;//BAUD_M  
  
    UTX0IF = 0;// ����UART0�����жϱ�־λ    
    URX0IF = 0;// ����UART0�����жϱ�־λ  
    URX0IE = 1;//ʹ�ܴ���0�����ж�  
    EA = 1;//���ж��ܿ���
  
    U0CSR |= 0x40;//�������ݽ��չ���
}

static void initLed(void)
{
  CC2530_IOCTL(LED_PORT,LED_PIN,CC2530_OUTPUT);
  LED = LED_OFF;
}


/**
 * @fn      uart0Send
 * 
 * @brief	ͨ������0��������
 *
 * @param pMsg - ���������ݵĵ�ַ
 * @param msgLen - ���������ݵĳ��ȣ����ֽ�Ϊ��λ

 *
 * @return 	none
 */
static void uart0Send(uint8_t *pMsg, uint8_t msgLen)  
{  
    uint8_t i;  
    for (i = 0; i < msgLen; i++) {  
        U0DBUF = pMsg[i];//�����ݴ�ŵ�����0���ݻ���Ĵ���U0DBUF��
        while (UTX0IF == 0);//�ȴ��������
        UTX0IF = 0;//�����жϱ�־λ
    }  
}

void Delay(uint16_t nMs)
{
    uint16_t i,j;
  
    for (i = 0; i < nMs; i++)
        //����ʵ�ʲ��Կ��Եó�ִ��535��ѭ����ʱ��ӽ�1ms

        for (j = 0; j < 535; j++);
}