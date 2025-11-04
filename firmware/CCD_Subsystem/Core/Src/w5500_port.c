#include "stm32l0xx_hal.h"
//#include "wizchip_conf.h"
#include "w5500_port.h"

extern SPI_HandleTypeDef hspi2;  // SPI2 handle from CubeMX

// --- Chip Select (CS) control ---
void W5500_Select(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET); // CS low
}
void W5500_Deselect(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // CS high
}

// --- SPI single-byte read/write ---
uint8_t W5500_ReadByte(void) {
    uint8_t rx = 0xFF;
    HAL_SPI_Receive(&hspi2, &rx, 1, HAL_MAX_DELAY);
    return rx;
}
void W5500_WriteByte(uint8_t tx) {
    HAL_SPI_Transmit(&hspi2, &tx, 1, HAL_MAX_DELAY);
}

// --- SPI burst read/write ---
void W5500_ReadBurst(uint8_t* pBuf, uint16_t len) {
    HAL_SPI_Receive(&hspi2, pBuf, len, HAL_MAX_DELAY);
}
void W5500_WriteBurst(uint8_t* pBuf, uint16_t len) {
    HAL_SPI_Transmit(&hspi2, pBuf, len, HAL_MAX_DELAY);
}

uint8_t getVERSIONR(void)
{
    // 0x0039 is the address of VERSIONR for the W5500
    return WIZCHIP_READ(0x0039);
}
