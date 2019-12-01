/**
********************************************************************************
*   @file    rtc_app.c
*   @author  郝宽宽
*   @version V1.0.0
*   @date    2019-04-24
*   @brief   rtc模块
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "stm324xg_eval.h"
#include "stm32f4xx_rtc.h"

#include "rtc_app.h"
#include "freertos_wrapper.h"
#include "freertos_app.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t set_time_flag = 0;

RTC_TimeTypeDef  RTC_TimeStructure;
RTC_InitTypeDef  RTC_InitStructure;
RTC_AlarmTypeDef RTC_AlarmStructure;
RTC_DateTypeDef RTC_DateStructure;



/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  十进制转换为相同数的十六进制，如in 16 => out 0x16
 * @param  dec 十进制
 * @return 十六进制
 * @retval 十六进制
 */
uint16_t hkk_dec2hex(uint16_t dec)
{
    uint16_t hex = 0;
	uint32_t tmp = dec;
    uint8_t  i;

    for (i = 0; i < 3; i++)
    {
        hex |= (tmp / 100) % 10;
        
        if (2 == i)
            break;
        
        hex <<= 4;
        tmp = tmp * 10;
    }
    
    return hex;
}

/**
  * @brief  Display the current time.
  * @param  None
  * @retval None
  */
void hkk_get_rtc_datetime(void)
{
    char datetime[50] = {0};
    /* Get the current Time */
    RTC_GetTime(RTC_Format_BIN, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BIN, &RTC_DateStructure);

    /* Display time Format : hh:mm:ss */
    sprintf((char *)datetime, "20%0.2d-%0.2d-%0.2d %0.2d:%0.2d:%0.2d",
                            RTC_DateStructure.RTC_Year,
                            RTC_DateStructure.RTC_Month,
                            RTC_DateStructure.RTC_Date,
                            RTC_TimeStructure.RTC_Hours, 
                            RTC_TimeStructure.RTC_Minutes, 
                            RTC_TimeStructure.RTC_Seconds);
    printf("%s\r\n", datetime);
}

/**
  * @brief  Display the current Alarm.
  * @param  None
  * @retval None
  */
void hkk_get_rtc_alarm(void)
{
    char alarm[50] = {0};
    /* Get the current Alarm */
    RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
    sprintf((char *)alarm, "%0.2d:%0.2d:%0.2d", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
    printf("%s\r\n", alarm);
}

/**
 * @brief  设置年月日
 * @param  year 年
 * @param  month
 * @param  day
 * @return none
 * @retval none
 */
void hkk_set_rtc_date(uint16_t year, uint16_t month, uint16_t day)
{
    RTC_DateTypeDef RTC_DateStructure;
    
    RTC_DateStructure.RTC_Year = hkk_dec2hex(year) & 0xFF;
    RTC_DateStructure.RTC_Month = hkk_dec2hex(month) & 0xFF;
    RTC_DateStructure.RTC_Date = hkk_dec2hex(day) & 0xFF;
    
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

    return ;
}

/**
 * @brief  设置时分秒
 * @param  hour 小时
 * @param  min 分钟
 * @param  second 秒
 * @return none
 * @retval none
 */
void hkk_set_rtc_time(uint16_t hour, uint16_t min, uint16_t second)
{
    RTC_TimeTypeDef  RTC_TimeStructure;

    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = hkk_dec2hex(hour) & 0xFF;
    RTC_TimeStructure.RTC_Minutes = hkk_dec2hex(min) & 0xFF;
    RTC_TimeStructure.RTC_Seconds = hkk_dec2hex(second) & 0xFF; 

    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   

    return;
}

/**
 * @brief  从串口获取原始数据，解析并设定闹钟
 * @param  hour 小时
 * @param  min 分钟
 * @param  sec 秒
 * @return none
 * @retval none
 */
void hkk_set_rtc_alarm(uint16_t hour, uint16_t min, uint16_t sec)
{
    RTC_AlarmTypeDef RTC_AlarmStructure;

    RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = hkk_dec2hex(hour) & 0xFF;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = hkk_dec2hex(min) & 0xFF;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = hkk_dec2hex(sec) & 0xFF;
    RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

    /* 先除能alarm a然后设置，然后使能 */
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

    return ;
}

/**
  * @brief  This function handles RTC Alarm interrupt A request.
  * @param  None
  * @retval None
  */
void RTC_Alarm_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    common_msg alarm_notify;

    /* Check on the Alarm A flag and on the number of interrupts per Second (60*8) */
    if(RTC_GetITStatus(RTC_IT_ALRA) != RESET) 
    {
        hkk_fill_queue_msg(&alarm_notify, RTC_ALARM, 0, NULL);
        xQueueSendFromISR( xQueueRtc, &alarm_notify, &xHigherPriorityTaskWoken );
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
    
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

/**
  * @brief  Configures the RTC peripheral and select the clock source.
  * @param  None
  * @retval None
  */
static void hkk_rtc_config(void)
{
  
    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);

    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);

    /* Wait till LSE is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }

    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);

    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    /* ck_spre(1Hz) = RTCCLK(LSE) /(AsynchPrediv + 1)*(SynchPrediv + 1)*/
    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
    RTC_InitStructure.RTC_SynchPrediv = 0xFF;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);
}

