/*
 * role_master.h
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#ifndef SRC_ROLE_MASTER_H_
#define SRC_ROLE_MASTER_H_

#include "role.h"

#define POLL_INTERVAL   500
#define POLL_TIMEOUT    200

typedef enum{
    MASTER_IDLE,
    MASTER_POLL,
    MASTER_WAIT,
    MASTER_RECEIVED_FB
}eMasterState_t;

typedef struct{
    uint8_t id;
    uint8_t status;
}sNode_t;

class RoleMaster : public Role
{
    BiLED2 **leds;

    eMasterState_t mState;

    uint32_t nextPoll;
    uint8_t mArmed;
    uint8_t rocketCount;
    volatile uint8_t shortPress = 0;
    volatile uint8_t longPress = 0;

    uint8_t rxData[4];
    bool mRxAvailable;

    void armSlaves(uint8_t armed);

    sNode_t tempNodes[2];
    uint8_t nodesFound;
    uint8_t pollcount;

    sNode_t mNodes[MAX_NODES];
    uint8_t mNodeCount;

    HAL_StatusTypeDef requestStatus(uint8_t slave);
    void sendToSlaves(uint8_t *data);

public:
    RoleMaster(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount);

    void debug(uint8_t argc, char **argv);
    void buttonCallback(uint8_t state);
    void runStates();
    void run();
};

#endif /* SRC_ROLE_MASTER_H_ */
