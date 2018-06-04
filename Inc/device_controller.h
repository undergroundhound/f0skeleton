/*
 * device_controller.h
 *
 *  Created on: 02 May 2018
 *      Author: christo
 */

#ifndef DEVICE_CONTROLLER_H_
#define DEVICE_CONTROLLER_H_

#include "hw.h"
#include "node_interface.h"
#include "bi_led_2.h"

#include "role.h"

class DeviceController
{
    Role *mRole;

    void armSlaves(uint8_t armed);

public:
    DeviceController();
    virtual ~DeviceController();

    void init(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount);
    void buttonCB(uint8_t type);
    void debug(uint8_t argc, char **argv);
    void run();
};

#endif /* DEVICE_CONTROLLER_H_ */
