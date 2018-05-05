/*
 * role_master.h
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#ifndef SRC_ROLE_MASTER_H_
#define SRC_ROLE_MASTER_H_

#include "role.h"

class RoleMaster : public Role
{
    uint8_t mArmed;
    uint8_t rocketCount;
    volatile uint8_t shortPress = 0;
    volatile uint8_t longPress = 0;

    void armSlaves(uint8_t armed);

public:
    RoleMaster(BiLED *led, NodeInterface *nodeInterface);
    void buttonCallback(uint8_t state);
    void run();
};

#endif /* SRC_ROLE_MASTER_H_ */