/**
  * @brief  Configures the RTC Alarm.
  * @param  None
  * @retval None
  */
static void hkk_rtc_alarm_config(void)
{
    /* Set the alarm 05h:30min:30s */
    RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x05;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x45;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x00;
    RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

    /* Configure the RTC Alarm A register */
    RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

    /* Enable RTC Alarm A Interrupt */
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);

    /* Enable the alarm */
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

    RTC_ClearFlag(RTC_FLAG_ALRAF);

    /* Set the date: Thursday April 19th 2019 */
    RTC_DateStructure.RTC_Year = 0x19;
    RTC_DateStructure.RTC_Month = RTC_Month_April;
    RTC_DateStructure.RTC_Date = 0x24;
    RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Thursday;
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

    /* Set the time to 00h 00mn 00s AM */
    RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
    RTC_TimeStructure.RTC_Hours   = 0x00;
    RTC_TimeStructure.RTC_Minutes = 0x00;
    RTC_TimeStructure.RTC_Seconds = 0x00; 

    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   

    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);
}

/**
  * @brief  Configures the RTC Alarm interrupt.
  * @param  None
  * @retval None
  */
static void hkk_rtc_alarm_ti_config(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* EXTI configuration */
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Alarm Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


/**
 * @brief  rtc init
 * @return none
 * @retval none
 */
void hkk_rtc_init(void)
{
    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2 || set_time_flag)
    {  
        /* RTC configuration  */
        hkk_rtc_config();
        hkk_rtc_alarm_config();
    }
    else
    {
        /* Check if the Power On Reset flag is set */
        if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
        {
            /* Power On Reset occurred     */
            STM_EVAL_LEDOff(LED1);
        }
        /* Check if the Pin Reset flag is set */
        else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
        {
            /* External Reset occurred */
            STM_EVAL_LEDOff(LED1);
        }

        /* Enable the PWR clock */
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

        /* Allow access to RTC */
        PWR_BackupAccessCmd(ENABLE);

        /* Wait for RTC APB registers synchronisation */
        RTC_WaitForSynchro();

        /* Clear the RTC Alarm Flag */
        RTC_ClearFlag(RTC_FLAG_ALRAF);

        /* Clear the EXTI Line 17 Pending bit (Connected internally to RTC Alarm) */
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
    
    hkk_rtc_alarm_ti_config();

    return ;
}

/************************ (C) COPYRIGHT haokuankuan/郝宽宽 ******END OF FILE****/


