
#ifndef INC_HW_MSP_H_
#define INC_HW_MSP_H_


/*!
 * \brief GPIOs Macro
 */

#define RCC_GPIO_CLK_ENABLE( __GPIO_PORT__ )              \
do {                                                    \
    switch( __GPIO_PORT__)                                \
    {                                                     \
      case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_ENABLE(); break;    \
      case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_ENABLE(); break;    \
      case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_ENABLE(); break;    \
      case GPIOD_BASE: default:  __HAL_RCC_GPIOD_CLK_ENABLE(); \
    }                                                    \
  } while(0)

#define RCC_GPIO_CLK_DISABLE( __GPIO_PORT__ )              \
do {                                                    \
    switch( __GPIO_PORT__)                                \
    {                                                     \
      case GPIOA_BASE: __HAL_RCC_GPIOA_CLK_DISABLE(); break;    \
      case GPIOB_BASE: __HAL_RCC_GPIOB_CLK_DISABLE(); break;    \
      case GPIOC_BASE: __HAL_RCC_GPIOC_CLK_DISABLE(); break;    \
      case GPIOD_BASE: default:  __HAL_RCC_GPIOD_CLK_ENABLE(); \
    }                                                    \
  } while(0)




#endif /* INC_HW_MSP_H_ */
