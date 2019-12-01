/**
********************************************************************************
*   @file    freertos_app.h
*   @author  郝宽宽
*   @version V1.0.0
*   @date    2019-04-24
*   @brief   freertos 测试任务接口
********************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FREERTOS_APP_H_
#define __FREERTOS_APP_H_

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
extern TaskHandle_t xLedOnTaskHandle;
extern TaskHandle_t xLedOffTaskHandle;

extern TaskHandle_t xBeepTaskHandle;
extern TaskHandle_t xRtcTAskHandle;

extern QueueHandle_t xQueueRtc;


/* Exported macro ------------------------------------------------------------*/
#define QUEUE_MSG_LEN 100

/** beep relative */
#define BEEP_ON  0x02
#define BEEP_OFF 0x01

/** rtc relative */
#define RTC_ALARM    0x01

#define RTC_SET_DATE 0x02
#define RTC_SET_TIME 0x03
#define RTC_GET_DATETIME 0x04

#define RTC_SET_ALARM_TIME 0x05
#define RTC_GET_ALARM_TIME 0x06

/* Exported types ------------------------------------------------------------*/    
struct QUEUE_FORMAT {
    uint16_t cmd_id; /**< 指令ID */
    uint16_t len;    /**< 数据长度 */
    uint8_t  data[QUEUE_MSG_LEN];  /**< 数据地址 */
};
typedef struct QUEUE_FORMAT common_msg;


/* Exported functions ------------------------------------------------------- */

void hkk_freertos_test(void);
void hkk_fill_queue_msg(common_msg *msg, uint16_t cmd_id, uint16_t length, uint8_t *data);

#endif /* __FREERTOS_APP_H_ */

/************************ (C) COPYRIGHT haokuankuan/郝宽宽 ******END OF FILE****/

