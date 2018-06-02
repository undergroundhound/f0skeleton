/*
 * role.cpp
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#include "role.h"

Role::Role(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount) : mNodeInterface(nodeInterface), mLeds(led), mLedCount(ledCount)
{
}
