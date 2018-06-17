/*
 * role_master.cpp
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#include <string.h>

#include "role_master.h"
#include "p_msg.h"

RoleMaster::RoleMaster(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount) : Role(nodeInterface, led, ledCount), leds(led)
{
    nextPoll = 0;
    shortPress = 0;
    longPress = 0;
    mArmed = 0;
    rocketCount = 0;
    mRxAvailable = false;

    nodeIdx = 0;

    checkSlaves(mArmed);
    nodeInterface->listen();
    led[0]->setFlash(LED_HEARTBEAT, LED_GREEN);

    for(uint8_t idx = 0; idx < 2; idx++)
    {
        nodeStatus[idx] = 0xFF;
        nodeTimeout[idx] = 0;
    }
}


void RoleMaster::sendToSlaves(uint8_t *data)
{
    for(uint8_t idx = 0; idx < MAX_NODES; idx++)
    {
        if(nodeStatus[idx] == 0xFF)
            return;
//        printf("send to: %d\n", idx+1);
        mNodeInterface->sendToNode(idx+1, data);
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
    sendToSlaves(mData);
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
        printf("s\t- nodes status\n");
        return;
    }

    if(argc > 1 )
    {
        //single char
        if(strlen(argv[1]) == 1)
        {
            char c = argv[1][0];
            switch(c)
            {
                case 's':
                {
//                    PrintInfo("# of Nodes");
//                    printf("%02d\n", mNodeCount);
//                    for(uint8_t idx = 0; idx < mNodeCount; idx++)
//                    {
////                        PrintInfo("Node Status");
////                        uint8_t status = 0;
////                        if(getStatus(mNodes[idx], &status) == HAL_OK)
////                            printf("%d = 0x%02X\n", mNodes[idx], status);
////                        else
////                            printf(RED("timeout\n"));
//                    }
                }
                break;

                default:
                    break;
            }
        }
        return;
    }
}

void RoleMaster::checkSlaves(uint8_t armed)
{
    if(HAL_GetTick() > nextPoll)
    {
        nextPoll = HAL_GetTick() + 500;
        //just ping devices

        if(armed && (nodeIdx > MAX_NODES))
        {
            uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};
            sendToSlaves(data);

            return;
        }
        //search for node one by one

        if(nodeIdx > MAX_NODES)
            nodeIdx = 1;

        sPmsg_t pmsg;
        pmsg.type = PMSG_TYPE_GET;
        pmsg.tag = PMSG_TAG_STATUS;
        pmsg.data[0] = nodeIdx;
        pmsg.data[1] = 0;

        mNodeInterface->sendToNode(nodeIdx, (uint8_t *)&pmsg);
        mNodeInterface->listen();
        nodeIdx++;
    }
}

void RoleMaster::run()
{
    if(!mArmed)
    {
        for(uint8_t idx = 0; idx < 2;idx++)
        {
            if(HAL_GetTick() > nodeTimeout[idx])
            {
                mLeds[idx+1]->setFlash(LED_ON, LED_RED);
                nodeStatus[idx] = 0xFF;
            }
        }
    }

    checkSlaves(mArmed);

    if(mNodeInterface->runRx(rxData))
    {
//        PrintInfo("Master data in: ");
//        for (uint8_t idx = 0; idx < 4; idx++)
//            printf("%02X ", rxData[idx]);
//        printf("\n");

        sPmsg_t pmsg;
        memcpy(&pmsg, rxData, 4);

        switch(pmsg.type)
        {
            break;
            case PMSG_TYPE_SET:
            {
                switch(pmsg.tag)
                {
                    case PMSG_TAG_STATUS:
                    {
                        uint8_t node = pmsg.data[0];
                        uint8_t status = pmsg.data[1];
                        nodeTimeout[node-1] = HAL_GetTick() + NODE_TIMEOUT;
                        nodeStatus[node-1] = status;

                        if(status == 0)
                            mLeds[node]->setFlash(LED_ON, LED_GREEN);
                        else
                            mLeds[node]->setFlash(LED_FAST_FLASH, LED_GREEN);
                    }
                    break;
                    default:
                        break;
                }
            }break;
            default:
                break;
        }

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
            sendToSlaves(mData);
        }
    }
}

