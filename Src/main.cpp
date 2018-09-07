/**
 ******************************************************************************
 * File Name          : main.c
 * Description        : Main program body
 ******************************************************************************
 ** This notice applies to any and all portions of this file
 * that are not between comment pairs USER CODE BEGIN and
 * USER CODE END. Other portions of this file, whether
 * inserted by the user or by software development tools
 * are owned by their respective copyright owners.
 *
 * COPYRIGHT(c) 2018 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
//#include "main.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_conf.h"
#include "hw.h"
#include "hw_gpio.h"
#include "bi_led_2.h"

#include "i2c.h"
#include "nvm.h"

#include "spi.h"
#include "nrf24L01.h"
#include "node_interface.h"
#include "device_controller.h"

#include "output.h"
#include "button.h"
#include "terminal.h"

/* Private variables ---------------------------------------------------------*/

#define VERSION_MAJOR   0
#define VERSION_MINOR   1
#define VERSION_FIX     0

#define PrintInfo(_info) printf("%15s : ", _info)

uint16_t openAdcValue = 100;

/* Private function prototypes -----------------------------------------------*/

IWDG_HandleTypeDef iwdg;

static cSPI spi = cSPI();
cOutput *csn;
cOutput *ce;

NVM *mNVM;

NRF24L01 nrf = NRF24L01();
NodeInterface *nodeInterface;

Button *buttonLaunch;

DeviceController deviceController = DeviceController();

/*!
 * Debug entries
 */

void getIds(uint8_t &netId, uint8_t &id)
{
    sNvm_t nvm;
    mNVM->get(&nvm);
    netId = nvm.netId;
    id = nvm.id;
}

void printId()
{
    uint8_t id = 0;
    uint8_t netId = 0;
    getIds(netId, id);
    PrintInfo("Network ID");
    printf("0x%02X\n", netId);
    PrintInfo("Device ID");
    printf("0x%02X\n", id);
}

void printChannel()
{
    sNvm_t nvm;
    mNVM->get(&nvm);
    PrintInfo("WiFi Channel");
    printf("0x%02X\n", nvm.channel);
}

void printOpenAdcValue()
{
    sNvm_t nvm;
    mNVM->get(&nvm);
    PrintInfo("ADC open value");
    printf("%d\n", nvm.adc);
}

void nrfIrq(void)
{
    nodeInterface->irq(nrf.getStatus());
}

void buttonIrq()
{
    buttonLaunch->irq();
}

void buttonPress(uint8_t state)
{
    deviceController.buttonCB(state);
}

void resetSource()
{
    PrintInfo("Reset");
    if(READ_BIT(RCC->CSR, RCC_CSR_LPWRRSTF))
        printf("Low-power\n");
    if(READ_BIT(RCC->CSR, RCC_CSR_WWDGRSTF))
        printf("Window watchdog\n");
    if(READ_BIT(RCC->CSR, RCC_CSR_IWDGRSTF))
        printf("Independent Window watchdog\n");
    if(READ_BIT(RCC->CSR, RCC_CSR_SFTRSTF))
        printf("Software reset\n");
    if(READ_BIT(RCC->CSR, RCC_CSR_PORRSTF))
        printf("POR/PDR on\n");
//    if(READ_BIT(RCC->CSR, RCC_CSR_PINRSTF))
//        printf("Pin\n");
    //   if(READ_BIT(RCC->CSR, RCC_CSR_FWRSTF))
    //      printf("Reset        : Firewall\n");
    if(READ_BIT(RCC->CSR, RCC_CSR_OBLRSTF))
        printf("Options bytes load\n");

    SET_BIT(RCC->CSR, RCC_CSR_RMVF);
}


