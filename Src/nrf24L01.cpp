/*
 * nrf24L01.cpp
 *
 *  Created on: 18 Apr 2018
 *      Author: christo
 */

#include "nrf24L01.h"

#include <string.h>

/*-------- Private Functions ----------------------------------------------*/
void NRF24L01::readRegister(uint8_t reg, uint8_t *value, uint8_t len)
{
    mCSN->reset();

    mSPI->writeOpCode(R_REGISTER | (REGISTER_MASK & reg));
    mSPI->read(value, len);

    mCSN->set();
}

void NRF24L01::writeRegister(uint8_t reg, uint8_t *value, uint8_t len)
{
    mCSN->reset();

    mSPI->writeOpCode(W_REGISTER | (REGISTER_MASK & reg));
    mSPI->write(value, len);

    mCSN->set();
}

void NRF24L01::setRegister(uint8_t reg, uint8_t value)
{
    mCSN->reset();

    mSPI->writeOpCode(W_REGISTER | (REGISTER_MASK & reg));
    mSPI->writeOpCode(value);

    mCSN->set();
}

void NRF24L01::getRegister(uint8_t reg, uint8_t *value)
{
    mCSN->reset();

    mSPI->writeOpCode(R_REGISTER | (REGISTER_MASK & reg));
    mSPI->read(value, 1);

    mCSN->set();
}

/*-------- Public Functions ----------------------------------------------*/
NRF24L01::NRF24L01()
{
    mSPI = 0;
    mCSN = 0;
    mCE = 0;
    mPayLoadLen = 0;
    mInitialized = false;
}

NRF24L01::~NRF24L01()
{

}

HAL_StatusTypeDef NRF24L01::init(cSPI *spi, cOutput *csn, cOutput *ce) //: mSPI(spi), mOutput(output)
{
    mSPI = spi;
    mCSN = csn;
    mCE = ce;
    mPayLoadLen = 0;
    mCSN->set();
    mCE->reset();

    //read status to see that device is there
    uint8_t status = 0;
    mCSN->reset();
    mSPI->read(&status, 1);
    mCSN->set();
    if (!status)
        return HAL_ERROR;

    mInitialized = true;
    return HAL_OK;
}



HAL_StatusTypeDef NRF24L01::config(uint8_t channel, uint8_t pay_length)
{
    if (!mInitialized)
        return HAL_ERROR;

    //check if channel is valid
    if(channel > 125)
        return HAL_ERROR;

    // Set RF channel
    setRegister(RF_CH, channel);

    /* Use static payload length ... */
    mPayLoadLen = pay_length;


    // Set length of incoming payload
    setRegister(RX_PW_P0, 0x00); // Auto-ACK pipe ...
    setRegister(RX_PW_P1, mPayLoadLen); // Data payload pipe
    setRegister(RX_PW_P2, 0x00); // Pipe not used
    setRegister(RX_PW_P3, 0x00); // Pipe not used
    setRegister(RX_PW_P4, 0x00); // Pipe not used
    setRegister(RX_PW_P5, 0x00); // Pipe not used

    // 1 Mbps, TX gain: 0dbm
    setRegister(RF_SETUP, (DATARATE_250kbps << RF_DR) | (POWER_m6dbm << RF_PWR));

    // CRC enable, 1 byte CRC length
    setRegister(CONFIG, (1<<EN_CRC)|(0<<CRCO));

    // Auto Acknowledgment
    setRegister(EN_AA, (1<<ENAA_P0)|(1<<ENAA_P1)|(0<<ENAA_P2)|(0<<ENAA_P3)|(0<<ENAA_P4)|(0<<ENAA_P5));

    // Enable RX addresses
    setRegister(EN_RXADDR, (1<<ERX_P0)|(1<<ERX_P1)|(0<<ERX_P2)|(0<<ERX_P3)|(0<<ERX_P4)|(0<<ERX_P5));

    // Auto retransmit delay: 1250 us and Up to 15 retransmit trials
    setRegister(SETUP_RETR, (0x04 << ARD) | (0x0F << ARC));

    // Dynamic length configurations: No dynamic length
    setRegister(DYNPD, (0<<DPL_P0)|(0<<DPL_P1)|(0<<DPL_P2)|(0<<DPL_P3)|(0<<DPL_P4)|(0<<DPL_P5));

    // Start listening
    powerUpRx();

    return HAL_OK;
}

void NRF24L01::rxAddress(uint8_t *addr)
{
    if (!mInitialized)
        return;

    mCE->reset();
    writeRegister(RX_ADDR_P1, addr, nrf24_ADDR_LEN);
//    writeRegister(RX_ADDR_P0, addr, nrf24_ADDR_LEN);
    mCE->set();
}

