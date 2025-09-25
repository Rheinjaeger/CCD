/*
 * Temperature.c
 *
 *  Created on: Sep 13, 2025
 *      Author: Admin
 */
#include "stm32l0xx_hal.h"

extern ADC_HandleTypeDef hadc;
extern void Error_Handler(void);
#define VREF_VOLT_mV	3300

//	NTC B value is 3950
//	NTC resistance is 10 KOhm with temperature at 25 C
//	AD precision is 12Bit
//	Ntc connect Gnd
//	------------------------------------------------------------


const short AD_Result[121] = {
	0xE9D,0xE89,0xE73,0xE5D,0xE46,0xE2E,0xE15,0xDFB,0xDE0,0xDC5,
	0xDA8,0xD8A,0xD6B,0xD4C,0xD2B,0xD0A,0xCE7,0xCC4,0xCA0,0xC7B,
	0xC55,0xC2E,0xC07,0xBDE,0xBB6,0xB8C,0xB62,0xB37,0xB0B,0xADF,
	0xAB3,0xA86,0xA58,0xA2B,0x9FD,0x9CF,0x9A0,0x972,0x943,0x915,
	0x8E6,0x8B8,0x88A,0x85C,0x82E,0x800,0x7D3,0x7A6,0x779,0x74D,
	0x721,0x6F6,0x6CB,0x6A1,0x677,0x64E,0x626,0x5FE,0x5D7,0x5B1,
	0x58B,0x566,0x542,0x51E,0x4FB,0x4D9,0x4B8,0x497,0x477,0x458,
	0x43A,0x41C,0x3FF,0x3E3,0x3C7,0x3AC,0x392,0x378,0x35F,0x347,
	0x330,0x319,0x302,0x2ED,0x2D8,0x2C3,0x2AF,0x29C,0x289,0x277,
	0x265,0x254,0x243,0x233,0x223,0x214,0x205,0x1F6,0x1E8,0x1DB,
	0x1CE,0x1C1,0x1B4,0x1A8,0x19D,0x191,0x186,0x17C,0x171,0x167,
	0x15E,0x154,0x14B,0x142,0x13A,0x131,0x129,0x121,0x11A,0x112,
	0x10B
};


const char Temp_Value[121] = {
	-20,-19,-18,-17,-16,-15,-14,-13,-12,-11,

	-10,-9,-8,-7,-6,-5,-4,-3,-2,-1,

	0,1,2,3,4,5,6,7,8,9,

	10,11,12,13,14,15,16,17,18,19,

	20,21,22,23,24,25,26,27,28,29,

	30,31,32,33,34,35,36,37,38,39,

	40,41,42,43,44,45,46,47,48,49,

	50,51,52,53,54,55,56,57,58,59,

	60,61,62,63,64,65,66,67,68,69,

	70,71,72,73,74,75,76,77,78,79,

	80,81,82,83,84,85,86,87,88,89,

	90,91,92,93,94,95,96,97,98,99,

	100
};


/*
*********************************************************************************************************
*   Function Name: FndTempVal
*   Description:   Look up the table to get the temperature value
*   Parameter:     Adval
*   Return Value:  Temp
*********************************************************************************************************
*/
static int8_t FndTempVal(uint16_t val)
{
	uint8_t i;
    for (i=0;i<121;i++)
	{
		if( (val<=AD_Result[i]) && (val>AD_Result[i+1]) )
		{
			break;
		}
	}
	return(Temp_Value[i] );
}


// HAL_ADC_Start(): Start ADC conversion
// HAL_ADC_PollForConversion(): Blocking wait until conversion is complete
// HAL_ADC_GetValue(): Get the conversion result

static uint16_t ADC_GetData(uint32_t ADC_Channel)
{
   ADC_ChannelConfTypeDef sConfig = {0};
   sConfig.Channel = ADC_Channel; // channel
   sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;

   if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK) {
       Error_Handler();
   }
   HAL_ADC_Start(&hadc);
   HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
   return (uint16_t)HAL_ADC_GetValue(&hadc);
}
/*
*********************************************************************************************************
*   Function Name: ADC1_GetData
*   Description:   Get the AD conversion value of the selected ADC1 channel
*   Parameter:     ADC_Channel
*   Return Value:  Adval
*********************************************************************************************************
*/
static uint16_t ADC1_GetData(uint8_t ADC_Channel)
{
		uint16_t val;
		uint32_t mulval;

		val=ADC_GetData(ADC_Channel);

		mulval=(uint32_t)VREF_VOLT_mV*val;
		val=mulval>>12;

		return val;
}
/*
*********************************************************************************************************
*   Function Name: GetNTCTempVal
*   Description:   Read NTC temperature value
*   Parameter:     null
*   Return Value:  Temp
*********************************************************************************************************
*/
int8_t GetNTCTempVal(void)
{
	int8_t ret;
	uint16_t adval;

	adval=ADC1_GetData(ADC_CHANNEL_2);
	ret = FndTempVal(adval);
	return ret;

}
float Temp;
/*
*********************************************************************************************************
*   Function Name: GetTMP20ATempVal
*   Description:   Read internal temperature from TMP20A
*   Parameter:     null
*   Return Value:  Temp
*********************************************************************************************************
*/
//The linear transfer function for –40°C to +110°C is
//Vout =  11.77 mV / C *T + 1857.6 mV
//T = (Vout-1857.6)/11.77
void GetTMP20ATempVal(void)
{
	float res;
	uint16_t adval;
	adval=ADC1_GetData(ADC_CHANNEL_3);

	res = adval-1857.6f;
	Temp = res/11.77f;
}
