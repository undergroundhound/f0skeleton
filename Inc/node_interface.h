#ifndef SRC_NRF_COMMS_H_
#define SRC_NRF_COMMS_H_

#include "nrf24L01.h"
#include "nvm.h"
#include "hw.h"

#define MAX_NODES   8

typedef struct{
    bool busy = false;
    HAL_StatusTypeDef status;
    uint8_t dataOut[4];
} sNodeObj_t;

typedef enum{
    NODE_ROLE_UNKNOWN,
    NODE_ROLE_MASTER,
    NODE_ROLE_SLAVE
} eNodeRoles_t;

typedef enum{
    NODE_STATE_UNKNOWN,
    NODE_STATE_IDLE,
    NODE_STATE_SEND,
    NODE_STATE_WAIT,
    NODE_STATE_RESULT
} eNodeStates_t;

class NodeInterface
{
    NRF24L01 *mNRF;
    eNodeRoles_t mRole;
    eNodeStates_t mState;

    bool mInitialized;
    uint8_t mPayLoadLen;

    uint8_t mId;
    uint8_t mNetId;
    uint8_t mRxData[4];

    uint8_t mNodes[MAX_NODES];
    uint8_t mNodeCount;

    uint8_t deviceAddr[3] =
    { 0x3E, 0x7C, 0x8D };

    void setTxIds(uint8_t netId, uint8_t id);
    void setRxIds(uint8_t netId, uint8_t id);

    sNodeObj_t *mCurrObj;
    volatile bool mDataAvailable;

public:
    NodeInterface(NRF24L01 *nrfRadio);
    virtual ~NodeInterface();

    HAL_StatusTypeDef configure(uint8_t channel, uint8_t payloadLen, uint8_t netId, uint8_t id);
    eNodeRoles_t getRole();

    void listen();
    void irq(uint8_t status);

    HAL_StatusTypeDef pingNodes(uint8_t startAddr, uint8_t endAddr, uint8_t *state);
    uint8_t getNodes(uint8_t *nodes);

    HAL_StatusTypeDef sendToNode(uint8_t nodeId, uint8_t *data);
    HAL_StatusTypeDef sendToNodes(uint8_t *data);
    HAL_StatusTypeDef sendToMaster(uint8_t *data);

    uint8_t runRx(uint8_t *data);
    HAL_StatusTypeDef send(uint8_t *data, uint8_t len, uint32_t timeout);
    void runTx();
};

#endif /* SRC_NRF_COMMS_H_ */
