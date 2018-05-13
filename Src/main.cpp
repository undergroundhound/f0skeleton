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

#include "../Inc/node_interface.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_conf.h"

#include "hw.h"
#include "terminal.h"
#include "output.h"
#include "spi.h"
#include "nrf24L01.h"
#include "spi_device.h"
#include "nvm.h"
#include "hw_gpio.h"
#include "led.h"
#include "button.h"
#include "p_msg.h"
#include "bi_led.h"
#include "device_controller.h"
#include "adc.h"

/* Private variables ---------------------------------------------------------*/

#define PrintInfo(_info) printf("%15s : ", _info)


/* Private function prototypes -----------------------------------------------*/

cSPI spi = cSPI();
cOutput *csn;
cOutput *ce;
cOutput *mFlashCs;
cSpiDevice *mSpiFlash;
NVM *mNVM;

NRF24L01 nrf = NRF24L01();
NodeInterface *nodeInterface;

LED *RstatusLed;
LED *GstatusLed;
BiLED *statusLED;

Button *buttonLaunch;

DeviceController deviceController = DeviceController();

cADC *adc;

static uint8_t deviceAddr[4] =
{ 0x3E, 0x7C, 0x8D, 0x2E };

/*!
 * Debug entries
 */

void printAddr()
{
    sNvm_t nvm;
    mNVM->get(&nvm);

    PrintInfo("TX Address");
    for (uint8_t idx = 0; idx < 5; idx++)
        printf("%02X ", nvm.txAddr[idx]);
    printf("\n");
    PrintInfo("RX Address");
    for (uint8_t idx = 0; idx < 5; idx++)
        printf("%02X ", nvm.rxAddr[idx]);
    printf("\n");
}

void charToHex(char *characters, uint8_t *hexData, uint8_t len)
{
    char byte[2];
    uint8_t charIdx = 0;
    for (uint8_t idx = 0; idx < len; idx++)
    {
        memcpy(byte, &characters[charIdx], 2);
        charIdx += 2;
        hexData[idx] = (uint8_t) strtol(byte, NULL, 16);
    }
}

uint8_t getId()
{
    sNvm_t nvm;
    mNVM->get(&nvm);
    uint8_t id = nvm.id;
    return id;
}

void printId()
{
    uint8_t id = getId();
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


void nrfIrq(void)
{
    if(nrf.dataReady())
        nodeInterface->dataAvailable(true);
}

void buttonIrq()
{
    buttonLaunch->irq();
}

void buttonPress(uint8_t state)
{
    deviceController.buttonCB(state);
}

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    HW_GPIO_ClockEnable();

    terminal_init();
    printf("welcome!\n");

    PrintInfo("SPI1");
    if (spi.init(SPI1, 50000) == HAL_OK)
        printf(GREEN("OK\n"));
    else
        printf(RED("Fail\n"));

    /* Initialize the LEDs */
    RstatusLed = new LED(GPIOA, GPIO_PIN_0);
    GstatusLed = new LED(GPIOA, GPIO_PIN_1);
    statusLED = new BiLED(RstatusLed, GstatusLed);

    csn = new cOutput(GPIOB, GPIO_PIN_10);
    ce = new cOutput(GPIOB, GPIO_PIN_4);

    mFlashCs = new cOutput(GPIOB, GPIO_PIN_6);
    mSpiFlash = new cSpiDevice(&spi, mFlashCs);
    mNVM = new NVM(mSpiFlash);

    buttonLaunch = new Button(GPIOB, GPIO_PIN_5);
    buttonLaunch->setCb(buttonPress);
    HW_SetupIrq(GPIOB, GPIO_PIN_5, GPIO_MODE_IT_RISING_FALLING, GPIO_PULLUP, buttonIrq);

    /* Probe the flash */
    uint8_t id[3];
    mSpiFlash->readId(id, 3);
    PrintInfo("Flash ID");
    for (int i = 0; i < 3; i++)
        printf("%02X", id[i]);
    printf("\n");

    PrintInfo("nRF");
    if(nrf.init(&spi, csn, ce) == HAL_OK)
    {
        printf(GREEN("OK\n"));
        HW_SetupIrq(GPIOA, GPIO_PIN_8, GPIO_MODE_IT_FALLING, GPIO_NOPULL, nrfIrq);
    }
    else
        printf(RED("Fail\n"));

    nodeInterface = new NodeInterface(&nrf);

    {
        sNvm_t nvm;
        mNVM->get(&nvm);
        PrintInfo("nRF Config");
        if(nodeInterface->configure(nvm.channel, 4, nvm.txAddr, nvm.rxAddr, nvm.id) == HAL_OK)
            printf(GREEN("OK\n"));
        else
            printf(RED("Fail\n"));
    }

    printId();
    printChannel();
    printAddr();
    nodeInterface->listen();

    deviceController.init(nodeInterface, statusLED);

    statusLED->setFlasher(BILED_SLOW_FUCKAROUND);

    adc = new cADC();
    adc->init();
    nrf.powerUpRx();
    printf("adc [0]: %d\n", adc->sampleChannel(4));


    while (1)
    {
        statusLED->run();
        buttonLaunch->run();
        deviceController.run();
        terminal_run();
    }
}

void Id(uint8_t argc, char **argv)
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

    uint8_t addr[5];
    memcpy(addr, deviceAddr, 4);
    addr[4] = (uint8_t) id;
    memcpy(nvm.rxAddr, addr, 5);

    addr[4] = 0;
    memcpy(nvm.txAddr, addr, 5);

    nvm.id = (uint8_t) id;
    mNVM->set(&nvm);
    printId();
    printAddr();
}
sTermEntry_t idEntry =
{ "id", "Set device id", Id };

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

void Address(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        printAddr();
        return;
    }

    if (argc != 3)
    {
        printf("addr <(t)x/(r)x> <addr[5]>");
        return;
    }

    sNvm_t nvm;
    mNVM->get(&nvm);

    if (strlen(argv[2]) != 10)
    {
        printf("5 bytes for addr\n");
        return;
    }

    uint8_t addr[5];
    charToHex(argv[2], addr, 5);

    char txRx = argv[1][0];
    if (txRx == 't')
    {
        memcpy(nvm.txAddr, addr, 5);
    }
    else if (txRx == 'r')
    {
        memcpy(nvm.rxAddr, addr, 5);
    }
    else
        return;

    mNVM->set(&nvm);
}
sTermEntry_t addrEntry =
{ "addr", "Set radio addr", Address };

void SendPacket(uint8_t argc, char **argv)
{

    uint8_t data[4];
    memset(data, 0x00, 4);

    printf("ping node ");

    nrf.send(data);
    while(nrf.isSending());

    if(nrf.lastMessageStatus() == NRF24_TRANSMISSON_OK)
        printf(GREEN("OK\n"));
    else
        printf(RED("Fail\n"));

}
sTermEntry_t sendEntry =
{ "gs", "Get and Set", SendPacket };

/**
 * @brief  This function is executed in case of error occurrence.
 * @param  None
 * @retval None
 */
void _Error_Handler(const char * file, int line)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
    {
    }
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
