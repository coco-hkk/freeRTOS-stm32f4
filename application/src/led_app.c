/**
********************************************************************************
* @file    led_app.c
* @author  郝宽宽
* @version V1.0.0
* @date    2019-04-23
* @brief   led应用
********************************************************************************
*/

/* Includes  -----------------------------------------------------------------*/
#include <stdio.h>

#include "stm324xg_eval.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  led初始化
 * @return null
 * @retval null
 */
void hkk_led_init(void)
{
    STM_EVAL_LEDInit(LED1);

    return;
}

/**
 * @brief  led on
 * @return None
 * @retval None 
 */
void hkk_led_on(void)
{
    STM_EVAL_LEDOn(LED1);

    return ;
}

/**
 * @brief  led off
 * @return None 
 * @retval None 
 */
void hkk_led_off(void)
{
    STM_EVAL_LEDOff(LED1);

    return ;
}

/************************ (C) COPYRIGHT haokuankuan/郝宽宽 ******END OF FILE****/

