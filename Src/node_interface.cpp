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

    mNRF->txAddress(txAddr);
    memcpy(mTxAddress, txAddr, 5);

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

void NodeInterface::dataAvailable()
{
    mDataAvailable = true;
}

HAL_StatusTypeDef NodeInterface::pingNode(uint8_t *addr)
{
    //set payload size to zero
    uint8_t data[mPayLoadLen];
    memset(data, 0x00, mPayLoadLen);

    mNRF->txAddress(addr);

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

HAL_StatusTypeDef NodeInterface::pingNodes(uint8_t startAddr, uint8_t endAddr)
{
    //if not master
    if(mId)
        return HAL_ERROR;

    uint8_t devAddr[5];
    memcpy(devAddr, deviceAddr, 4);

    while(startAddr < endAddr)
    {
        devAddr[4] = startAddr;

        HAL_StatusTypeDef status = pingNode(devAddr);
        if(status == HAL_TIMEOUT)
            return HAL_TIMEOUT;

        if(status == HAL_OK)
            mNodes[mNodeCount++] = startAddr;

        startAddr++;
    }

    return HAL_OK;
}

HAL_StatusTypeDef NodeInterface::sendToNode(uint8_t *nodeAddr, uint8_t *data)
{
    //set payload size to zero
    mNRF->txAddress(nodeAddr);

    mNRF->send(data);
    uint32_t timeOut = HAL_GetTick() + 100;
    while(mNRF->isSending() && (timeOut > HAL_GetTick()));

    if(timeOut < HAL_GetTick())
        return HAL_TIMEOUT;

    if(mNRF->lastMessageStatus() == NRF24_TRANSMISSON_OK)
        return HAL_OK;

    return HAL_ERROR;
}

HAL_StatusTypeDef NodeInterface::sendToNodes(uint8_t *data)
{
    //if not master
    if(mId)
        return HAL_ERROR;

    uint8_t devAddr[5];
    memcpy(devAddr, deviceAddr, 4);

    for(uint8_t idx = 0; idx < mNodeCount; idx++)
    {
        devAddr[4] = mNodes[idx];

        HAL_StatusTypeDef status = sendToNode(devAddr, data);
        if(status == HAL_TIMEOUT)
            return HAL_TIMEOUT;
    }

    return HAL_OK;
}


uint8_t NodeInterface::getNodes(uint8_t *nodes)
{
    if(mNodeCount)
        memcpy(nodes, mNodes, mNodeCount);
    return mNodeCount;
}

void NodeInterface::run()
{
    if (mDataAvailable)
    {
        mDataAvailable = false;
        uint8_t data[mPayLoadLen];
        if (mNRF->dataReady())
        {
            mNRF->getData(data);
            printf("rx:");
            for (uint8_t idx = 0; idx < 4; idx++)
                printf(" %02X", data[idx]);
            printf("\n");
        }
    }
}
