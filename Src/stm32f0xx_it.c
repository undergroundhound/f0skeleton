/**
  ******************************************************************************
  * @file    stm32f0xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_it.h"
#include "terminal.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Interruption and Exception Handlers         */ 
/******************************************************************************/

/**
* @brief This function handles System tick timer.
*/
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f0xx.s).                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */
void USART2_IRQHandler(void)
{
    static uint8_t escaped;
    if (USART2->ISR & UART_FLAG_RXNE)
    {
        USART2->ICR = 0xFF;
        uint8_t c = USART2->RDR;

        // escape character
        if(c == 0x1B)
        {
            escaped++;
            return;
        }

        if(escaped)
        {
            //escape the next byte
            if(escaped++ > 1)
            {
                terminal_handleByte(0, c);
                escaped = 0;
            }
            return;
        }

        //echo typed character
        USART2->TDR = c;
        while (!(USART2->ISR & USART_ISR_TC));

        if (c == '\r')
        {
            USART2->TDR = '\n';
            while (!(USART2->ISR & USART_ISR_TC));
        }

        terminal_handleByte(c, 0);
    }
}

void EXTI0_1_IRQHandler( void )
{
  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_0 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_1 );
}

void EXTI2_3_IRQHandler( void )
{
  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_2 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_3 );
}


void EXTI4_15_IRQHandler( void )
{
  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_4 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_5 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_6 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_7 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_8 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_9 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_10 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_11 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_12 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_14 );

  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_15 );
}


/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
