/**
********************************************************************************
*   @file    usart_app.c
*   @author  郝宽宽
*   @version V1.0.0
*   @date    2019-04-23
*   @brief   usart模块
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm324xg_eval.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "freertos_wrapper.h"

#include "beep_app.h"
#include "freertos_app.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define FPUTC int fputc(int ch, FILE *f)

/* Private macro -------------------------------------------------------------*/
#define USART1_REC_NUM  			100

/* Private variables ---------------------------------------------------------*/
uint8_t uart_recbuffer[USART1_REC_NUM];
uint8_t uart_byte_cnt = 0;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  FPUTC fputc实现
 * @param  ch 发送字符
 * @param  f 实现中不需要
 * @return None
 * @retval None
 */
FPUTC
{
    USART_SendData(USART1, ch);

    while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TC) == RESET);

    return ch;
}

/**
 * @brief  usart1 isr
 * @return none
 * @retval none
 */
void USART1_IRQHandler(void)
{
	uint8_t rec_char;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    common_msg send_data;

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		rec_char = (uint8_t)USART_ReceiveData(USART1);
        if(rec_char == '<')
		{
			uart_byte_cnt=0x01;
		}
		else if(rec_char == '>')
		{
			if (strcmp("led on", (char *)uart_recbuffer) == 0)
            {
                xTaskNotifyFromISR( xLedOnTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken );
            }
			else if (strcmp("led off", (char *)uart_recbuffer) == 0)
            {
                xTaskNotifyFromISR( xLedOffTaskHandle, 0, eNoAction, &xHigherPriorityTaskWoken );
            }
            else if (strcmp("beep on", (char *)uart_recbuffer) == 0)
            {
                xTaskNotifyFromISR( xBeepTaskHandle, BEEP_ON, eSetBits, &xHigherPriorityTaskWoken );
            }
            else if (strcmp("beep off", (char *)uart_recbuffer) == 0)
            {
                xTaskNotifyFromISR( xBeepTaskHandle, BEEP_OFF, eSetBits, &xHigherPriorityTaskWoken );
            }
            else if (strcmp("get datetime", (char *)uart_recbuffer) == 0)
            {
                hkk_fill_queue_msg(&send_data, RTC_GET_DATETIME, 0, NULL);
                xQueueSendFromISR( xQueueRtc, &send_data, &xHigherPriorityTaskWoken );
            }
            else if (strcmp("get alarmtime", (char *)uart_recbuffer) == 0)
            {
                hkk_fill_queue_msg(&send_data, RTC_GET_ALARM_TIME, 0, NULL);
                xQueueSendFromISR( xQueueRtc, &send_data, &xHigherPriorityTaskWoken );
            }
            else if (strncmp("set date", (char *)uart_recbuffer, strlen("set date")) == 0)
            {
                hkk_fill_queue_msg(&send_data, RTC_SET_DATE, uart_byte_cnt - (1 + strlen("set date")), &uart_recbuffer[strlen("set date")]);
                xQueueSendFromISR( xQueueRtc, &send_data, &xHigherPriorityTaskWoken );
            }
            else if (strncmp("set time", (char *)uart_recbuffer, strlen("set time")) == 0)
            {
                hkk_fill_queue_msg(&send_data, RTC_SET_TIME, uart_byte_cnt - (1 + strlen("set time")), &uart_recbuffer[strlen("set time")]);
                xQueueSendFromISR( xQueueRtc, &send_data, &xHigherPriorityTaskWoken );
            }
            else if (strncmp("set alarm", (char *)uart_recbuffer, strlen("set alarm")) == 0)
            {
                hkk_fill_queue_msg(&send_data, RTC_SET_ALARM_TIME, uart_byte_cnt - (1 + strlen("set alarm")), &uart_recbuffer[strlen("set alarm")]);
                xQueueSendFromISR( xQueueRtc, &send_data, &xHigherPriorityTaskWoken );
            }

            memset(uart_recbuffer, 0x00, USART1_REC_NUM);
			uart_byte_cnt = 0;
            
            portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
	    }
		else if((uart_byte_cnt > 0) && (uart_byte_cnt <= USART1_REC_NUM))
		{
			uart_recbuffer[uart_byte_cnt - 1] = rec_char;
			uart_byte_cnt++;
		}
   }
}


/**
 * @brief  usart1配置
 * @return None
 * @retval None
 */
static void hkk_usart1_config(void)
{
    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    STM_EVAL_COMInit(COM1, &USART_InitStructure);

    return ;
}

/**
 * @brief  usart1中断配置
 * @return None
 * @retval None
 */
static void hkk_usart1_it_config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    USART_ClearFlag(USART1, USART_FLAG_TC);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    return ;
}

/**
 * @brief  usart init
 * @return None
 * @retval None
 */
void hkk_usart_init(void)
{
    hkk_usart1_config();
    hkk_usart1_it_config();

    return ;
}

/************************ (C) COPYRIGHT haokuankuan/郝宽宽 ******END OF FILE****/

