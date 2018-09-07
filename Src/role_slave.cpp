/*
 * role_slave.cpp
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */
#include <string.h>

#include "role_slave.h"
#include "p_msg.h"

#define FIRE_TIME_ON    1000 //1000ms
#define CON_CHECK_INT   200
#define CON_TIMEOUT     2500

uint8_t currConnection;
uint8_t channels[4] = { ADC_CHANNEL_7, ADC_CHANNEL_5, ADC_CHANNEL_3, ADC_CHANNEL_1};
extern uint16_t openAdcValue;

RoleSlave::RoleSlave(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount) : Role(nodeInterface, led, ledCount), leds(led)
{
    mArmed = false;
    mStatus = 0x0F;
    mLastTick = 0;
    mConnectionTimeout = 0;
    mConnected = false;

    out1.reset();
    outputs[0] = &out1;
    timeOuts[0] = 0;

    out2.reset();
    outputs[1] = &out2;
    timeOuts[1] = 0;

    out3.reset();
    outputs[2] = &out3;
    timeOuts[2] = 0;

    out4.reset();
    outputs[3] = &out4;
    timeOuts[3] = 0;

    mAdc.init();

    mLeds[0]->setFlash(LED_ON, LED_RED);
}


void RoleSlave::checkConnections()
{
    if(currConnection > 3)
        currConnection = 0;

    uint16_t sample = mAdc.sampleChannel(channels[currConnection]);
//    printf("chan: %d adc = %d\n", currConnection, sample);
    if( sample < openAdcValue )
    {
        mLeds[currConnection+1]->setFlash(LED_FAST_FLASH, LED_RED);
        mStatus |= (1 << currConnection);
    }
    else
    {
        mLeds[currConnection+1]->setFlash(LED_ON, LED_GREEN);
        mStatus &= ~(1 << currConnection);
    }
    currConnection++;
}

void RoleSlave::arm(uint8_t state)
{
    PrintInfo("Slave");

    mArmed = state;
    if(mArmed)
    {
        printf(RED("ARMED\n"));
        mLeds[0]->setFlash(LED_FAST_FLASH, LED_RED);
    }
    else
    {
        printf(GREEN("!ARMED\n"));
        mLeds[0]->setFlash(LED_HEARTBEAT, LED_GREEN);
        for(uint8_t idx = 0; idx < 4; idx++)
        {
            outputs[idx]->reset();
            timeOuts[idx] = 0;
        }
    }
}


void RoleSlave::buttonCallback(uint8_t state)
{
    switch (state) {
            case 1:
                printf(": ");
                break;
            case 2:
//                shortPress = 1;
                break;
            case 3:
//                longPress = 1;
                break;
            default:
                break;
        }
}

void RoleSlave::debug(uint8_t argc, char **argv)
{
    if(argc == 1)
        {
            printf(GREEN_B("Slave Device\n"));
            printf("a\t- adc\n");
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
                        uint8_t buf[4];
                        memset(buf, 0x00, 4);
                        HAL_StatusTypeDef status = mNodeInterface->sendToNode(0, buf);
                        PrintInfo("master");
                        if(status == HAL_OK)
                            printf(GREEN("OK\n"));
                        else
                            printf(RED("FAIL\n"));
                    }
                    break;

                    case 'a':
                    {
                        uint8_t channels[4] = { 0x01, 0x04, 0x05, 0x07};
                        printf(CYAN_B("slave adc\n"));
                        for(uint8_t idx = 0; idx < 4; idx++)
                        {
                            printf("c [%d]: %d\n", channels[idx], (int)mAdc.sampleChannel(channels[idx]));
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

void RoleSlave::run()
{
    if(!mArmed)
    {
        if(HAL_GetTick() > mLastTick)
        {
            checkConnections();
            mLastTick = HAL_GetTick() + CON_CHECK_INT;
        }
    }

    if(HAL_GetTick() > mConnectionTimeout)
    {
        //nie meer connected nie.
        if(mArmed)
            mArmed = false;

        mConnected = false;
        leds[0]->setFlash(LED_ON, LED_RED);
    }

    //reset output after a second
    for(uint8_t idx=0; idx < 4; idx++)
    {
        if(timeOuts[idx] && timeOuts[idx] < HAL_GetTick())
        {
            printf("output[%d] reset\n", idx);
            outputs[idx]->reset();
            timeOuts[idx] = 0;
        }
    }

    if(mNodeInterface->runRx(rxData))
    {
        sPmsg_t pmsg;
        memcpy(&pmsg, rxData, 4);

//        PrintInfo("Slave data in: ");
//        for (uint8_t idx = 0; idx < 4; idx++)
//            printf("%02X ", rxData[idx]);
//        printf("\n");

        switch(pmsg.type)
        {
            case PMSG_TYPE_UNKNOWN:
            {
                mConnectionTimeout = HAL_GetTick() + CON_TIMEOUT;
                if(!mArmed)
                    arm(true);
            }
                break;
            case PMSG_TYPE_SET:
            {
                mConnectionTimeout = HAL_GetTick() + CON_TIMEOUT;
                switch(pmsg.tag)
                {
                    case PMSG_TAG_ARM:
                    {
                        uint8_t armed = pmsg.data[1];
                        arm(armed);
                    }
                    break;
                    case PMSG_TAG_FIRE:
                    {
                        uint8_t fireCount = pmsg.data[1];
                        PrintInfo("FIRE");
                        printf("%d\n", pmsg.data[1]);
                        if(fireCount < 4)
                        {
                            printf("output[%d] set\n", fireCount);
                            outputs[fireCount]->set();
                            timeOuts[fireCount] = HAL_GetTick() + FIRE_TIME_ON;
                            leds[fireCount+1]->setFlash(LED_ON, LED_RED);
                        }
                    }
                    break;
                    default:
                        break;
                }
            }break;
            case PMSG_TYPE_GET:
            {
                mConnectionTimeout = HAL_GetTick() + CON_TIMEOUT;
                switch (pmsg.tag) {
                    case PMSG_TAG_STATUS:
                    {
                        uint8_t value = mStatus;
                        pmsg.type = PMSG_TYPE_SET;
                        pmsg.data[1] = value;
                        mNodeInterface->sendToNode(0, (uint8_t *) &pmsg);

                        if(!mConnected)
                        {
                            mConnected = true;
                            mLeds[0]->setFlash(LED_HEARTBEAT, LED_GREEN);
                        }
                    }
                    break;
                    default:
                        break;
                }
            }
                break;
            default:
                break;
        }

    }
}
