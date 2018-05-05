/*
 * button.h
 *
 *  Created on: 30 Apr 2018
 *      Author: christo
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "hw.h"

class Button
{
    GPIO_TypeDef *mGpio;
    uint32_t mPin;

    uint8_t shortPress = 0;
    volatile bool mState;

    uint32_t mDebounce;
    volatile uint32_t timeDown;

    void (*mCallback)(uint8_t) = 0;
public:
    Button(GPIO_TypeDef *gpio, uint32_t pin);
    virtual ~Button();

    void setCb(void (*callback)(uint8_t state));
    void irq();
    void run();
};

#endif /* BUTTON_H_ */
