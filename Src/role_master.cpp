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
    mState = MASTER_IDLE;

    mNodeCount = 0;
    pollcount = 0;
    nodesFound = 0;

//    checkSlaves(mArmed);
    nodeInterface->listen();
    led[0]->setFlash(LED_HEARTBEAT, LED_GREEN);
}


void RoleMaster::sendToSlaves(uint8_t *data)
{
    for(uint8_t idx = 0; idx < mNodeCount; idx++)
    {
        printf("send to: %d\n", mNodes[idx].id);
        mNodeInterface->sendToNode(mNodes[idx].id, data);
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

HAL_StatusTypeDef RoleMaster::requestStatus(uint8_t slave)
{
    sPmsg_t pmsg;
    pmsg.type = PMSG_TYPE_GET;
    pmsg.tag = PMSG_TAG_STATUS;
    pmsg.data[0] = slave;
    pmsg.data[1] = mArmed;

//    printf("out s: %d pmsg: ", slave);
//    uint8_t data[4];
//    memcpy(data, &pmsg, 4);
//    for(uint8_t idx=0; idx < 4; idx++)
//    {
//        printf("%02X", data[idx]);
//    }
//    printf("\n");

    HAL_StatusTypeDef status = mNodeInterface->sendToNode(slave, (uint8_t *)&pmsg);
//    uint8_t tryCount = 1;
//    while(status != HAL_OK && tryCount)
//    {
//        HAL_Delay(10);
//        status = mNodeInterface->sendToNode(slave, (uint8_t *)&pmsg);
//        tryCount--;
//    }

    return status;
//    if(status != HAL_OK && !tryCount)
//        return HAL_ERROR;
//
//    return HAL_OK;
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
//void RoleMaster::checkSlaves(uint8_t armed)
//{
//    if(HAL_GetTick() > nextPoll)
//    {
//        nextPoll = HAL_GetTick() + 500;
//
//        //just ping devices
//        if(armed)
//        {
//            uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};
//            sendToSlaves(data);
//
//            return;
//        }
//
//        mNodeCount = 0;
//        for(uint8_t idx = 1; idx <= MAX_NODES; idx++)
//        {
//            HAL_Delay(1);
//            HAL_StatusTypeDef s = requestStatus(idx);
//            printf("s: %d\n", s);
//            if(s != HAL_OK)
//            {
//
//                mLeds[idx]->setFlash(LED_FAST_FLASH, LED_RED);
//            }
//
//        }
//    }
//}



void RoleMaster::runStates()
{
    switch (mState) {
        case MASTER_IDLE:
        {
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


            if(HAL_GetTick() > nextPoll)
            {
                nodesFound = 0;
                if(mArmed)
                {
                    uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};
                    sendToSlaves(data);
                    nextPoll = HAL_GetTick() + POLL_INTERVAL;
                    return;
                }
                mState = MASTER_POLL;
            }
        }
        break;
        case MASTER_POLL:
        {
            if(HAL_GetTick() < nextPoll)
                return;

            if(pollcount >= MAX_NODES)
            {
                pollcount = 0;
                mState = MASTER_WAIT;
                nextPoll = HAL_GetTick() + 500;
                return;
            }

            uint8_t idx = pollcount + 1;
            //failed to send
            printf("pollNode %d\n", idx);
            if(requestStatus(idx) != HAL_OK)
            {
                mLeds[idx]->setFlash(LED_FAST_FLASH, LED_RED);
            }
            nextPoll = HAL_GetTick() + 750;
            pollcount++;
        }
        break;
        case MASTER_WAIT:
        {
            //timeout
            if(HAL_GetTick() > nextPoll)
            {
                mState = MASTER_RECEIVED_FB;
            }
        }
        break;
        case MASTER_RECEIVED_FB:
        {
            mNodeCount = nodesFound;
            for(uint8_t idx = 0; idx < nodesFound; idx++)
            {
//                printf("nodeFound: %d\n", tempNodes[idx].id);
                memcpy((void*)&mNodes[idx], (void*)&tempNodes[idx], sizeof(sNode_t));
            }
            mNodeCount = nodesFound;
            mState = MASTER_IDLE;
            nextPoll = HAL_GetTick() + POLL_INTERVAL;
        }
        break;
        default:
            break;
    }
}

void RoleMaster::run()
{
    runStates();
//    checkSlaves(mArmed);

    if(mNodeInterface->runRx(rxData))
    {
        sPmsg_t pmsg;
        memcpy(&pmsg, rxData, 4);

        PrintInfo("Master data in: ");
        for (uint8_t idx = 0; idx < 4; idx++)
            printf("%02X ", rxData[idx]);
        printf("\n");

        switch(pmsg.type)
        {
            case PMSG_TYPE_SET:
            {
                uint8_t idx = pmsg.data[0];
                if(pmsg.data[1] == 0)
                {
                        printf("status == 0\n");
                        mLeds[idx]->setFlash(LED_ON, LED_GREEN);
                }
                else
                    mLeds[idx]->setFlash(LED_FAST_FLASH, LED_GREEN);

                printf("nodeFound: %d\n", pmsg.data[0]);
                tempNodes[nodesFound].id = pmsg.data[0];
                tempNodes[nodesFound].status = pmsg.data[1];
                nodesFound++;

            }break;
            default:
                break;
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
                    PrintInfo("# of Nodes");
                    printf("%02d\n", mNodeCount);
                    for(uint8_t idx = 0; idx < mNodeCount; idx++)
                    {
                        PrintInfo("Node Status");
                        uint8_t status = 0;
                        if(requestStatus(mNodes[idx].id) == HAL_OK)
                            printf("%d = 0x%02X\n", mNodes[idx], status);
                        else
                            printf(RED("timeout\n"));
                    }
                }
                break;

                default:
                    break;
            }
        }
        return;
    }
}
