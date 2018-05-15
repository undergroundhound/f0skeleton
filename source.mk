######################################
# target
######################################
TARGET = F0_skeleton
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m0

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F030C6Tx_FLASH.ld

# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_adc.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_adc_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dma.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart_ex.c \
Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_spi.c \
Src/hw.c \
Src/system_stm32f0xx.c \
Src/stm32f0xx_hal_msp.c \
Src/stm32f0xx_it.c \
Src/commands.c \
Drivers/BSP/Utils/Src/terminal.c \
Drivers/BSP/Utils/Src/spi_device.c \
Drivers/BSP/Utils/Src/hw_gpio.c \
Drivers/BSP/Utils/Src/i2c.c \
#Drivers/BSP/nrf24L01/nrf24.c \
Drivers/BSP/nrf24L01/radioPinFunctions.c

# C++ sources
CXX_SOURCES =  \
$(wildcard Src/*.cpp) \
Drivers/BSP/Utils/Src/output.cpp \
Drivers/BSP/Utils/Src/spi.cpp  \
#$(wildcard Drivers/BSP/Utils/Src/*.cpp)

# ASM sources
ASM_SOURCES =  \
startup_stm32f030x6.s

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F030x6

# C includes
C_INCLUDES =  \
-IInc \
-IDrivers/STM32F0xx_HAL_Driver/Inc \
-IDrivers/STM32F0xx_HAL_Driver/Inc/Legacy \
-IDrivers/CMSIS/Device/ST/STM32F0xx/Include \
-IDrivers/CMSIS/Include \
-IDrivers/BSP/Utils/Inc \
-IDrivers/BSP/nrf24L01
