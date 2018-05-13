/*
 * con_check.cpp
 *
 *  Created on: 12 May 2018
 *      Author: christo
 */

#include "con_check.h"

ConCheck::ConCheck(cADC *adc, uint8_t channel, uint8_t validAdc) : mAdc(adc), mChannel(channel), mValidAdc(validAdc)
{

}

uint8_t ConCheck::check()
{
    if(mAdc->sampleChannel(mChannel) > mValidAdc)
        return 1;

    return 0;
}
