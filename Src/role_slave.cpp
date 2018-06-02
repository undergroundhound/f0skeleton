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

RoleSlave::RoleSlave(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount) : Role(nodeInterface, led, ledCount)
{
    mArmed = false;

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

//    led1.setFlash(LED_RED, LED_FAST_FLASH);
}

void RoleSlave::arm(uint8_t state)
{
    PrintInfo("Slave");

    mArmed = state;
    if(mArmed)
    {
        printf(RED("ARMED\n"));
    }
    else
    {
        printf(GREEN("!ARMED\n"));
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

void RoleSlave::run()
{
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
        switch(pmsg.type)
        {
            case PMSG_TYPE_SET:
            {
                switch(pmsg.tag)
                {
                    case PMSG_TAG_ARM:
                    {
                        uint8_t armed = pmsg.data[1];
                        arm(armed);
                        if(armed)
                        {
                            mLeds[0]->flash(BILED2_FLASH_RED, 1000);
                        }
                        else
                        {
                            mLeds[0]->flash(BILED2_FLASH_GREEN, 1000);
                        }
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
                        }
                    }
                    break;
                    case PMSG_TAG_WRITE_REG:
                        printf("write_reg: %d = %d\n", pmsg.data[0], pmsg.data[1]);
                        slaveRegisters.setRegister(pmsg.data[0], pmsg.data[1]);
                        break;
                    default:
                        break;
                }
            }break;
            case PMSG_TYPE_GET:
            {
                switch (pmsg.tag) {
                    case PMSG_TAG_READ_REG:
                    {
                        uint8_t reg = pmsg.data[0];
                        uint8_t value = 0;

                        slaveRegisters.getRegister(reg, &value);
                        printf("read_reg: %d = %d\n", pmsg.data[0], value);
//                        printf("read_reg: %d\n", reg);
                        pmsg.type = PMSG_TYPE_SET;
                        pmsg.data[1] = value;
                        mNodeInterface->sendToMaster((uint8_t *) &pmsg);
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
//        PrintInfo("Slave data in: ");
//        for (uint8_t idx = 0; idx < 4; idx++)
//            printf("%02X ", rxData[idx]);
//        printf("\n");
    }
}
