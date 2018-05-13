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

    ADC_ChannelConfTypeDef sConfig;

    __HAL_RCC_ADC1_CLK_ENABLE();
      /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
      */
    hadc.Instance = ADC1;
    hadc.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc.Init.Resolution = ADC_RESOLUTION_8B;
    hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc.Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
    hadc.Init.EOCSelection = DISABLE;
    hadc.Init.LowPowerAutoWait = DISABLE;
    hadc.Init.LowPowerAutoPowerOff = DISABLE;
    hadc.Init.ContinuousConvMode = DISABLE;
    hadc.Init.DiscontinuousConvMode = DISABLE;
    hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc.Init.DMAContinuousRequests = DISABLE;
    hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    HAL_StatusTypeDef status = HAL_ADC_Init(&hadc);
    if (status != HAL_OK)
    {
        //TODO
//        printf("status: %d\n", status);
//      _Error_Handler(__FILE__, __LINE__);
    }

      /**Configure for the selected ADC regular channel to be converted.
      */
    sConfig.Channel = ADC_CHANNEL_4;
    sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
    sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
    if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)
    {
        printf("2\n");
//      _Error_Handler(__FILE__, __LINE__);
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
        //enable the adc clock
//        __HAL_RCC_ADC1_CLK_ENABLE();

        //calibrate the adc
        HAL_ADCEx_Calibration_Start(&hadc);

        //deselect all channels
        sConfig.Channel = 0x0;
        sConfig.Rank = ADC_RANK_NONE;
        HAL_ADC_ConfigChannel(&hadc, &sConfig);

        //configure selected channel
        sConfig.Channel = channel;
        sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
        HAL_ADC_ConfigChannel(&hadc, &sConfig);

        HAL_ADC_Start(&hadc);

        //wait for the end of conversion
        HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);

        //get the converted value of channel
        result = HAL_ADC_GetValue(&hadc);

//        __HAL_ADC_DISABLE(&hadc);

        //disable the adc clock
//        __HAL_RCC_ADC1_CLK_DISABLE();
    }

    return result;
}
