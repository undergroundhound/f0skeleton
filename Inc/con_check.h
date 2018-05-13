/*
 * con_check.h
 *
 *  Created on: 12 May 2018
 *      Author: christo
 */

#ifndef SRC_CON_CHECK_H_
#define SRC_CON_CHECK_H_

#include "hw.h"
#include "adc.h"

class ConCheck
{
    cADC *mAdc;
    uint8_t mChannel;
    uint8_t mValidAdc;
public:
    ConCheck(cADC *adc, uint8_t channel, uint8_t validAdc);

    uint8_t check();
};

#endif /* SRC_CON_CHECK_H_ */
