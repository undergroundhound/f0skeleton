/*
 * bi_led_2.h
 *
 *  Created on: 01 Jun 2018
 *      Author: christo
 */

#ifndef SRC_BI_LED_2_H_
#define SRC_BI_LED_2_H_

#include "output.h"

#define LED_RED     0
#define LED_GREEN   1

typedef struct{
    uint16_t delay;
    uint8_t flashByte;
} sLEDflash_t;

const sLEDflash_t LED_ON         = {0xFFFF,  0xFF};
const sLEDflash_t LED_OFF        = {0xFFFF, 0x00};
const sLEDflash_t LED_HEARTBEAT  = {125,    0b10100000};
const sLEDflash_t LED_FAST_FLASH = {100,    0b10101010};
const sLEDflash_t LED_SLOW_FLASH = {750,    0b11001100};

class BiLED2
{
    cOutput *mGreen;
    cOutput *mRed;

    sLEDflash_t mCurrSequence;
    uint8_t mLedColour;
    uint8_t mTickCount;
    uint32_t mLastLick;

public:
    BiLED2(cOutput *green, cOutput *red);

    void setFlash(sLEDflash_t seqeunce, uint8_t colour);
    void off();
    void run();
};

#endif /* SRC_BI_LED_2_H_ */
