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

void RoleMaster::debug(uint8_t argc, char **argv)
{
    if(argc == 1)
    {
        printf(GREEN_B("Master Device\n"));
        printf("p\t- ping nodes\n");
        return;
    }

    if(argc == 2)
    {
        //single char
        if(strlen(argv[1]) == 1)
        {
            char c = argv[1][0];
            switch(c)
            {
                case 'p':
                {
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
                }
                break;
                case 's':
                {
                    PrintInfo("Send to node");
                    printf("1\n");
                    uint8_t data[4] = {0x00, 0x01, 0x02, 0x03};

                    mNodeInterface->sendToNode(1, data);
                }
                break;

                default:
                    break;
            }
        }
        return;
    }
}

void RoleMaster::checkSlaves()
{
    if(HAL_GetTick() > nextPoll)
    {
        nextPoll = HAL_GetTick() + 5000;
        static uint8_t status = 0;
        uint8_t newStatus = 0;
        mNodeInterface->pingNodes(1, MAX_NODES, &newStatus);
        printf("node state: %02X : %02X\n", status, newStatus);
        if(status != newStatus)
        {
            mLeds[1]->setFlash(LED_FAST_FLASH, LED_RED);
            mLeds[2]->setFlash(LED_FAST_FLASH, LED_RED);
            status = newStatus;
            if(status & 0x01)
                mLeds[1]->setFlash(LED_HEARTBEAT, LED_GREEN);
            if(status & 0x02)
                mLeds[2]->setFlash(LED_HEARTBEAT, LED_GREEN);
        }
    }
}

void RoleMaster::run()
{
//    checkSlaves();
    if(mNodeInterface->runRx(rxData))
    {
        PrintInfo("Master data in: ");
          for (uint8_t idx = 0; idx < 4; idx++)
              printf("%02X ", rxData[idx]);
          printf("\n");
    }
    if(longPress)
    {
        longPress = 0;

        if(!mArmed)
        {
            // arm now
            mLeds[0]->setFlash(LED_FAST_FLASH, LED_RED);
            rocketCount= 0;
            mArmed = 1;
            printf(RED("ARMED\n"));
            armSlaves(1);
        }
        else
        {
            mLeds[0]->setFlash(LED_HEARTBEAT, LED_GREEN);
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

