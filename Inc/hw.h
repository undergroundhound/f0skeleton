#ifndef INC_HW_H_
#define INC_HW_H_

//#include "stm32f0xx_hal_conf.h"
#include "stm32f0xx_hal.h"
#include "hw_msp.h"
#include "hw_gpio.h"
#include "terminal.h"

#include <stdio.h>

#define PrintInfo(_info) printf("%15s : ", _info)

void SystemClock_Config(void);
void MX_GPIO_Init(void);


#endif /* INC_HW_H_ */
