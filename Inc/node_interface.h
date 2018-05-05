#ifndef SRC_NRF_COMMS_H_
#define SRC_NRF_COMMS_H_

#include "nrf24L01.h"
#include "nvm.h"
#include "hw.h"

#define MAX_NODES   8

typedef enum{
    NODE_ROLE_UNKNOWN,
    NODE_ROLE_MASTER,
    NODE_ROLE_SLAVE
} eNodeRoles_t;

class NodeInterface
{
    NRF24L01 *mNRF;
    eNodeRoles_t mRole;

    bool mInitialized;
    volatile bool mDataAvailable;
    uint8_t mPayLoadLen;

    uint8_t mId;
    uint8_t mRxAddress[5];
    uint8_t mTxAddress[5];

    uint8_t mNodes[MAX_NODES];
    uint8_t mNodeCount;

    uint8_t deviceAddr[4] =
    { 0x3E, 0x7C, 0x8D, 0x2E };

public:
    NodeInterface(NRF24L01 *nrfRadio);
    virtual ~NodeInterface();

    HAL_StatusTypeDef configure(uint8_t channel, uint8_t payloadLen, uint8_t *txAddr, uint8_t *rxAddr, uint8_t id);
    eNodeRoles_t getRole();

    void listen();
    void dataAvailable();

    HAL_StatusTypeDef pingNode(uint8_t *addr);
    HAL_StatusTypeDef pingNodes(uint8_t startAddr, uint8_t endAddr);
    uint8_t getNodes(uint8_t *nodes);

    HAL_StatusTypeDef sendToNode(uint8_t *nodeAddr, uint8_t *data);
    HAL_StatusTypeDef sendToNodes(uint8_t *data);

    void run();
};

#endif /* SRC_NRF_COMMS_H_ */
