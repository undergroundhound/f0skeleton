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

void DeviceController::init(NodeInterface *nodeInterface, BiLED *led)
{
    PrintInfo("Device Role");

    if(nodeInterface->getRole() == NODE_ROLE_MASTER)
    {
        mRole = new RoleMaster(led, nodeInterface);
        printf(CYAN("Master\n"));

    }
    else
    {
        mRole = new RoleSlave(led, nodeInterface);
        printf(CYAN("Slave\n"));
        nodeInterface->listen();
    }
}

void DeviceController::buttonCB(uint8_t type)
{
    if(!mRole)
        return;

    mRole->buttonCallback(type);
}

void DeviceController::run()
{
    if(!mRole)
        return;

    mRole->run();
}
