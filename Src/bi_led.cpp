/*
 * bi_led.cpp
 *
 *  Created on: 02 May 2018
 *      Author: christo
 */

#include "bi_led.h"

BiLED::BiLED(LED *red, LED *green)
{
    mRed = red;
    mGreen = green;
}

BiLED::~BiLED()
{
    // TODO Auto-generated destructor stub
}

void BiLED::setFlasher(sBiLEDflash_t sequence)
{
    sLEDflash_t seq;
    seq.delay = sequence.delay;
    seq.flashByte = sequence.greenFlashByte;
    mGreen->setFlash(seq);

    seq.flashByte = sequence.redFlashByte;
    mRed->setFlash(seq);
}

void BiLED::setFlash(eLedColour_t colour, sLEDflash_t seqeunce)
{
    if(colour == LED_GREEN)
    {
        mGreen->setFlash(seqeunce);
        mRed->setFlash(LED_OFF);
    }
    else
    {
        mGreen->setFlash(LED_OFF);
        mRed->setFlash(seqeunce);
    }
}

void BiLED::run()
{
    mRed->run();
    mGreen->run();
}
