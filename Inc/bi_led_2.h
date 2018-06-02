/*
 * bi_led_2.h
 *
 *  Created on: 01 Jun 2018
 *      Author: christo
 */

#ifndef SRC_BI_LED_2_H_
#define SRC_BI_LED_2_H_

#include "output.h"

#define BILED2_FLASH_GREEN 0
#define BILED2_FLASH_RED 1

class BiLED2
{
    cOutput *mGreen;
    cOutput *mRed;

    bool mEnabled;
    uint32_t mFlashOffTick;

    void reset();
public:
    BiLED2(cOutput *green, cOutput *red);

    void flash(uint8_t flash, uint32_t timeout);
    void run();
};

#endif /* SRC_BI_LED_2_H_ */