void init_slaveOutputs()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    HW_GPIO_ClockEnable();
    init_slaveOutputs();

    terminal_init();
    printf("Wi-Py welcomes you!\n");
    PrintInfo("Version");
    printf("%02d.%02d.%02d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_FIX);

    resetSource();

    I2C iic = I2C();
    iic.init();
    mNVM = new NVM(&iic);

    PrintInfo("SPI1");
    if (spi.init(SPI1, 4000000) == HAL_OK)
        printf(GREEN("OK\n"));
    else
        printf(RED("Fail\n"));

    csn = new cOutput(GPIOB, GPIO_PIN_12);
    ce = new cOutput(GPIOB, GPIO_PIN_2);

    PrintInfo("nRF");
    if(nrf.init(&spi, csn, ce) == HAL_OK)
    {
        printf(GREEN("OK\n"));
        HW_SetupIrq(GPIOB, GPIO_PIN_1, GPIO_MODE_IT_FALLING, GPIO_PULLUP, nrfIrq);
    }
    else
        printf(RED("Fail\n"));

    nodeInterface = new NodeInterface(&nrf);

    buttonLaunch = new Button(GPIOB, GPIO_PIN_7);
    buttonLaunch->setCb(buttonPress);
    HW_SetupIrq(GPIOB, GPIO_PIN_7, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP, buttonIrq);

    {
        sNvm_t nvm;
        mNVM->get(&nvm);

        PrintInfo("nRF Config");
        if(nodeInterface->configure(nvm.channel, 4, nvm.netId, nvm.id) == HAL_OK)
            printf(GREEN("OK\n"));
        else
            printf(RED("Fail\n"));

        printId();
        printChannel();
        openAdcValue = nvm.adc;
        printOpenAdcValue();

    }

    nodeInterface->listen();

    /* Initialize the LEDs */
    cOutput led1red(GPIOA, GPIO_PIN_12);
    cOutput led1green(GPIOA, GPIO_PIN_13);

    cOutput led2red(GPIOF, GPIO_PIN_6);
    cOutput led2green(GPIOF, GPIO_PIN_7);

    cOutput led3red(GPIOA, GPIO_PIN_14);
    cOutput led3green(GPIOA, GPIO_PIN_15);

    cOutput led4red(GPIOB, GPIO_PIN_3);
    cOutput led4green(GPIOB, GPIO_PIN_4);

    cOutput led5red(GPIOB, GPIO_PIN_5);
    cOutput led5green(GPIOB, GPIO_PIN_6);

    BiLED2 *leds[5];
    leds[0] = new BiLED2(&led1green, &led1red);
    leds[1] = new BiLED2(&led2green, &led2red);
    leds[2] = new BiLED2(&led3green, &led3red);
    leds[3] = new BiLED2(&led4green, &led4red);
    leds[4] = new BiLED2(&led5green, &led5red);


    leds[0]->setFlash(LED_OFF, LED_RED);
    leds[1]->setFlash(LED_OFF, LED_RED);
    leds[2]->setFlash(LED_OFF, LED_RED);
    leds[3]->setFlash(LED_OFF, LED_RED);
    leds[4]->setFlash(LED_OFF, LED_RED);


    deviceController.init(nodeInterface, leds, 5);

    nrf.powerUpRx();

    iwdg.Instance = IWDG;
    iwdg.Init.Prescaler = IWDG_PRESCALER_8;
    iwdg.Init.Reload = 0x0FFF;
    iwdg.Init.Window = 0x0FFF;
    HAL_IWDG_Init(&iwdg);

    while (1)
    {
        HAL_IWDG_Refresh(&iwdg);
        static uint8_t ledCount = 0;
        if(ledCount > 4)
            ledCount = 0;
        leds[ledCount++]->run();
        HAL_IWDG_Refresh(&iwdg);
        buttonLaunch->run();
        HAL_IWDG_Refresh(&iwdg);
        deviceController.run();
        HAL_IWDG_Refresh(&iwdg);
        terminal_run();
    }
}

void adc(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        printOpenAdcValue();
        return;
    }

    if (argc != 2)
        return;

    sNvm_t nvm;
    mNVM->get(&nvm);

    int a = atoi(argv[1]);

    if ((a < 0) || (a > 4095))
    {
        printf("0 < id < 4095\n");
        return;
    }
    openAdcValue = a;
    nvm.adc = (uint16_t)a;
    mNVM->set(&nvm);
    printf("set adc %d\n", a);
}
sTermEntry_t adcEntry =
{ "adc", "Set ADC output", adc };


void Id(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        printId();
        return;
    }

    if (argc != 2)
        return;

    sNvm_t nvm;
    mNVM->get(&nvm);

    int id = atoi(argv[1]);

    if ((id < 0) || (id > MAX_NODES))
    {
        printf("0 < id < %d\n", MAX_NODES);
        return;
    }
    nvm.id = (uint8_t)id;
    mNVM->set(&nvm);
    printf("set id %d\n", id);
}
sTermEntry_t idEntry =
{ "id", "Set device id", Id };

void netId(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        printId();
        return;
    }

    if (argc != 2)
    {
        printf("id <id[1]>");
        return;
    }

    sNvm_t nvm;
    mNVM->get(&nvm);

    int id = atoi(argv[1]);

    if ((id < 0) || (id > 255))
    {
        printf("0 < id < 255\n");
        return;
    }

    nvm.netId = (uint8_t)id;
    mNVM->set(&nvm);
    HAL_Delay(5);
    printf("set netid %d\n", id);
}
sTermEntry_t netIdEntry =
{ "nid", "Set Network id", netId };

void Channel(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        printChannel();
        return;
    }

    if (argc != 2)
    {
        printf("chan <channel[1]>");
        return;
    }

    sNvm_t nvm;
    mNVM->get(&nvm);

    int chan = atoi(argv[1]);

    if ((chan < 0) || (chan > 16))
    {
        printf("0 < id < 16\n");
        return;
    }

    nvm.channel = (uint8_t) chan;
    mNVM->set(&nvm);
    printChannel();
}
sTermEntry_t channelEntry =
{ "chan", "Set WiFi channel", Channel };

void DeviceDebug(uint8_t argc, char **argv)
{
    deviceController.debug(argc, argv);
}
sTermEntry_t deviceEntry =
{ "d", "Device debug", DeviceDebug };

void NRFinfo(uint8_t argc, char **argv)
{
    uint8_t status = nrf.getStatus();
    PrintInfo("NRF Status");
    printf("0x%02X\n", status);

    uint8_t cfg = nrf.getConfig();
    PrintInfo("NRF Config");
    printf("0x%02X\n", cfg);
}
sTermEntry_t nrfEntry =
{ "nrf", "nrf info", NRFinfo };

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(const char * file, int line)
{
    printf("error %s @ %d", file, line);
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
//    while (1)
//    {
//    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT

/**
 * @brief Reports the name of the source file and the source line number
 * where the assert_param error has occurred.
 * @param file: pointer to the source file name
 * @param line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */

}

#endif

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
