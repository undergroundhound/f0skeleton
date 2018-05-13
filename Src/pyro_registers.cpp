/*
 * pyro_registers.cpp
 *
 *  Created on: 11 May 2018
 *      Author: christo
 */

#include "pyro_registers.h"
#include <string.h>

PyroRegisters::PyroRegisters()
{
    memset(mRegisters, 0xFF, PYRO_REGISTER_COUNT);
}

void PyroRegisters::setRegister(uint8_t reg, uint8_t value)
{
    if(reg > PYRO_REGISTER_COUNT)
        return;

    memcpy(&mRegisters[reg], &value, 1);
}

void PyroRegisters::getRegister(uint8_t reg, uint8_t *value)
{
    if(reg > PYRO_REGISTER_COUNT)
        return;

    memcpy(value, &mRegisters[reg], 1);
}
