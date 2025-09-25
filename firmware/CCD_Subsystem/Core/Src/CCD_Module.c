/*
 * CCD_Module.c
 *
 *  Created on: Sep 13, 2025
 *      Author: Admin
 */
#include "stm32l0xx_hal.h"

#define CCD_I1_Pin GPIO_PIN_0
#define CCD_I1_GPIO_Port GPIOB
#define CCD_I2_Pin GPIO_PIN_1
#define CCD_I2_GPIO_Port GPIOB
#define CCD_I3_Pin GPIO_PIN_2
#define CCD_I3_GPIO_Port GPIOB

#define CCD_R1_Pin GPIO_PIN_3
#define CCD_R1_GPIO_Port GPIOB
#define CCD_R2_Pin GPIO_PIN_4
#define CCD_R2_GPIO_Port GPIOB
#define CCD_R3_Pin GPIO_PIN_5
#define CCD_R3_GPIO_Port GPIOB
#define CCD_R_Pin GPIO_PIN_6
#define CCD_R_GPIO_Port GPIOB

// CCD_Init function is responsible for initializing the sensor
// CCD_StartCapture starts the sensor for image capture
// CCD_ReadData reads data from the sensor and stores it in the specified buffer
// CCD_DataTransfer transfers the read data to the host for further processing

// The driving process of the frame-transfer area CCD mainly works by controlling
// the timing of the image area clocks Iϕ1, Iϕ2, Iϕ3, the storage area clocks Sϕ1, Sϕ2, Sϕ3,
// the readout clocks Rϕ1, Rϕ2, Rϕ3, ϕR, and the transfer gate DG.
// This achieves two main working phases within one frame cycle: the exposure phase
// and the transfer phase. These two phases alternate, enabling continuous exposure
// and output of images.
//
void CCD_Module_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_GPIOB_CLK_ENABLE();
	HAL_GPIO_WritePin(GPIOB, CCD_I1_Pin|CCD_I2_Pin|CCD_I3_Pin|
	                          CCD_R1_Pin|CCD_R2_Pin|CCD_R3_Pin
	                          |CCD_R_Pin, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = CCD_I1_Pin|CCD_I2_Pin|CCD_I3_Pin
	                          |CCD_R1_Pin|CCD_R2_Pin|CCD_R3_Pin
	                          |CCD_R_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOB, CCD_I1_Pin|CCD_I2_Pin|CCD_I3_Pin|
	                          CCD_R3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, CCD_R1_Pin|CCD_R2_Pin|CCD_R_Pin, GPIO_PIN_SET);
}
//
void CCD_StartCapture(void)
{
	uint16_t i;

	HAL_GPIO_WritePin(GPIOB, CCD_I1_Pin|CCD_I2_Pin|CCD_I3_Pin|
		                          CCD_R3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, CCD_R1_Pin|CCD_R2_Pin|CCD_R_Pin, GPIO_PIN_SET);
	for(i=0;i<1027;i++)
	{

		HAL_GPIO_WritePin(GPIOB,CCD_R1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,CCD_R2_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,CCD_R3_Pin,GPIO_PIN_SET);

		HAL_GPIO_WritePin(GPIOB,CCD_R_Pin,GPIO_PIN_RESET);
		bsp_DelayUS(2);
		HAL_GPIO_WritePin(GPIOB,CCD_R1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,CCD_R2_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB,CCD_R3_Pin,GPIO_PIN_RESET);

		HAL_GPIO_WritePin(GPIOB,CCD_R_Pin,GPIO_PIN_SET);

		HAL_GPIO_WritePin(GPIOB,CCD_I1_Pin,GPIO_PIN_SET);
		bsp_DelayUS(2);
		HAL_GPIO_WritePin(GPIOB,CCD_I1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,CCD_I2_Pin,GPIO_PIN_SET);
		bsp_DelayUS(2);
		HAL_GPIO_WritePin(GPIOB,CCD_I2_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,CCD_I3_Pin,GPIO_PIN_SET);
		bsp_DelayUS(2);
		HAL_GPIO_WritePin(GPIOB,CCD_I3_Pin,GPIO_PIN_RESET);


	}

}
//
void CCD_StopCapture(void)
{
	HAL_GPIO_WritePin(GPIOB, CCD_I1_Pin|CCD_I2_Pin|CCD_I3_Pin|
		                          CCD_R3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, CCD_R1_Pin|CCD_R2_Pin|CCD_R_Pin, GPIO_PIN_SET);

}