void NRF24L01::txAddress(uint8_t *addr)
{
    if (!mInitialized)
        return;
    /* RX_ADDR_P0 must be set to the sending addr for auto ack to work. */
    writeRegister(RX_ADDR_P0, addr, nrf24_ADDR_LEN);
    writeRegister(TX_ADDR, addr, nrf24_ADDR_LEN);
}

uint8_t NRF24L01::dataReady()
{
    if (!mInitialized)
        return 0;
    // See note in getData() function - just checking RX_DR isn't good enough
    uint8_t status = getStatus();

    // We can short circuit on RX_DR, but if it's not set, we still need
    // to check the FIFO for any pending packets
    if (status & (1 << RX_DR))
    {
        return 1;
    }

    return !rxFifoEmpty();;
}

uint8_t NRF24L01::rxFifoEmpty()
{
    uint8_t fifoStatus;

    readRegister(FIFO_STATUS, &fifoStatus, 1);

    return (fifoStatus & (1 << RX_EMPTY));
}


void NRF24L01::getData(uint8_t *data)
{
    /* Pull down chip select */
    mCSN->reset();

    /* Send cmd to read rx payload */
    mSPI->writeOpCode(R_RX_PAYLOAD);

    /* Read payload */
    uint8_t buf[4];
    mSPI->read(buf, NRF24_DATA_LEN);
    memcpy(data, buf, NRF24_DATA_LEN);

    /* Pull up chip select */
    mCSN->set();

    /* Reset status register */
    setRegister(STATUS, (1 << RX_DR));

    flushRx();
}

uint8_t NRF24L01::retransmissionCount()
{
    uint8_t rv;
    readRegister(OBSERVE_TX, &rv, 1);
    rv = rv & 0x0F;
    return rv;
}

void NRF24L01::send(uint8_t *value)
{
    /* Go to Standby-I first */
    mCE->reset();

    /* Set to transmitter mode , Power up if needed */
    powerUpTx();

    /* Do we really need to flush TX fifo each time ? */
#if 1
    /* Pull down chip select */
    mCSN->reset();

    /* Write cmd to flush transmit FIFO */
    mSPI->writeOpCode(FLUSH_TX);

    /* Pull up chip select */
    mCSN->set();
#endif

    /* Pull down chip select */
    mCSN->reset();

    /* Write cmd to write payload */
    mSPI->writeOpCode(W_TX_PAYLOAD);

    /* Write payload */
    mSPI->write(value, mPayLoadLen);

    /* Pull up chip select */
    mCSN->set();

    /* Start the transmission */
    mCE->set();
}

uint8_t NRF24L01::isSending()
{
    uint8_t status;

    /* read the current status */
    status = getStatus();

    /* if sending successful (TX_DS) or max retries exceded (MAX_RT). */
    if ((status & ((1 << TX_DS) | (1 << MAX_RT))))
    {
        setRegister(STATUS, ((1 << TX_DS) | (1 << MAX_RT)));
        return 0; /* false */
    }

    return 1; /* true */
}

uint8_t NRF24L01::getStatus()
{
    uint8_t rv;

    mCSN->reset();
    mSPI->read(&rv, 1);
    mCSN->set();

    return rv;
}

uint8_t NRF24L01::getConfig()
{
    uint8_t rv;

    getRegister(CONFIG, &rv);
    return rv;
}

uint8_t NRF24L01::lastMessageStatus()
{
    uint8_t rv;

    rv = getStatus();

    /* Transmission went OK */
    if ((rv & ((1 << TX_DS))))
    {
        setRegister(STATUS, (1 << TX_DS));
        return NRF24_TRANSMISSON_OK;
    }
    /* Maximum retransmission count is reached */
    /* Last message probably went missing ... */
    else if ((rv & ((1 << MAX_RT))))
    {
        setRegister(STATUS, (1 << MAX_RT));
        return NRF24_MESSAGE_LOST;
    }
    /* Probably still sending ... */
    else
    {
        return 0xFF;
    }
}

void NRF24L01::flushRx()
{
    mCSN->reset();
    mSPI->writeOpCode(FLUSH_RX);
    mCSN->set();
}

void NRF24L01::powerUpRx()
{
    flushRx();

    setRegister(STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));

    mCE->reset();
    setRegister(CONFIG, nrf24_CONFIG | ((1 << PWR_UP) | (1 << PRIM_RX)));
    mCE->set();
}

void NRF24L01::powerUpTx()
{
    setRegister(STATUS, (1 << RX_DR) | (1 << TX_DS) | (1 << MAX_RT));

    setRegister(CONFIG, nrf24_CONFIG | ((1 << PWR_UP) | (0 << PRIM_RX)));
}

void NRF24L01::powerDown()
{
    mCE->reset();
    setRegister(CONFIG, nrf24_CONFIG);
}
