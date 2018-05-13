/*
 * role_slave.cpp
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */
#include <string.h>

#include "role_slave.h"
#include "p_msg.h"


RoleSlave::RoleSlave(BiLED *led, NodeInterface *nodeInterface) : Role(led, nodeInterface)
{
    mArmed = false;

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
                            mLED->setFlash(LED_RED, LED_FAST_FLASH);
                        }
                        else
                        {
                            mLED->setFlash(LED_GREEN, LED_HEARTBEAT);
                        }
                    }
                    break;
                    case PMSG_TAG_FIRE:
                    {
                        PrintInfo("FIRE");
                        printf("%d\n", pmsg.data[1]);
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
