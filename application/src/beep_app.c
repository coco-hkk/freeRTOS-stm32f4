/**
********************************************************************************
*   @file    beep_app.c
*   @author  郝宽宽
*   @version V1.0.0
*   @date    2019-04-24
*   @brief   蜂鸣器模块
********************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_gpio.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief  蜂鸣器开
 * @return none
 * @retval none
 */
void hkk_beep_on(void)
{
    GPIO_SetBits(GPIOG, GPIO_Pin_7);

    return ;
}

/**
 * @brief  蜂鸣器关
 * @return none
 * @retval none
 */
void hkk_beep_off(void)
{
    GPIO_ResetBits(GPIOG,GPIO_Pin_7);

    return ;
}

/**
 * @brief  蜂鸣器初始化
 * @return none
 * @retval none
 */
void hkk_beep_init(void)
{   
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);
  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;       
    GPIO_Init(GPIOG, &GPIO_InitStructure);               
	
    GPIO_ResetBits(GPIOG,GPIO_Pin_7);                    
}


/************************ (C) COPYRIGHT haokuankuan/郝宽宽 ******END OF FILE****/
