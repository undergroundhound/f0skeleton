#ifndef INC_HW_H_
#define INC_HW_H_

//#include "stm32f0xx_hal_conf.h"
#include "stm32f0xx_hal.h"
#include "hw_msp.h"
#include "hw_gpio.h"

void SystemClock_Config(void);
void MX_GPIO_Init(void);

#endif /* INC_HW_H_ */
