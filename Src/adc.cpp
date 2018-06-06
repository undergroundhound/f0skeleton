/*
 * adc.cpp
 *
 *  Created on: 06 Dec 2017
 *      Author: cfurter
 */

#include "adc.h"

cADC::cADC()
{
    mInitialized = false;
}

cADC::~cADC()
{

}

void cADC::init()
{
    if(mInitialized)
        return;

    __HAL_RCC_ADC1_CLK_ENABLE();


    printf("kom hier\n");
    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
     */
    hadc.Instance = ADC1;
    hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc.Init.Resolution = ADC_RESOLUTION_12B;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc.Init.LowPowerAutoWait = DISABLE;
    hadc.Init.LowPowerAutoPowerOff = DISABLE;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.DMAContinuousRequests = DISABLE;
    hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    if (HAL_ADC_Init(&hadc) != HAL_OK)
    {
        printf("fokkop\n");
        //    _Error_Handler(__FILE__, __LINE__);
    }

    mInitialized = true;
}

void cADC::deInit()
{
    mInitialized = false;
}

uint16_t cADC::sampleChannel(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig;
    uint16_t result = 0;

    if(mInitialized)
    {
        //calibrate the adc
        HAL_ADCEx_Calibration_Start(&hadc);

        //deselect all channels
        sConfig.Channel = 0x0;
        sConfig.Rank = ADC_RANK_NONE;
        HAL_ADC_ConfigChannel(&hadc, &sConfig);

        //configure selected channel
        sConfig.Channel = channel;
        sConfig.SamplingTime = ADC_SAMPLETIME_71CYCLES_5;
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        HAL_ADC_ConfigChannel(&hadc, &sConfig);

        HAL_ADC_Start(&hadc);

        //wait for the end of conversion
        HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);

        //get the converted value of channel
        result = HAL_ADC_GetValue(&hadc);

        HAL_ADC_Stop(&hadc);
    }

    return result;
}
