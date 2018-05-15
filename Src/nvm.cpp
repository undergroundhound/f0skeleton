/*
 * nvm.cpp
 *
 *  Created on: 21 Apr 2018
 *      Author: christo
 */

#include "nvm.h"

NVM::NVM(I2C *device) : mDevice(device)
{

}

NVM::~NVM()
{

}


void NVM::get(sNvm_t *nvm)
{
    mDevice->read(NVM_ADDR, (uint8_t *)nvm, sizeof(sNvm_t));
}

void NVM::set(sNvm_t *nvm)
{
    mDevice->write(NVM_ADDR, (uint8_t *)nvm, sizeof(sNvm_t));
}
