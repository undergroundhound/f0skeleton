/*
 * role_slave.h
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#ifndef SRC_ROLE_SLAVE_H_
#define SRC_ROLE_SLAVE_H_

#include "role.h"

#include "adc.h"

#define OUTPUT_OPEN_ADC     100

class RoleSlave : public Role
{
    BiLED2 **leds;

    uint8_t rxData[4];
    uint8_t mArmed;
    uint8_t mStatus;
    uint32_t mLastTick;
    uint32_t mConnectionTimeout;

    void arm(uint8_t state);

    cOutput *outputs[4];
    uint32_t timeOuts[4];

    cOutput out1 = cOutput(GPIOB, GPIO_PIN_0);
    cOutput out2 = cOutput(GPIOA, GPIO_PIN_6);
    cOutput out3 = cOutput(GPIOA, GPIO_PIN_4);
    cOutput out4 = cOutput(GPIOA, GPIO_PIN_2);
    cADC mAdc;

    void checkConnections();
public:
    RoleSlave(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount);
    void buttonCallback(uint8_t state);
    void debug(uint8_t argc, char **argv);
    void run();
};

#endif /* SRC_ROLE_SLAVE_H_ */
