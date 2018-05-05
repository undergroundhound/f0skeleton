/*
 * nvm.cpp
 *
 *  Created on: 21 Apr 2018
 *      Author: christo
 */

#include "nvm.h"

NVM::NVM(cSpiDevice *device)
{
    mDevice = device;
}

NVM::~NVM()
{

}

void NVM::erase()
{
    mDevice->erase(NVM_ADDR, 4);
}

void NVM::get(sNvm_t *nvm)
{
    mDevice->read(NVM_ADDR, (uint8_t *)nvm, sizeof(sNvm_t));
}

void NVM::set(sNvm_t *nvm)
{
    erase();
    while(mDevice->isReady() == HAL_BUSY)
        HAL_Delay(50);
    mDevice->write(NVM_ADDR, (uint8_t *)nvm, sizeof(sNvm_t));
}
