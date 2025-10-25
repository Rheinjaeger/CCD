/*
 * w5500_port.h
 *
 *  Created on: Oct 9, 2025
 *      Author: Javier
 */

#ifndef INC_W5500_PORT_H_
#define INC_W5500_PORT_H_

#include "stm32l0xx_hal.h"
#include "wizchip_conf.h"

void W5500_Select(void);
void W5500_Deselect(void);
uint8_t W5500_ReadByte(void);
void W5500_WriteByte(uint8_t tx);
void W5500_ReadBurst(uint8_t* pBuf, uint16_t len);
void W5500_WriteBurst(uint8_t* pBuf, uint16_t len);
uint8_t getVERSIONR(void);

#endif /* INC_W5500_PORT_H_ */
