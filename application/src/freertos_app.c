/**
********************************************************************************
*   @file    freertos_app.c
*   @author  郝宽宽
*   @version V1.0.0
*   @date    2019-04-24
*   @brief   freertos任务
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "freertos_wrapper.h"

#include "freertos_app.h"

#include "led_app.h"
#include "beep_app.h"
#include "usart_app.h"
#include "rtc_app.h"


/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define ULONG_MAX                   0xffffffff
#define QUEUE_LENGTH                5

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TaskHandle_t xLedOnTaskHandle;
TaskHandle_t xLedOffTaskHandle;

TaskHandle_t xBeepTaskHandle;
TaskHandle_t xRtcTAskHandle;

QueueHandle_t xQueueRtc;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  解析由中断得到的数据，设置date
 * @param  msg 指针，包含要设置的date数据
 * @param  seprator 指针，分隔符
 * @return None 
 * @retval None 
 */
void hkk_set_rtc_date_cmd(common_msg *msg, const char *seprator)
{
    uint16_t date[3], i = 0;
    char *p, *q;
        
    if (0 == msg->len)
        return;

    p = (char *)msg->data;

    while ((q = strtok(p, seprator)) != NULL)
    {
        date[i++] = atoi(q);
        p = NULL;
    }

    hkk_set_rtc_date(date[0], date[1], date[2]);
    
    return ;
}

/**
 * @brief  解析从串口获取的原始数据，设置rtc时间，时分秒
 * @param  msg 串口获取的原始数据
 * @param  seprator 指针，分隔符
 * @return None 
 * @retval none
 */
void hkk_set_rtc_time_cmd(common_msg *msg, const char *seprator)
{
    uint16_t date[3], i = 0;
    char *p, *q;
        
    if (0 == msg->len)
        return;

    p = (char *)msg->data;

    while ((q = strtok(p, seprator)) != NULL)
    {
        date[i++] = atoi(q);
        p = NULL;
    }

    hkk_set_rtc_time(date[0], date[1], date[2]);
    
    return ;
}

/**
 * @brief  从中断获取元数据，解析设置定时闹钟
 * @param  msg 原始数据
 * @param  seprator 分隔符
 * @return None
 * @retval None
 */
void hkk_set_rtc_alarm_cmd(common_msg *msg, const char *seprator)
{
    uint16_t date[3], i = 0;
    char *p, *q;
        
    if (0 == msg->len)
        return;

    p = (char *)msg->data;

    while ((q = strtok(p, seprator)) != NULL)
    {
        date[i++] = atoi(q);
        p = NULL;
    }

    hkk_set_rtc_alarm(date[0], date[1], date[2]);
    
    return ;
}

static void hkk_beep_task(void * para)
{
    uint32_t ulNotifiedValue; 
    BaseType_t xResult;

    for (;;)
    {
        xResult = xTaskNotifyWait( pdFALSE, 
                                    ULONG_MAX, 
                                    &ulNotifiedValue,
                                    portMAX_DELAY );
        if( xResult == pdPASS )
        {
            switch (ulNotifiedValue)
            {
                case BEEP_ON:
                    hkk_beep_on();
                    break;
                case BEEP_OFF:
                    hkk_beep_off();
                    break;
                default:
                    break;
            }
        }
    }
}

static void hkk_led_on_task(void * para)
{
    for (;;)
    {
        xTaskNotifyWait( pdFALSE, 
                        ULONG_MAX,
                        NULL, 
                        portMAX_DELAY );
        
        printf("led on\r\n");
        hkk_led_on();
    }
}

static void hkk_led_off_task(void * para)
{
    for (;;)
    {
        xTaskNotifyWait( pdFALSE, 
                    ULONG_MAX,
                    NULL, 
                    portMAX_DELAY );
        
        printf("led off\r\n");
        hkk_led_off();
    }
}

static void hkk_rtc_task(void * para)
{
    common_msg rec_msg;
    
    for (;;)
    {
        if( xQueueReceive( xQueueRtc, &rec_msg, portMAX_DELAY ) == pdPASS )
        {
            switch (rec_msg.cmd_id)
            {
                case RTC_ALARM:
                    hkk_beep_on();
                    vTaskDelay(pdMS_TO_TICKS(500));
                    hkk_beep_off();
                    break;
                case RTC_SET_DATE:
                    hkk_set_rtc_date_cmd(&rec_msg, "-");
                    break;
                case RTC_SET_TIME:
                    hkk_set_rtc_time_cmd(&rec_msg, ":");
                    break;
                case RTC_GET_DATETIME:
                    hkk_get_rtc_datetime();
                    break;
                case RTC_SET_ALARM_TIME:
                    hkk_set_rtc_alarm_cmd(&rec_msg, ":");
                    break;
                case RTC_GET_ALARM_TIME:
                    hkk_get_rtc_alarm();
                    break;

                default:
                    break;
            }
        }
        else
        {
            printf("rtc queue receive failed.\r\n");
        }
    }
}

/**
 * @brief  将结构体数据收集打包
 * @param  msg 结构体变量指针
 * @param  cmd_id 命令ID
 * @param  length 命令数据长度 @note 可以为0，表示只有指令
 * @param  data 待拷贝数据
 * @note   在接受端一定要将堆栈释放
 * @return none
 * @retval 打包好的msg
 */
void hkk_fill_queue_msg(common_msg *msg, uint16_t cmd_id, uint16_t length, uint8_t *data)
{
    msg->cmd_id = cmd_id;

    if (0 == length)
    {
        msg->len  = 0;
        return ;
    }
    
    msg->len    = length;
    memset(msg->data, 0x00, sizeof(msg->data));
    memcpy(msg->data, data, msg->len);
}

/**
 * @brief  multitask led test
 * @return none
 * @retval none
 */
void hkk_freertos_test(void)
{
    xQueueRtc = xQueueCreate( QUEUE_LENGTH, sizeof(common_msg) );
    if( xQueueRtc == NULL )
    {
        printf("xQueueRtc create failed.\r\n");
    }
    
    xTaskCreate(hkk_led_on_task,
                "led_on",
                256,
                NULL,
                2,
                &xLedOnTaskHandle);

    xTaskCreate(hkk_led_off_task,
                "led_off",
                256,
                NULL,
                2,
                &xLedOffTaskHandle);

    xTaskCreate(hkk_beep_task,
                "beep",
                256,
                NULL,
                2,
                &xBeepTaskHandle);

    xTaskCreate(hkk_rtc_task,
                "rtc",
                256,
                NULL,
                2,
                &xRtcTAskHandle);

    return ;
}



/************************ (C) COPYRIGHT haokuankuan/郝宽宽 ******END OF FILE****/
