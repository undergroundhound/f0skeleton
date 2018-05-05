/*
 * bi_led.h
 *
 *  Created on: 02 May 2018
 *      Author: christo
 */

#ifndef BI_LED_H_
#define BI_LED_H_

#include "led.h"

typedef struct{
    uint16_t delay;
    uint8_t redFlashByte;
    uint8_t greenFlashByte;
} sBiLEDflash_t;

const sBiLEDflash_t BILED_SLOW_FUCKAROUND = {250, \
        0b10101010, \
        0b01010101};

typedef enum{
    LED_RED,
    LED_GREEN
}eLedColour_t;

class BiLED
{
    LED *mRed;
    LED *mGreen;

public:
    BiLED(LED *red, LED *green);
    virtual ~BiLED();

    void setFlasher(sBiLEDflash_t sequence);
    void setFlash(eLedColour_t colour, sLEDflash_t seqeunce);
    void run();
};

#endif /* BI_LED_H_ */
