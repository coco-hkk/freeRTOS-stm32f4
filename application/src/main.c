/* include */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f4xx.h"
#include "freertos_wrapper.h"
#include "main.h"
#include "led_app.h"
#include "usart_app.h"
#include "beep_app.h"
#include "freertos_app.h"
#include "rtc_app.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  优先级分组、系统时钟初始化
 * @return null
 * @retval null
 */
static void hkk_system_init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    SysTick_Config(SystemCoreClock);
    
    return ;
}

int main(void)
{
    hkk_system_init();
    hkk_usart_init();
    hkk_led_init();
    hkk_beep_init();
    hkk_rtc_init();

    hkk_freertos_test();
   
    vTaskStartScheduler();

    for(;;);
}

/************************ (C) COPYRIGHT haokuankuan/郝宽宽 ******END OF FILE****/

