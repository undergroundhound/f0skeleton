/*
 * role_slave.h
 *
 *  Created on: 05 May 2018
 *      Author: christo
 */

#ifndef SRC_ROLE_SLAVE_H_
#define SRC_ROLE_SLAVE_H_

#include "role.h"

class RoleSlave : public Role
{
public:
    RoleSlave(BiLED *led, NodeInterface *nodeInterface);
    void buttonCallback(uint8_t state);
    void run();
};

#endif /* SRC_ROLE_SLAVE_H_ */
