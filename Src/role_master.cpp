/*
 * role_master.cpp
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#include <string.h>

#include "role_master.h"
#include "p_msg.h"
#include "pyro_registers.h"

RoleMaster::RoleMaster(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount) : Role(nodeInterface, led, ledCount)
{
    nextPoll = 0;
    shortPress = 0;
    longPress = 0;
    mArmed = 0;
    rocketCount = 0;
    mRxAvailable = false;

    uint8_t status = 0;
    mNodeInterface->pingNodes(1, MAX_NODES, &status);

    uint8_t nodes[MAX_NODES];
    uint8_t nodeCount = mNodeInterface->getNodes(nodes);
    PrintInfo("# of Nodes");
    printf("%02d\n", nodeCount);
    for(uint8_t idx = 0; idx < nodeCount; idx++)
    {
        PrintInfo("Node");
        printf("%d\n", nodes[idx]);
    }

    nodeInterface->listen();
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

void RoleMaster::getRegister(uint8_t slave, uint8_t reg, uint8_t *value)
{
    sPmsg_t pmsg;
    pmsg.type = PMSG_TYPE_GET;
    pmsg.tag = PMSG_TAG_READ_REG;
    pmsg.data[0] = reg;
    pmsg.data[1] = 0;

    mNodeInterface->sendToNode(slave, (uint8_t *)&pmsg);

    mNodeInterface->dataAvailable(false);

    int timeout = 1000;
    while(!mNodeInterface->runRx((uint8_t *)&pmsg) && timeout--);

    if(!timeout)
        return;

    *value = pmsg.data[1];
}

void RoleMaster::setRegister(uint8_t slave, uint8_t reg, uint8_t value)
{
    sPmsg_t pmsg;
    pmsg.type = PMSG_TYPE_SET;
    pmsg.tag = PMSG_TAG_WRITE_REG;
    pmsg.data[0] = reg;
    pmsg.data[1] = value;

    mNodeInterface->sendToNode(slave, (uint8_t *)&pmsg);
}

void RoleMaster::buttonCallback(uint8_t state)
{
    switch (state) {
        case 1:
            printf(": ");
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


void RoleMaster::checkSlaves()
{
    if(HAL_GetTick() > nextPoll)
    {
        nextPoll = HAL_GetTick() + 1000;
        uint8_t status = 0;
        mNodeInterface->pingNodes(1, MAX_NODES, &status);
        printf("node state: %02X\n", status);
        if(status)
        {
            mLeds[1]->flash(BILED2_FLASH_GREEN, 200);
            if(status > 1)
                mLeds[2]->flash(BILED2_FLASH_GREEN, 200);
        }
    }

//    if(pingNode(devAddr) == HAL_OK)
//    {
//        mLedR.reset();
//        mLedG.set();
//    }
//    else
//    {
//        mLedG.reset();
//        mLedR.set();
//    }
}

void RoleMaster::run()
{
    checkSlaves();
//    if(mNodeInterface->runRx(rxData))
//    {
//        PrintInfo("Master data in: ");
//          for (uint8_t idx = 0; idx < 4; idx++)
//              printf("%02X ", rxData[idx]);
//          printf("\n");
//    }
    if(longPress)
    {
        longPress = 0;

        if(!mArmed)
        {
            // arm now
            mLeds[0]->flash(BILED2_FLASH_RED, 1000);
            rocketCount= 0;
            mArmed = 1;
            printf(RED("ARMED\n"));
            armSlaves(1);
        }
        else
        {
            mLeds[0]->flash(BILED2_FLASH_GREEN, 1000);
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
            printf("pew: %d\n", rocketCount);
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

