/*
 * button.cpp
 *
 *  Created on: 30 Apr 2018
 *      Author: christo
 */

#include "button.h"

Button::Button(GPIO_TypeDef *gpio, uint32_t pin)
{
    mState = false;
    mGpio = gpio;
    mPin = pin;
    mCallback = 0;
    mDebounce = 0;
}

Button::~Button()
{
    // TODO Auto-generated destructor stub
}


void Button::setCb(void (*callback)(uint8_t state))
{
    if(callback)
        mCallback = callback;
}


void Button::irq()
{
    mDebounce = 0;

    if(HAL_GetTick() < mDebounce)
        return;

    mDebounce = HAL_GetTick() + 300;

    if (HW_GPIO_Read(mGpio, mPin) == RESET)
    {
        mState = true;
        timeDown = HAL_GetTick();
    }
}

void Button::run()
{
    if(mState)
    {
        if(!shortPress && (HAL_GetTick() - timeDown) > 20)
        {
//            printf("shortpress\n");
            shortPress = 1;
            if(mCallback)
                mCallback(1);

            return;
        }

        if((HAL_GetTick() - timeDown)  > 750)
        {
//            printf("long press\n");
            shortPress = 0;
            mState = false;
            if(mCallback)
                mCallback(3);

            return;
        }

        if(HW_GPIO_Read(mGpio, mPin) != RESET)
        {
            mState = false;

            if(shortPress && (HAL_GetTick() - timeDown)  < 750)
            {
                if(mCallback)
                    mCallback(2);
            }

            shortPress = 0;
        }
    }
}
