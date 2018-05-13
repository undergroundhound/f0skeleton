/*
 * adc.h
 *
 *  Created on: 06 Dec 2017
 *      Author: cfurter
 */

#ifndef ADC_H_
#define ADC_H_

#include "hw.h"

class cADC
{
    bool mInitialized;
    ADC_HandleTypeDef hadc;
public:
    virtual ~cADC();
    cADC();

    void init();
    void deInit();

    uint16_t sampleChannel(uint32_t channel);
};

#endif /* ADC_H_ */
