/*
 * role_slave.h
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#ifndef SRC_ROLE_SLAVE_H_
#define SRC_ROLE_SLAVE_H_

#include "role.h"
#include "pyro_registers.h"

//#include "adc.h"
//#include "con_check.h"

class RoleSlave : public Role
{
    uint8_t rxData[4];
    uint8_t mArmed;

    void arm(uint8_t state);
    PyroRegisters slaveRegisters;

//    cADC mAdc;
//    ConCheck *conCheckers;
//    LED ledR = LED(GPIOA, GPIO_PIN_1);
//    LED ledG = LED(GPIOA, GPIO_PIN_0);
//    BiLED led1 = BiLED(&ledR, &ledG);

public:
    RoleSlave(BiLED *led, NodeInterface *nodeInterface);
    void buttonCallback(uint8_t state);
    void run();
};

#endif /* SRC_ROLE_SLAVE_H_ */
