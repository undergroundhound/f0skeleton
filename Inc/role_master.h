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
    BiLED2 **leds;

    uint32_t nextPoll;
    uint8_t mArmed;
    uint8_t rocketCount;
    volatile uint8_t shortPress = 0;
    volatile uint8_t longPress = 0;

    uint8_t rxData[4];
    bool mRxAvailable;

    void checkSlaves(uint8_t armed);

    void armSlaves(uint8_t armed);
    uint8_t mNodes[MAX_NODES];
    uint8_t mNodeCount;

    HAL_StatusTypeDef getStatus(uint8_t slave, uint8_t *value);
    void sendToSlaves(uint8_t *data);

public:
    RoleMaster(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount);

    void debug(uint8_t argc, char **argv);
    void buttonCallback(uint8_t state);
    void run();
};

#endif /* SRC_ROLE_MASTER_H_ */
