/*
 * nrf24L01.h
 *
 *  Created on: 18 Apr 2018
 *      Author: christo
 */

#ifndef SRC_NRF24L01_H_
#define SRC_NRF24L01_H_

#include "nRF24L01.h"
#include "spi.h"
#include "output.h"

#define nrf24_ADDR_LEN 5
#define nrf24_CONFIG ((1<<EN_CRC)|(0<<CRCO))

#define NRF24_TRANSMISSON_OK 0
#define NRF24_MESSAGE_LOST   1

class NRF24L01 {
	cSPI *mSPI;
	cOutput *mCSN;
	cOutput *mCE;

	bool mInitialized;
	uint8_t mPayLoadLen;

	void readRegister(uint8_t reg, uint8_t *value, uint8_t len);
	void writeRegister(uint8_t reg, uint8_t *value, uint8_t len);
	void setRegister(uint8_t reg, uint8_t value);
	void getRegister(uint8_t reg, uint8_t *value);

public:
	NRF24L01();
	virtual ~NRF24L01();

	HAL_StatusTypeDef init(cSPI *spi, cOutput *csn, cOutput *ce);
	HAL_StatusTypeDef config(uint8_t channel, uint8_t pay_lenght);

	uint8_t retransmissionCount();
	uint8_t readStatus();
	uint8_t getStatus();
	uint8_t lastMessageStatus();
    uint8_t rxFifoEmpty();

    void rxAddress(uint8_t *addr);
    void txAddress(uint8_t *addr);

	uint8_t dataReady();
	void getData(uint8_t *data);

	void send(uint8_t *value);
	uint8_t isSending();

	void powerUpRx();
	void powerUpTx();
	void powerDown();
};

#endif /* SRC_NRF24L01_H_ */
