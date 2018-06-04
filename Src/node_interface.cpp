#include "../Inc/node_interface.h"

#include "terminal.h"

#include <string.h>
#include <stdlib.h>



NodeInterface::NodeInterface(NRF24L01 *nrfRadio) : mNRF(nrfRadio)
{
    mInitialized = true;
    mPayLoadLen = 4;
    mNodeCount = 0;
    memset(mNodes, 0xFF, MAX_NODES);

    mNetId = 0;
    mId = 0;
    mRole = NODE_ROLE_UNKNOWN;
    mState = NODE_STATE_UNKNOWN;
    mCurrObj = 0;
}

NodeInterface::~NodeInterface()
{

}

HAL_StatusTypeDef NodeInterface::configure(uint8_t channel, uint8_t payloadLen, uint8_t netId, uint8_t id)
{
    HAL_StatusTypeDef status = mNRF->config(channel, payloadLen);
    if(status != HAL_OK)
        return status;

    mNetId = netId;
    mId = id;

    if(!mId)
        mRole = NODE_ROLE_MASTER;
    else
        mRole = NODE_ROLE_SLAVE;

    mPayLoadLen = payloadLen;

    setTxIds(netId, 0);
    setRxIds(netId, id);
    return status;
}

void NodeInterface::setTxIds(uint8_t netId, uint8_t id)
{
    uint8_t txAddr[5];
    memcpy(txAddr, deviceAddr, 3);
    txAddr[3] = netId;
    txAddr[4] = id;
    mNRF->txAddress(txAddr);
}

void NodeInterface::setRxIds(uint8_t netId, uint8_t id)
{
    uint8_t rxAddr[5];
    memcpy(rxAddr, deviceAddr, 3);
    rxAddr[3] = netId;
    rxAddr[4] = id;
    mNRF->rxAddress(rxAddr);
}

eNodeRoles_t NodeInterface::getRole()
{
    return mRole;
}

void NodeInterface::listen()
{
    mNRF->powerUpRx();
}

HAL_StatusTypeDef NodeInterface::pingNodes(uint8_t startAddr, uint8_t endAddr, uint8_t *state)
{
    //if not master
    if(mId)
        return HAL_ERROR;

    uint8_t s = 0;

    mNodeCount = 0;
    memset(mNodes, 0x00, MAX_NODES);

    while(startAddr < endAddr)
    {
        uint8_t data[4];
        memset(data, 0x00, 4);
        HAL_StatusTypeDef status = sendToNode(startAddr, data);
        if(status == HAL_TIMEOUT)
            return HAL_TIMEOUT;

        if(status == HAL_OK)
        {
            s |= (1 << mNodeCount);
            mNodes[mNodeCount++] = startAddr;
        }

        startAddr++;
    }

    *state = s;
    return HAL_OK;
}

HAL_StatusTypeDef NodeInterface::sendToNode(uint8_t nodeId, uint8_t *data)
{
    uint8_t devAddr[5];
    memcpy(devAddr, deviceAddr, 3);
    devAddr[3] = mNetId;
    devAddr[4] = nodeId;

    mNRF->powerUpTx();
    HAL_Delay(5);
    //set payload size to zero
    mNRF->txAddress(devAddr);

    HAL_Delay(5);

    HAL_StatusTypeDef status = send(data, 4, 10);

    mNRF->powerUpRx();

    return status;
}

HAL_StatusTypeDef NodeInterface::sendToNodes(uint8_t *data)
{
    //if not master
    if(mId)
        return HAL_ERROR;

    for(uint8_t idx = 0; idx < mNodeCount; idx++)
    {
        HAL_StatusTypeDef status = sendToNode(mNodes[idx], data);
        if(status == HAL_TIMEOUT)
            return HAL_TIMEOUT;
    }

    return HAL_OK;
}

HAL_StatusTypeDef NodeInterface::sendToMaster(uint8_t *data)
{
    //if master return
    if(!mId)
        return HAL_ERROR;

    return sendToNode(0, data);
}

uint8_t NodeInterface::getNodes(uint8_t *nodes)
{
    if(mNodeCount)
        memcpy(nodes, mNodes, mNodeCount);
    return mNodeCount;
}

void NodeInterface::irq(uint8_t status)
{
    //transmit
    if(status & (1 << RX_DR))
    {
//        printf("di\n");
        mNRF->setRegister(STATUS, (1 << RX_DR));
        mNRF->getData(mRxData);
        mDataAvailable = true;
    }

    //fail
    if(status & (1 << MAX_RT))
    {
        mNRF->setRegister(STATUS, (1 << MAX_RT));
        if(mCurrObj)
        {
            mCurrObj->status = HAL_ERROR;
            mState = NODE_STATE_RESULT;
        }
    }

    //reg
    if(status & (1 << TX_DS))
    {
        mNRF->setRegister(STATUS, (1 << TX_DS));
        if(mCurrObj)
        {
            mCurrObj->status = HAL_OK;
            mState = NODE_STATE_RESULT;
        }
//        mState = NODE_STATE_RECEIVE;
    }
}

uint8_t NodeInterface::runRx(uint8_t *data)
{
    if(!mDataAvailable)
        return 0;

    mDataAvailable = false;
    memcpy(data, mRxData, NRF24_DATA_LEN);
    return 1;
}

HAL_StatusTypeDef NodeInterface::send(uint8_t *data, uint8_t len, uint32_t timeout)
{
    if(len != 4)
        return HAL_ERROR;

    sNodeObj_t obj;
    mCurrObj = &obj;

    memcpy(mCurrObj->dataOut, data, len);
    mState = NODE_STATE_SEND;

    mCurrObj->busy = true;
    while(mCurrObj->busy && timeout--)
    {
        runTx();
        HAL_Delay(5);
    }

    if(!timeout)
        return HAL_TIMEOUT;

    mCurrObj = 0;

    if(obj.status == HAL_OK)
        return HAL_OK;

    return HAL_ERROR;
}
void NodeInterface::runTx()
{
    switch (mState)
    {
        case NODE_STATE_IDLE:
            break;
        case NODE_STATE_SEND:
        {
            if(!mCurrObj)
            {
                mState = NODE_STATE_IDLE;
                return;
            }
            if(mNRF->getStatus() & (1 << MAX_RT))
                mNRF->setRegister(STATUS, (1 << MAX_RT));

            mNRF->send(mCurrObj->dataOut);
            mState = NODE_STATE_WAIT;
        }
        break;
        case NODE_STATE_WAIT:
        {
            if(!mCurrObj)
            {
                mState = NODE_STATE_IDLE;
                return;
            }
        }
        break;

        case NODE_STATE_RESULT:
        {
            if(!mCurrObj)
            {
                mState = NODE_STATE_IDLE;
                return;
            }
            mCurrObj->busy = false;
        }
        break;


        default:
            mState = NODE_STATE_IDLE;
            break;
    }
}
