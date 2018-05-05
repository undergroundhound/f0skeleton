/*
 * output.h
 *
 *  Created on: 10 Dec 2017
 *      Author: christo
 */

#ifndef DRIVERS_BSP_UTILS_OUTPUT_H_
#define DRIVERS_BSP_UTILS_OUTPUT_H_

#include <stdbool.h>

#include "stm32f0xx_hal.h"
class cOutput {
	uint32_t mPin;
	GPIO_TypeDef *mPort;
public:
	cOutput(GPIO_TypeDef *port, uint32_t pin);
	virtual ~cOutput();

	void set();
	void reset();
	bool get();
	void toggle();
};

#endif /* DRIVERS_BSP_UTILS_OUTPUT_H_ */
