/*
 * p_msg.h
 *
 *  Created on: 02 May 2018
 *      Author: christo
 */

#ifndef INC_P_MSG_H_
#define INC_P_MSG_H_

#include "hw.h"

typedef enum{
    PMSG_TYPE_UNKNOWN,
    PMSG_TYPE_GET,
    PMSG_TYPE_SET
}ePmsgType_t;

typedef enum{
    PMSG_TAG_UNKNOWN,
    PMSG_TAG_STATUS,
    PMSG_TAG_ARM,
    PMSG_TAG_FIRE,
    PMSG_TAG_READ_REG,
    PMSG_TAG_WRITE_REG
}ePmsgTag_t;

typedef struct{
    ePmsgType_t type;
    ePmsgTag_t tag;
    uint8_t data[2];
}sPmsg_t;

#endif /* INC_P_MSG_H_ */
