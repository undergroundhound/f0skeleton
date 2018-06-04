/*
 * nvm.h
 *
 *  Created on: 21 Apr 2018
 *      Author: christo
 */

#ifndef INC_NVM_H_
#define INC_NVM_H_

#include <stdint.h>

#include "i2c.h"

#define NVM_ADDR    0

typedef struct{
	uint8_t netId;      //1
	uint8_t id;         //2
	uint8_t channel;    //2
}sNvm_t;

class NVM {
    I2C *mDevice;
public:
	NVM(I2C *device);
	virtual ~NVM();

	void get(sNvm_t *nvm);
	void set(sNvm_t *nvm);
};

#endif /* INC_NVM_H_ */
