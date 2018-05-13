/*
 * pyro_registers.h
 *
 *  Created on: 11 May 2018
 *      Author: christo
 */

#ifndef SRC_PYRO_REGISTERS_H_
#define SRC_PYRO_REGISTERS_H_

#include "hw.h"

#define PYRO_REGISTER_COUNT      4

typedef enum{
    P_REG_ADC1,
    P_REG_ADC2,
    P_REG_ADC3,
    P_REG_ADC4
} ePyroRegister_t;

class PyroRegisters
{
    uint8_t mRegisters[PYRO_REGISTER_COUNT];
public:
    PyroRegisters();
    void setRegister(uint8_t reg, uint8_t value);
    void getRegister(uint8_t reg, uint8_t *value);
};

#endif /* SRC_PYRO_REGISTERS_H_ */
