/*
 * role_master.cpp
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#include <string.h>

#include "role_master.h"
#include "p_msg.h"

RoleMaster::RoleMaster(BiLED *led, NodeInterface *nodeInterface) : Role(led, nodeInterface)
{
    shortPress = 0;
    longPress = 0;
    mArmed = 0;
    rocketCount = 0;

    mNodeInterface->pingNodes(1, MAX_NODES);

    uint8_t nodes[MAX_NODES];
    uint8_t nodeCount = mNodeInterface->getNodes(nodes);
    PrintInfo("# of Nodes");
    printf("%02d\n", nodeCount);
    for(uint8_t idx = 0; idx < nodeCount; idx++)
    {
        PrintInfo("Node");
        printf("%d\n", nodes[idx]);
    }

}

void RoleMaster::armSlaves(uint8_t armed)
{
    sPmsg_t msg;
    msg.type = PMSG_TYPE_SET;
    msg.tag = PMSG_TAG_ARM;

    if(armed)
    {
        msg.data[0] = 1;
        msg.data[1] = 1;
    }
    else
    {
        msg.data[0] = 0;
        msg.data[1] = 0;
    }
    memcpy(mData, &msg, 4);
    mNodeInterface->sendToNodes(mData);
}


void RoleMaster::buttonCallback(uint8_t state)
{
    switch (state) {
        case 1:
//            printf(": ");
            break;
        case 2:
            shortPress = 1;
            break;
        case 3:
            longPress = 1;
            break;
        default:
            break;
    }
}



void RoleMaster::run()
{
    if(longPress)
    {
        longPress = 0;

        if(!mArmed)
        {
            // arm now
            mLED->setFlash(LED_RED, LED_FAST_FLASH);
            rocketCount= 0;
            mArmed = 1;
            printf(RED("ARMED\n"));
            armSlaves(1);
        }
        else
        {
            mLED->setFlash(LED_GREEN, LED_HEARTBEAT);
            mArmed = 0;
            printf(GREEN("!ARMED\n"));
            armSlaves(0);
        }
    }

    if(shortPress)
    {
        shortPress = 0;
        if(mArmed)
        {
            printf("pew\n");
            sPmsg_t msg;
            msg.type = PMSG_TYPE_SET;
            msg.tag = PMSG_TAG_FIRE;
            msg.data[0] = 0;
            msg.data[1] = rocketCount++;
            memcpy(mData, &msg, 4);
            mNodeInterface->sendToNodes(mData);
        }
    }
}

