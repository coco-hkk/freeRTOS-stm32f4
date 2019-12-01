/**
********************************************************************************
*   @file    rtc_app.h
*   @author  郝宽宽
*   @version V1.0.0
*   @date    2019-04-24
*   @brief   rtc接口
********************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_APP_H_
#define __RTC_APP_H_

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void hkk_rtc_init(void);
void hkk_get_rtc_datetime(void);
void hkk_get_rtc_alarm(void);
void hkk_set_rtc_date(uint16_t year, uint16_t month, uint16_t day);
void hkk_set_rtc_time(uint16_t hour, uint16_t min, uint16_t second);
void hkk_set_rtc_alarm(uint16_t hour, uint16_t min, uint16_t sec);

#endif /* __RTC_APP_H_ */


/************************ (C) COPYRIGHT haokuankuan/郝宽宽 ******END OF FILE****/


