/*
 * nvm.h
 *
 *  Created on: 21 Apr 2018
 *      Author: christo
 */

#ifndef INC_NVM_H_
#define INC_NVM_H_

#include <stdint.h>
#include "spi_device.h"

#define NVM_ADDR    0

typedef struct{
	uint8_t id;         //1
	uint8_t channel;    //2
	uint8_t slaveIds[6];//8
	uint8_t rxAddr[5];  //13
	uint8_t placeK2[3]; //16
	uint8_t txAddr[5];  //21
	uint8_t placeK3[11];//32
}sNvm_t;

class NVM {
    cSpiDevice *mDevice;
public:
	NVM(cSpiDevice *device);
	virtual ~NVM();

	void erase();
	void get(sNvm_t *nvm);
	void set(sNvm_t *nvm);
};

#endif /* INC_NVM_H_ */
