/*
 * led.cpp
 *
 *  Created on: 21 Apr 2018
 *      Author: christo
 */

#include "led.h"

LED::LED(GPIO_TypeDef *port, uint32_t pin) : cOutput(port, pin)
{
    mCurrSequence = LED_OFF;
    mTickCount = 0;
    mLastLick = 0;
}

LED::~LED()
{
    // TODO Auto-generated destructor stub
}

void LED::setFlash(sLEDflash_t sequence)
{
    mCurrSequence = sequence;
}

void LED::run()
{
    if(mCurrSequence.delay == 0xFFFF)
    {
        if(!get())
            set();

        mTickCount = 0;
        return;
    }

    if(mTickCount > 7)
        mTickCount = 0;

    if(mLastLick > HAL_GetTick())
        return;

    mLastLick = HAL_GetTick() + mCurrSequence.delay;

    if(((mCurrSequence.flashByte >> mTickCount) & 0x01) == 0x01)
        reset();
    else
        set();

    mTickCount++;
}
