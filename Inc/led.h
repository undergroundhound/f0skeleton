/*
 * led.h
 *
 *  Created on: 21 Apr 2018
 *      Author: christo
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "output.h"

typedef struct{
    uint16_t delay;
    uint8_t flashByte;
} sLEDflash_t;

const sLEDflash_t LED_OFF        = {0xFFFF, 0b00000000};
const sLEDflash_t LED_HEARTBEAT  = {125,    0b10100000};
const sLEDflash_t LED_FAST_FLASH = {75,     0b10101010};
const sLEDflash_t LED_SLOW_FLASH = {250,    0b11001100};


class LED : public cOutput
{
    sLEDflash_t mCurrSequence;
    uint8_t mTickCount;
    uint32_t mLastLick;
public:
    LED(GPIO_TypeDef *port, uint32_t pin);

    void setFlash(sLEDflash_t seqeunce);
    void run();

    virtual ~LED();
};

#endif /* INC_LED_H_ */
