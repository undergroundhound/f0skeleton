/*
 * bi_led_2.cpp
 *
 *  Created on: 01 Jun 2018
 *      Author: christo
 */

#include "bi_led_2.h"

void BiLED2::reset()
{
    mGreen->reset();
    mRed->reset();
}

BiLED2::BiLED2(cOutput *green, cOutput *red) : mGreen(green), mRed(red)
{
    mFlashOffTick = 0;
    mEnabled = false;
}

void BiLED2::flash(uint8_t flash, uint32_t timeout)
{
    reset();
    mFlashOffTick = HAL_GetTick() + timeout;
    mEnabled = true;

    switch (flash) {
        case BILED2_FLASH_GREEN:
        {
            mGreen->set();
        }
        break;
        case BILED2_FLASH_RED:
        {
            mRed->set();
        }
        break;
        default:
            break;
    }
}

void BiLED2::run()
{
    if(!mEnabled)
        return;

    if(HAL_GetTick() < mFlashOffTick)
        return;

    reset();
    mEnabled = false;
}
