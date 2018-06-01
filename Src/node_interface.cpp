#include "../Inc/node_interface.h"

#include "terminal.h"

#include <string.h>
#include <stdlib.h>



NodeInterface::NodeInterface(NRF24L01 *nrfRadio) : mNRF(nrfRadio)
{
    mInitialized = true;
    mDataAvailable = false;
    mPayLoadLen = 4;
    mNodeCount = 0;
    memset(mNodes, 0xFF, MAX_NODES);

    mId = 0;
    mRole = NODE_ROLE_UNKNOWN;

}

NodeInterface::~NodeInterface()
{

}

HAL_StatusTypeDef NodeInterface::configure(uint8_t channel, uint8_t payloadLen, uint8_t *txAddr, uint8_t *rxAddr, uint8_t id)
{
    HAL_StatusTypeDef status = mNRF->config(channel, payloadLen);
    if(status != HAL_OK)
        return status;
    mId = id;
    if(!mId)
        mRole = NODE_ROLE_MASTER;
    else
        mRole = NODE_ROLE_SLAVE;

    mPayLoadLen = payloadLen;

    memcpy(mTxAddress, txAddr, 5);
    mNRF->txAddress(mTxAddress);

    mNRF->rxAddress(rxAddr);
    memcpy(mRxAddress, rxAddr, 5);
    return status;
}

eNodeRoles_t NodeInterface::getRole()
{
    return mRole;
}

void NodeInterface::listen()
{
    mNRF->powerUpRx();
}

void NodeInterface::dataAvailable(bool state)
{
    mDataAvailable = state;
}

HAL_StatusTypeDef NodeInterface::pingNode(uint8_t *addr)
{
    //set payload size to zero
    uint8_t data[mPayLoadLen];
    memset(data, 0x00, mPayLoadLen);

    mNRF->txAddress(addr);
    HAL_Delay(5);

    mNRF->send(data);
    uint32_t timeOut = HAL_GetTick() + 100;
    while(mNRF->isSending() && (timeOut > HAL_GetTick()))
        HAL_Delay(2);

    if(timeOut < HAL_GetTick())
        return HAL_TIMEOUT;

    if(mNRF->lastMessageStatus() == NRF24_TRANSMISSON_OK)
        return HAL_OK;

    return HAL_ERROR;
}

HAL_StatusTypeDef NodeInterface::pingNodes(uint8_t startAddr, uint8_t endAddr, uint8_t *state)
{
    //if not master
    if(mId)
        return HAL_ERROR;

    uint8_t s = 0;
    uint8_t devAddr[5];
    memcpy(devAddr, deviceAddr, 4);

    mNodeCount = 0;
    memset(mNodes, 0x00, MAX_NODES);

    while(startAddr < endAddr)
    {
        devAddr[4] = startAddr;

        HAL_StatusTypeDef status = pingNode(devAddr);
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
    memcpy(devAddr, deviceAddr, 4);
    devAddr[4] = nodeId;

    mNRF->powerUpTx();
    //set payload size to zero
    mNRF->txAddress(devAddr);

    mNRF->send(data);
    uint32_t timeOut = HAL_GetTick() + 1;
    while(mNRF->isSending() && (timeOut > HAL_GetTick()));

    mNRF->powerUpRx();
    mDataAvailable = false;

    if(timeOut < HAL_GetTick())
        return HAL_TIMEOUT;


//    if(mNRF->lastMessageStatus() == NRF24_TRANSMISSON_OK)
//    {
//
////        return HAL_OK;
//    }



    return HAL_ERROR;
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

uint8_t NodeInterface::runRx(uint8_t *data)
{
    if (mDataAvailable)
    {
        mDataAvailable = false;
        mNRF->getData(data);
        return 1;
    }
    return 0;
}
