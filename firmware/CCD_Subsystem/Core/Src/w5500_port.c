#include "w5500_port.h"

uint8_t W5500_ReadByte(void) {
    uint8_t d=0xFF, r=0xFF;
    HAL_SPI_TransmitReceive(&hspi2, &d, &r, 1, HAL_MAX_DELAY);
    return r;
}

void W5500_WriteByte(uint8_t wb) {
    HAL_SPI_Transmit(&hspi2, &wb, 1, HAL_MAX_DELAY);
}

void W5500_ReadBurst(uint8_t* pBuf, uint16_t len) {
    for (uint16_t i=0; i<len; i++) pBuf[i] = W5500_ReadByte();
}

void W5500_WriteBurst(uint8_t* pBuf, uint16_t len) {
    HAL_SPI_Transmit(&hspi2, pBuf, len, HAL_MAX_DELAY);
}

void W5500_HardReset(void) {
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(W5500_RST_GPIO_Port, W5500_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
}
