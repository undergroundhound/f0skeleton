/*
 * role.h
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#ifndef SRC_ROLE_H_
#define SRC_ROLE_H_

#include "hw.h"
#include "bi_led_2.h"
#include "node_interface.h"


class Role
{
protected:
    uint8_t mData[4];
    NodeInterface *mNodeInterface;
    BiLED2 **mLeds;
    uint8_t mLedCount;

public:
    Role(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount);
    virtual ~Role()
    {
    }

    virtual void buttonCallback(uint8_t state) = 0;
    virtual void run() = 0;
};

#endif /* SRC_ROLE_H_ */
