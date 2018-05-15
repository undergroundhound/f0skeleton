/*
 * i2c.h
 *
 *  Created on: 13 May 2018
 *      Author: christo
 */

#ifndef DRIVERS_BSP_I2C_H_
#define DRIVERS_BSP_I2C_H_

#include "hw.h"
class I2C
{
    I2C_HandleTypeDef mHandle;
    bool mInitialized;
public:
    I2C();
    void init();

    HAL_StatusTypeDef write(uint16_t addr,uint8_t *data, uint8_t len);
    HAL_StatusTypeDef read(uint16_t addr,uint8_t *data, uint8_t len);
};

#endif /* DRIVERS_BSP_I2C_H_ */
