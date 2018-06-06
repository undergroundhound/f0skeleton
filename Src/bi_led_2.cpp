/*
 * bi_led_2.cpp
 *
 *  Created on: 01 Jun 2018
 *      Author: christo
 */

#include "bi_led_2.h"

void BiLED2::off()
{
    mGreen->reset();
    mRed->reset();
}

BiLED2::BiLED2(cOutput *green, cOutput *red) : mGreen(green), mRed(red)
{
    mTickCount = 0;
    mLastLick =0 ;
    mLedColour = 0;
}

void BiLED2::setFlash(sLEDflash_t seqeunce, uint8_t colour)
{
//    if((mCurrSequence.delay == seqeunce.delay) && (mCurrSequence.flashByte == seqeunce.flashByte))
//        return;

    off();
    if(colour > 1)
        return;

    mCurrSequence = seqeunce;
    mLedColour = colour;
//    mTickCount = 0;
}

void BiLED2::run()
{
    cOutput *out = 0;
    if(mLedColour == LED_GREEN)
        out = mGreen;
    else if(mLedColour == LED_RED)
        out = mRed;

    if(mCurrSequence.delay == 0xFFFF)
    {
        if(mCurrSequence.flashByte == 0x00)
            out->reset();
        else
            out->set();

        mTickCount = 0;
        return;
    }

    if(mTickCount > 7)
        mTickCount = 0;

    if(mLastLick > HAL_GetTick())
        return;

    mLastLick = HAL_GetTick() + mCurrSequence.delay;

    if(((mCurrSequence.flashByte >> mTickCount) & 0x01) == 0x01)
        out->set();
    else
        out->reset();

    mTickCount++;
}
