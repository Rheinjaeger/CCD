#pragma once
#include "stm32l0xx_hal.h"
#include "wizchip_conf.h"
#include "main.h"

extern SPI_HandleTypeDef hspi2;   // from CubeMX

// Chip-select and control pins you mapped in .ioc
#define W5500_CS_GPIO_Port   GPIOA
#define W5500_CS_Pin         Ethernet_CE_Pin
#define W5500_RST_GPIO_Port  GPIOB
#define W5500_RST_Pin        Ethernet_RST_Pin

// CS helpers
static inline void W5500_Select(void)   { HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_RESET); }
static inline void W5500_Deselect(void) { HAL_GPIO_WritePin(W5500_CS_GPIO_Port, W5500_CS_Pin, GPIO_PIN_SET);   }

// SPI single-byte
uint8_t  W5500_ReadByte(void);
void     W5500_WriteByte(uint8_t wb);

// SPI burst
void     W5500_ReadBurst(uint8_t* pBuf, uint16_t len);
void     W5500_WriteBurst(uint8_t* pBuf, uint16_t len);

// Reset pulse
void     W5500_HardReset(void);
