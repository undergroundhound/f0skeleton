/*
 * role_slave.cpp
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#include "role_slave.h"

RoleSlave::RoleSlave(BiLED *led, NodeInterface *nodeInterface) : Role(led, nodeInterface)
{

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

}
