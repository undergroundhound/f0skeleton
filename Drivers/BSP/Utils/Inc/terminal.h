#ifndef _TERMINAL_H
#define _TERMINAL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint-gcc.h>
#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
/* --------------------------- USART HW definition -------------------------------*/

 #define TERM_USART                         USART1
 #define TERM_USART_CLK_ENABLE()            __HAL_RCC_USART1_CLK_ENABLE()
 #define TERM_USART_RX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
 #define TERM_USART_TX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
 #define TERM_USART_FORCE_RESET()           __HAL_RCC_USART1_FORCE_RESET()
 #define TERM_USART_RELEASE_RESET			__HAL_RCC___USART1_RELEASE_RESET()
 #define TERM_USART_GPIO_PORT            	GPIOA
 #define TERM_USART_TX_PIN                	GPIO_PIN_9
 #define TERM_USART_TX_AF                   GPIO_AF1_USART1
 #define TERM_USART_RX_PIN                  GPIO_PIN_10
 #define TERM_USART_RX_AF                   GPIO_AF1_USART1

 /* Definition for USARTx's NVIC */
 #define TERM_USART_IRQn                    USART1_IRQn
 #define TERM_USART_IRQHandler              USART1_IRQHandler

#ifndef VT100_OFF
#define COLOR(__c,__x)    "\x1b[3" #__c "m" __x "\x1b[0m"
#define COLOR_BOLD(__c,__x)    "\x1b[3" #__c ";1m" __x "\x1b[0m"
#define UNDERLINE(__x) "\x1b[4m" __x "\x1b[0m"
#define CLEAR_SCREEN    "\x1b[2J\x1b[H"
#else
#define COLOR(__c,__x)    __x
#define COLOR_BOLD(__c,__x) __x
#define UNDERLINE(__x) __x
#define CLEAR_SCREEN
#endif
#define RED(__x)        COLOR(1, __x )
#define GREEN(__x)        COLOR(2, __x )
#define YELLOW(__x)        COLOR(3, __x )
#define BLUE(__x)        COLOR(4, __x )
#define MAGENTA(__x)    COLOR(5, __x )
#define CYAN(__x)        COLOR(6, __x )
#define RED_B(__x)        COLOR_BOLD(1, __x )
#define GREEN_B(__x)        COLOR_BOLD(2, __x )
#define YELLOW_B(__x)        COLOR_BOLD(3, __x )
#define BLUE_B(__x)        COLOR_BOLD(4, __x )
#define MAGENTA_B(__x)    COLOR_BOLD(5, __x )
#define CYAN_B(__x)        COLOR_BOLD(6, __x )

#define printBuff(_c)	printf(#_c " : 0x%08X\n", (unsigned int)READ_REG(_c))

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct
{
    const char *cmd;
    const char *description;
    void (*cmdFunc)(uint8_t argc, char **argv);
} sTermEntry_t;

uint8_t terminal_init();
void terminal_deInit();
void terminal_ioInit();
void terminal_ioDeInit();
uint8_t terminal_run();
void terminal_handleByte(uint8_t byte, uint8_t escaped);

#ifdef __cplusplus
 }
#endif



#endif
