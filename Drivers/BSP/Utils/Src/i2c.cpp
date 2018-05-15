/*
 * i2c.cpp
 *
 *  Created on: 13 May 2018
 *      Author: christo
 */

#include "i2c.h"


I2C::I2C()
{
    mInitialized = false;
}

void I2C::init()
{
    mHandle.Instance = I2C1;
    mHandle.Init.Timing = 0x0000020B;
    mHandle.Init.OwnAddress1 = 0;
    mHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    mHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
//    mHandle.Init.OwnAddress2 = 0;
//    mHandle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
//    mHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
//    mHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&mHandle) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }
      /**Configure Analogue filter
      */
    if (HAL_I2CEx_ConfigAnalogFilter(&mHandle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

      /**Configure Digital filter
      */
    if (HAL_I2CEx_ConfigDigitalFilter(&mHandle, 0) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    HAL_Delay(10);

    printf("devReady: %d\n", HAL_I2C_IsDeviceReady(&mHandle, 0xA0, 5, 0xFFFF));
}

HAL_StatusTypeDef I2C::write(uint16_t addr,uint8_t *data, uint8_t len)
{
    uint16_t deviceAddr = 0xA0;
    HAL_Delay(10);
    return HAL_I2C_Mem_Write(&mHandle, deviceAddr, addr,I2C_MEMADD_SIZE_8BIT, data, len, 0xFFFFFF);
}

HAL_StatusTypeDef I2C::read(uint16_t addr,uint8_t *data, uint8_t len)
{
    uint16_t deviceAddr = 0xA0;
    HAL_Delay(10);
    return HAL_I2C_Mem_Read(&mHandle, deviceAddr, addr,I2C_MEMADD_SIZE_8BIT, data, len, 0xFFFFFF);
}


