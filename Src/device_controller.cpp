#include "device_controller.h"
#include <string.h>

#include "p_msg.h"

#include "role_master.h"
#include "role_slave.h"


DeviceController::DeviceController()
{
    mRole = 0;
}

DeviceController::~DeviceController()
{
    // TODO Auto-generated destructor stub
}

void DeviceController::init(NodeInterface *nodeInterface, BiLED2 **led, uint8_t ledCount)
{
    PrintInfo("Device Role");

    if(nodeInterface->getRole() == NODE_ROLE_MASTER)
    {
        printf(CYAN("Master\n"));
        mRole = new RoleMaster(nodeInterface, led, ledCount);

    }
    else
    {
        printf(CYAN("Slave\n"));
        mRole = new RoleSlave(nodeInterface, led, ledCount);
        nodeInterface->listen();
    }
}

void DeviceController::buttonCB(uint8_t state)
{
    if(!mRole)
        return;

    mRole->buttonCallback(state);
}

void DeviceController::debug(uint8_t argc, char **argv)
{
    mRole->debug(argc, argv);
}

void DeviceController::run()
{
    if(!mRole)
        return;


    mRole->run();
}
