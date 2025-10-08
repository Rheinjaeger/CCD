/****************************************Copyright (c)****************************************************
**
**
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               bsp_w5500.c
** Descriptions:            w5500 Driver
** IC MODEL:								STM32F303ZE
**--------------------------------------------------------------------------------------------------------
** Created by:
** Created date:            2025-08-26
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:
** Modified date:
** Version:
** Descriptions:
**
*********************************************************************************************************/
#include "stm32l0xx_hal.h"
#include "bsp_timer.h"
//#include "mb.h"
//#include "param.h"
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MB_TCP_PORT_USE_DEFAULT 502
// extern uint8_t W5500_INT_Flag;
// W5500 IO definitions
// Define the W5500 CS pin
#define W5500_SCS              GPIO_PIN_4
#define W5500_SCS_PORT         GPIOA
// Define the W5500 RST pin
#define W5500_RST              GPIO_PIN_10
#define W5500_RST_PORT         GPIOB
// Define the W5500 INT pin
#define W5500_INT              GPIO_PIN_8
#define W5500_INT_PORT         GPIOA


//
#define SPIx_SCK_PIN                     GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT               GPIOA

#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT              GPIOA

#define SPIx_MOSI_PIN                    GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT              GPIOA

//Common Register
#define MR			0x0000
#define RST			0x80
#define WOL			0x20
#define PB			0x10
#define PPP			0x08
#define FARP		0x02

#define GAR			0x0001
#define SUBR		0x0005
#define SHAR		0x0009
#define SIPR		0x000F

#define INTLEVEL	0x0013
#define IR			0x0015
#define CONFLICT	0x80
#define UNREACH		0x40
#define PPPOE		0x20
#define MP			0x10

#define IMR			0x0016
#define IM_IR7		0x80
#define IM_IR6		0x40
#define IM_IR5		0x20
#define IM_IR4		0x10

#define SIR			0x0017
#define S7_INT		0x80
#define S6_INT		0x40
#define S5_INT		0x20
#define S4_INT		0x10
#define S3_INT		0x08
#define S2_INT		0x04
#define S1_INT		0x02
#define S0_INT		0x01

#define SIMR		0x0018
#define S7_IMR		0x80
#define S6_IMR		0x40
#define S5_IMR		0x20
#define S4_IMR		0x10
#define S3_IMR		0x08
#define S2_IMR		0x04
#define S1_IMR		0x02
#define S0_IMR		0x01

#define RTR			0x0019
#define RCR			0x001B

#define PTIMER		0x001C
#define PMAGIC		0x001D
#define PHA			0x001E
#define PSID		0x0024
#define PMRU		0x0026

#define UIPR		0x0028
#define UPORT		0x002C

#define PHYCFGR		0x002E
#define RST_PHY		0x80
#define OPMODE		0x40
#define DPX			0x04
#define SPD			0x02
#define LINK		0x01

#define VERR		0x0039

/********************* Socket Register *******************/
#define Sn_MR		0x0000
	#define MULTI_MFEN		0x80
	#define BCASTB			0x40
	#define	ND_MC_MMB		0x20
	#define UCASTB_MIP6B	0x10
	#define MR_CLOSE		0x00
	#define MR_TCP			0x01
	#define MR_UDP			0x02
	#define MR_MACRAW		0x04

#define Sn_CR		0x0001
	#define OPEN			0x01
	#define LISTEN			0x02
	#define CONNECT			0x04
	#define DISCON			0x08
	#define CLOSE			0x10
	#define SEND			0x20
	#define SEND_MAC		0x21
	#define SEND_KEEP		0x22
	#define RECV			0x40

#define Sn_IR		0x0002
	#define IR_SEND_OK		0x10
	#define IR_TIMEOUT		0x08
	#define IR_RECV			0x04
	#define IR_DISCON		0x02
	#define IR_CON			0x01
//-----------------------------------------
#define Sn_SR		0x0003
	#define SOCK_CLOSED				0x00//closed
	#define SOCK_INIT					0x13//init state
	#define SOCK_LISTEN				0x14//listen state
	#define SOCK_ESTABLISHED	0x17//success to connect
	#define SOCK_CLOSE_WAIT		0x1C//close state
	#define SOCK_UDP					0x22
	#define SOCK_MACRAW				0x02

	#define SOCK_SYNSEND			0x15
	#define SOCK_SYNRECV			0x16
	#define SOCK_FIN_WAI			0x18//close state
	#define SOCK_CLOSING			0x1A//close state
	#define SOCK_TIME_WAIT		0x1B//close state
	#define SOCK_LAST_ACK			0x1D//close state
//-----------------------------------------
#define Sn_PORT			0x0004
#define Sn_DHAR	   		0x0006
#define Sn_DIPR			0x000C
#define Sn_DPORTR		0x0010

#define Sn_MSSR			0x0012
#define Sn_TOS			0x0015
#define Sn_TTL			0x0016

#define Sn_RXBUF_SIZE	0x001E
#define Sn_TXBUF_SIZE	0x001F
#define Sn_TX_FSR		0x0020
#define Sn_TX_RD		0x0022
#define Sn_TX_WR		0x0024
#define Sn_RX_RSR		0x0026
#define Sn_RX_RD		0x0028
#define Sn_RX_WR		0x002A

#define Sn_IMR			0x002C
	#define IMR_SENDOK	0x10
	#define IMR_TIMEOUT	0x08
	#define IMR_RECV	0x04
	#define IMR_DISCON	0x02
	#define IMR_CON		0x01

#define Sn_FRAG			0x002D
#define Sn_KPALVTR		0x002F

//-----------------------------------------
//Operation mode bits
#define VDM		0x00
#define FDM1	0x01
#define	FDM2	0x02
#define FDM4	0x03

//Read_Write control bit
#define RWB_READ	0x00
#define RWB_WRITE	0x04

//Block select bits
#define COMMON_R	0x00

//Socket 0
#define S0_REG		0x08
#define S0_TX_BUF	0x10
#define S0_RX_BUF	0x18

//Socket 1
#define S1_REG		0x28
#define S1_TX_BUF	0x30
#define S1_RX_BUF	0x38

//Socket 2
#define S2_REG		0x48
#define S2_TX_BUF	0x50
#define S2_RX_BUF	0x58

//Socket 3
#define S3_REG		0x68
#define S3_TX_BUF	0x70
#define S3_RX_BUF	0x78

//Socket 4
#define S4_REG		0x88
#define S4_TX_BUF	0x90
#define S4_RX_BUF	0x98

//Socket 5
#define S5_REG		0xa8
#define S5_TX_BUF	0xb0
#define S5_RX_BUF	0xb8

/* Socket 6 */
#define S6_REG		0xc8
#define S6_TX_BUF	0xd0
#define S6_RX_BUF	0xd8

/* Socket 7 */
#define S7_REG		0xe8
#define S7_TX_BUF	0xf0
#define S7_RX_BUF	0xf8

/* Define data size and data masks */
#ifdef SPI_DATASIZE_8
 #define SPI_DATASIZE                     SPI_DataSize_8b
 #define SPI_DATAMASK                     (uint8_t)0xFF
#elif defined (SPI_DATASIZE_7)
 #define SPI_DATASIZE                     SPI_DataSize_7b
 #define SPI_DATAMASK                     (uint8_t)0x7F
#elif defined (SPI_DATASIZE_6)
 #define SPI_DATASIZE                     SPI_DataSize_6b
 #define SPI_DATAMASK                     (uint8_t)0x3F
#elif defined (SPI_DATASIZE_5)
 #define SPI_DATASIZE                     SPI_DataSize_5b
 #define SPI_DATAMASK                     (uint8_t)0x1F
#endif

#define TCP_SERVER      0x00 // TCP server mode
#define TCP_CLIENT      0x01 // TCP client mode
#define UDP_MODE        0x02 // UDP (broadcast) mode

#define S_INIT          0x01 // Port initialized
#define S_CONN          0x02 // Port connected, data can be transmitted normally

#define S_RECEIVE       0x01 // Port received a data packet
#define S_TRANSMITOK    0x02 // Port finished sending a data packet

#define S_RX_SIZE       2048 // Define the size of the Socket receive buffer, can be modified according to W5500_RMSR
#define S_TX_SIZE       2048 // Define the size of the Socket transmit buffer, can be modified according to W5500_TMSR

typedef uint8_t SOCKET;  // Custom data type for socket number

// Network parameter variable definitions
uint8_t Gateway_IP[4];   // Gateway IP address
uint8_t Sub_Mask[4];     // Subnet mask
uint8_t Phy_Addr[6];     // Physical address (MAC)
uint8_t IP_Addr[4];      // Local IP address

uint8_t S0_DIP[4];       // Destination IP address for socket 0

uint8_t UDP_DIPR[4];     // Destination IP address for UDP (broadcast) mode
uint8_t UDP_DPORT[2];    // Destination port number for UDP (broadcast) mode

// Port operating mode
uint8_t S0_Mode = 3;     // Operating mode of socket 0: 0 = TCP server, 1 = TCP client, 2 = UDP (broadcast)

// Port operating state
uint8_t S0_State = 0;    // Status record of socket 0: 1 = initialized, 2 = connected (data transmission possible)

// Port data transmission state
uint8_t S0_Data;         // Data transmission state of socket 0: 1 = received data, 2 = transmission complete

// Port data buffers
uint8_t Rx_Buffer[2048]; // Receive buffer for socket
uint8_t Tx_Buffer[2048]; // Transmit buffer for socket



// W5500 GPIO initialization configuration
void bsp_InitW5500Gpio(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

//    EXTI_InitTypeDef  EXTI_InitStructure;

    // Enable GPIO clocks
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // W5500_RST pin initialization configuration (PA9 -> PB10)
    GPIO_InitStructure.Pin  = W5500_RST;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);
    HAL_GPIO_WritePin(W5500_RST_PORT, W5500_RST, GPIO_PIN_RESET);

/*
        //W5500_INT pin initialization configuration (PD8)
		GPIO_InitStructure.GPIO_Pin = W5500_INT;
		GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);

		//Whenever using external interrupts, RCC_APB2Periph_SYSCFG must be enabled
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		//Connect EXTI Line4 to PD8
		SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, GPIO_PinSource8);
*/
		//Configure Button EXTI line PD8 as W5500 interrupt input

//		EXTI_InitStructure.EXTI_Line = EXTI_Line8;
//		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
//		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//		EXTI_Init(&EXTI_InitStructure);

        /* Configure the chip select line as push-pull output mode */

		GPIO_InitStructure.Pin  = W5500_SCS;
		GPIO_InitStructure.Speed=GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(W5500_SCS_PORT, &GPIO_InitStructure);

		//Configure SCK, MISO 、 MOSI
		//SCK pin configuration
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
		GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
		GPIO_InitStructure.Pull  = GPIO_NOPULL;
		GPIO_InitStructure.Pin = SPIx_SCK_PIN;
		HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStructure);
    //MOSI pin configuration
		GPIO_InitStructure.Pin = SPIx_MOSI_PIN;
		HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStructure);
		//MISO pin configuration
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
		GPIO_InitStructure.Pull  = GPIO_PULLUP;
		GPIO_InitStructure.Pin = SPIx_MISO_PIN;
		HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStructure);

		HAL_GPIO_WritePin( W5500_SCS_PORT,W5500_SCS,GPIO_PIN_SET);//Set CS to High
		HAL_GPIO_WritePin( SPIx_SCK_GPIO_PORT,SPIx_SCK_PIN,GPIO_PIN_SET);	//Set SCK to High
		HAL_GPIO_WritePin( SPIx_MOSI_GPIO_PORT,SPIx_MOSI_PIN,GPIO_PIN_SET);	//Set MOSI to High
}
/*
*********************************************************************************************************
*   Function Name: sf_spi_RW
*   Description:   Send 1 byte to the device while sampling the data returned from the MISO line
*   Parameter:     TxDat : The byte value to send
*   Return Value:  The data returned from the device via the MISO line
*********************************************************************************************************
*/
uint8_t sf_spi_RW(uint8_t TxDat)
{
	uint8_t i;
	uint8_t ret;
	ret=0;
	for(i=0;i<8;i++)
	{
		HAL_GPIO_WritePin(SPIx_SCK_GPIO_PORT,SPIx_SCK_PIN,GPIO_PIN_RESET);//SCK=0
		if(TxDat&0x80)
			HAL_GPIO_WritePin(SPIx_MOSI_GPIO_PORT,SPIx_MOSI_PIN,GPIO_PIN_SET);
		else
			HAL_GPIO_WritePin(SPIx_MOSI_GPIO_PORT,SPIx_MOSI_PIN,GPIO_PIN_RESET);
		TxDat<<=1;
		HAL_GPIO_WritePin(SPIx_SCK_GPIO_PORT,SPIx_SCK_PIN,GPIO_PIN_SET);//SCK=1
		ret<<=1;
		if(HAL_GPIO_ReadPin(SPIx_MISO_GPIO_PORT,SPIx_MISO_PIN)==1)
			ret+=1;
	}
	return ret;
}
/*
*********************************************************************************************************
*   Function Name: sf_spiWriteWord
*   Description:   Send 2 bytes to the device
*   Parameter:     TxDat
*   Return Value:  None
*********************************************************************************************************
*/

void sf_spiWriteWord(uint16_t TxDat)
{
	sf_spi_RW(TxDat/256);
	sf_spi_RW(TxDat);
}
// Write a single byte to the specified register via SPI
void Write_W5500_Byte(uint16_t reg, uint8_t dat)
{
    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set the SCS of W5500 to Low

    sf_spiWriteWord(reg); // Write 16-bit register address
    // Write control byte: 1-byte data length, write data, select common register
    sf_spi_RW(dat); // Write 1 byte of data

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set the SCS of W5500 to High
}

// Write two bytes to the specified register via SPI
void Write_W5500_Word(uint16_t reg, uint16_t dat)
{
    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set the SCS of W5500 to Low

    sf_spiWriteWord(reg); // Write 16-bit register address
    sf_spi_RW(FDM2 | RWB_WRITE | COMMON_R); // Write control byte: 2-byte data length, write data, select common register
    sf_spiWriteWord(dat); // Write 16-bit data

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set the SCS of W5500 to High
}

// Write multiple bytes to the specified register via SPI
void Write_W5500_String(uint16_t reg, uint8_t *dat_ptr, uint16_t size)
{
    uint16_t i;

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set the SCS of W5500 to Low

    sf_spiWriteWord(reg); // Write 16-bit register address
    sf_spi_RW(VDM | RWB_WRITE | COMMON_R); // Write control byte: N-byte data length, write data, select common register

    for(i = 0; i < size; i++) // Loop to write 'size' bytes from buffer into W5500
    {
        sf_spi_RW(*dat_ptr++); // Write 1 byte of data
    }

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set the SCS of W5500 to High
}

//
void Write_W5500_SOCK_Byte(SOCKET s, uint16_t reg, uint8_t dat)
{
    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET);

    sf_spiWriteWord(reg); // Write 16-bit register address
    sf_spi_RW(FDM1 | RWB_WRITE | (s * 0x20 + 0x08)); // Write control byte: 1-byte data length, write data, select socket s register
    sf_spi_RW(dat); // Write 1 byte of data

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET);
}


/*******************************************************************************
* Function Name : Write_W5500_SOCK_2Byte
* Description   : Write 2 bytes of data to the specified socket register via SPI
* Input         : s: socket number, reg: 16-bit register address, dat: 16-bit data to be written (2 bytes)
* Output        : None
* Return Value  : None
* Notes         : None
*******************************************************************************/
void Write_W5500_SOCK_Word(SOCKET s, uint16_t reg, uint16_t dat)
{
    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET);

    sf_spiWriteWord(reg); // Write 16-bit register address
    sf_spi_RW(FDM2 | RWB_WRITE | (s * 0x20 + 0x08)); // Write control byte: 2-byte data length, write data, select socket s register
    sf_spiWriteWord(dat); // Write 16-bit data

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET);
}

/*******************************************************************************
* Function Name : Write_W5500_SOCK_4Byte
* Description   : Write 4 bytes of data to the specified socket register via SPI
* Input         : s: socket number, reg: 16-bit register address, *dat_ptr: pointer to buffer containing 4 bytes of data
* Output        : None
* Return Value  : None
* Notes         : None
*******************************************************************************/
void Write_W5500_SOCK_4Byte(SOCKET s, uint16_t reg, uint8_t *dat_ptr)
{
    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set W5500 SCS low

    sf_spiWriteWord(reg); // Write 16-bit register address
    sf_spi_RW(FDM4 | RWB_WRITE | (s * 0x20 + 0x08)); // Write control byte: 4-byte data length, write data, select socket s register

    sf_spi_RW(*dat_ptr++); // Write 1st byte of data
    sf_spi_RW(*dat_ptr++); // Write 2nd byte of data
    sf_spi_RW(*dat_ptr++); // Write 3rd byte of data
    sf_spi_RW(*dat_ptr++); // Write 4th byte of data

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set W5500 SCS high
}


/*******************************************************************************
* Function Name : Read_W5500_Byte
* Description   : Read 1 byte of data from the specified W5500 register
* Input         : reg: 16-bit register address
* Output        : None
* Return Value  : The 1-byte data read from the register
* Notes         : None
*******************************************************************************/
uint8_t Read_W5500_Byte(uint16_t reg)
{
    uint8_t ret;

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set W5500 SCS low
    // Write 16-bit register address
    sf_spiWriteWord(reg);
    // Write control byte: 1-byte data length, read data, select common register
    sf_spi_RW(FDM1 | RWB_READ | COMMON_R);
    // Send one dummy byte to provide 8 SCLK cycles to read MISO data
    ret = sf_spi_RW(0x00);

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set W5500 SCS high
    return ret; // Return the read register data
}

/*******************************************************************************
* Function Name : Read_W5500_SOCK_Byte
* Description   : Read 1 byte of data from the specified W5500 socket register
* Input         : s: socket number, reg: 16-bit register address
* Output        : None
* Return Value  : The 1-byte data read from the socket register
* Notes         : None
*******************************************************************************/
uint8_t Read_W5500_SOCK_Byte(SOCKET s, uint16_t reg)
{
    uint8_t ret;

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set W5500 SCS low

    sf_spiWriteWord(reg); // Write 16-bit register address
    sf_spi_RW(FDM1 | RWB_READ | (s * 0x20 + 0x08)); // Write control byte: 1-byte data length, read data, select socket s register

    // Send one dummy byte to provide 8 SCLK cycles to read MISO data
    ret = sf_spi_RW(0x00);

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set W5500 SCS high
    return ret; // Return the read socket register data
}


/*******************************************************************************
* Function Name : Read_W5500_SOCK_Word
* Description   : Read 2 bytes of data from the specified W5500 socket register
* Input         : s: socket number, reg: 16-bit register address
* Output        : None
* Return Value  : The 2-byte (16-bit) data read from the register
* Notes         : None
*******************************************************************************/
uint16_t Read_W5500_SOCK_Word(SOCKET s, uint16_t reg)
{
    uint16_t ret;

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set W5500 SCS low

    sf_spiWriteWord(reg); // Write 16-bit register address
    sf_spi_RW(FDM2 | RWB_READ | (s * 0x20 + 0x08)); // Write control byte: 2-byte data length, read, select socket s register

    ret = sf_spi_RW(0x00); // Read high byte
    ret *= 256;
    ret += sf_spi_RW(0x00); // Read low byte

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set W5500 SCS high
    return ret; // Return the read register data
}

/*******************************************************************************
* Function Name : Read_SOCK_Data_Buffer
* Description   : Read data from the W5500 receive buffer
* Input         : s: socket number, *dat_ptr: pointer to the buffer to store data
* Output        : None
* Return Value  : Length of data read, rx_size bytes
* Notes         : None
*******************************************************************************/
uint16_t Read_SOCK_Data_Buffer(SOCKET s, uint8_t *dat_ptr)
{
    uint16_t rx_size;
    uint16_t offset, offset1;
    uint16_t i;
    uint8_t val;

    rx_size = Read_W5500_SOCK_Word(s, Sn_RX_RSR);
    if (rx_size == 0)
        return 0; // Return if no data received
    if (rx_size > 1460)
        rx_size = 1460;

    offset = Read_W5500_SOCK_Word(s, Sn_RX_RD);
    offset1 = offset;
    offset &= (S_RX_SIZE - 1); // Calculate the actual physical address

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set W5500 SCS low

    sf_spiWriteWord(offset); // Write 16-bit address
    sf_spi_RW(VDM | RWB_READ | (s * 0x20 + 0x18)); // Write control byte: N-byte length, read, select socket s register

    if ((offset + rx_size) < S_RX_SIZE) // If the end address doesn't exceed the RX buffer boundary
    {
        for (i = 0; i < rx_size; i++) // Read rx_size bytes
        {
            val = sf_spi_RW(0x00); // Send a dummy byte to read 1 byte
            *dat_ptr = val;        // Store the read data into the destination buffer
            dat_ptr++;             // Increment destination buffer pointer
        }
    }
    else // If the end address exceeds the RX buffer boundary
    {
        offset = S_RX_SIZE - offset;
        for (i = 0; i < offset; i++) // Read the first 'offset' bytes
        {
            val = sf_spi_RW(0x00); // Send a dummy byte to read 1 byte
            *dat_ptr = val;        // Store the read data into the destination buffer
            dat_ptr++;             // Increment destination buffer pointer
        }
        HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET);  // Set W5500 SCS high

        HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set W5500 SCS low

        sf_spiWriteWord(0x00); // Write 16-bit address
        sf_spi_RW(VDM | RWB_READ | (s * 0x20 + 0x18)); // Write control byte: N-byte length, read, select socket s register

        for (; i < rx_size; i++) // Read the remaining (rx_size - offset) bytes
        {
            val = sf_spi_RW(0x00); // Send a dummy byte to read 1 byte
            *dat_ptr = val;        // Store the read data into the destination buffer
            dat_ptr++;             // Increment destination buffer pointer
        }
    }
    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set W5500 SCS high

    offset1 += rx_size; // Update the physical address for the next read starting position
    Write_W5500_SOCK_Word(s, Sn_RX_RD, offset1);
    Write_W5500_SOCK_Byte(s, Sn_CR, RECV); // Issue receive command
    return rx_size; // Return the length of received data
}


/*******************************************************************************
* Function Name : Write_SOCK_Data_Buffer
* Description   : Write data into the W5500 transmit buffer
* Input         : s: socket number, *dat_ptr: pointer to the data buffer, size: length of data to write
* Output        : None
* Return Value  : None
* Notes         : None
*******************************************************************************/
void Write_SOCK_Data_Buffer(SOCKET s, uint8_t *dat_ptr, uint16_t size)
{
    uint16_t offset, offset1;
    uint16_t i;

    // If in UDP mode, set the destination host IP and port here
    if ((Read_W5500_SOCK_Byte(s, Sn_MR) & 0x0F) != SOCK_UDP) // If socket open failed
    {
        Write_W5500_SOCK_4Byte(s, Sn_DIPR, UDP_DIPR); // Set destination host IP
        Write_W5500_SOCK_Word(s, Sn_DPORTR, UDP_DPORT[0]*256 + UDP_DPORT[1]); // Set destination host port
    }

    offset = Read_W5500_SOCK_Word(s, Sn_TX_WR);
    offset1 = offset;
    offset &= (S_TX_SIZE - 1); // Calculate the actual physical address

    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set W5500 SCS low

    sf_spiWriteWord(offset); // Write 16-bit address
    sf_spi_RW(VDM | RWB_WRITE | (s * 0x20 + 0x10)); // Write control byte: N-byte length, write, select socket s register

    if ((offset + size) < S_TX_SIZE) // If end address doesn't exceed TX buffer boundary
    {
        for (i = 0; i < size; i++) // Write 'size' bytes
        {
            sf_spi_RW(*dat_ptr++); // Write one byte
        }
    }
    else // If end address exceeds TX buffer boundary
    {
        offset = S_TX_SIZE - offset;
        for (i = 0; i < offset; i++) // Write the first 'offset' bytes
        {
            sf_spi_RW(*dat_ptr++); // Write one byte
        }
        HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET);  // Set W5500 SCS high

        HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_RESET); // Set W5500 SCS low

        sf_spiWriteWord(0x00); // Write 16-bit address
        sf_spi_RW(VDM | RWB_WRITE | (s * 0x20 + 0x10)); // Write control byte: N-byte length, write, select socket s register

        for (; i < size; i++) // Write the remaining (size - offset) bytes
        {
            sf_spi_RW(*dat_ptr++); // Write one byte
        }
    }
    HAL_GPIO_WritePin(W5500_SCS_PORT, W5500_SCS, GPIO_PIN_SET); // Set W5500 SCS high

    offset1 += size; // Update the physical address for the next write start position
    Write_W5500_SOCK_Word(s, Sn_TX_WR, offset1);
    Write_W5500_SOCK_Byte(s, Sn_CR, SEND); // Issue send command
}


//
void W5500HardwareReset(void)
{
    uint8_t count;
    count = 5;
    HAL_GPIO_WritePin(W5500_RST_PORT, W5500_RST, GPIO_PIN_RESET); // Pull reset pin low
    bsp_DelayMS(20);
    HAL_GPIO_WritePin(W5500_RST_PORT, W5500_RST, GPIO_PIN_SET);   // Pull reset pin high
    bsp_DelayMS(100);
    // Wait for the Ethernet link to come up
    while (count--)
    {
        if ((Read_W5500_Byte(PHYCFGR) & LINK) == 1)
            break;
    }
}
//
void Load_Net_Parameters(void)
{
    Gateway_IP[0] = 172; // Load gateway parameters
    Gateway_IP[1] = 21;
    Gateway_IP[2] = 96;
    Gateway_IP[3] = 2;

    //172.21.96.1

    Sub_Mask[0] = 255;   // Load subnet mask
    Sub_Mask[1] = 255;
    Sub_Mask[2] = 255;
    Sub_Mask[3] = 0;

    Phy_Addr[0] = 0x0c;  // Load physical address (MAC)
    Phy_Addr[1] = 0x29;
    Phy_Addr[2] = 0xab;
    Phy_Addr[3] = 0x7c;
    Phy_Addr[4] = 0x00;
    Phy_Addr[5] = 0x01;

    IP_Addr[0] = 172;    // Load local IP address
    IP_Addr[1] = 21;
    IP_Addr[2] = 96;
    IP_Addr[3] = 2;

    // Load destination IP for socket 0 (e.g., a PC for debugging)
    UDP_DIPR[0] = 172;
    UDP_DIPR[1] = 21;
    UDP_DIPR[2] = 96;
    UDP_DIPR[3] = 2;
    // Load destination port for socket 0 (e.g., a PC for debugging), 6000
    UDP_DPORT[0] = 0x17;
    UDP_DPORT[1] = 0x70;
    S0_Mode = UDP_MODE;  // Load socket 0 working mode: UDP (broadcast) mode
}
/**
@brief  W5500 keep-alive routine: set the socket keep-alive register Sn_KPALVTR,
        unit is 5 seconds
*/

void setkeepalive(SOCKET s)
{
	Write_W5500_SOCK_Byte(s,Sn_KPALVTR,2);
}
//
void W5500_Init(void)
{
    uint8_t i = 0;

    Write_W5500_Byte(MR, RST); // Software reset W5500; write 1 to enable, it auto-clears after reset
    bsp_DelayMS(10);           // Delay 10 ms (user-defined function)

    // Set gateway IP address. Gateway_IP is a 4-byte unsigned char array (user-defined).
    // Using a gateway allows communication beyond the local subnet, reaching other subnets or the Internet.
    Write_W5500_String(GAR, Gateway_IP, 4);

    // Set subnet mask (MASK). Sub_Mask is a 4-byte unsigned char array (user-defined).
    // The subnet mask is used for subnet calculations.
    Write_W5500_String(SUBR, Sub_Mask, 4);

    // Set MAC address. Phy_Addr is a 6-byte unsigned char array (user-defined),
    // used to uniquely identify the device on the network.
    // This address should be obtained from IEEE. Per OUI rules, the first 3 bytes are the vendor code,
    // and the last 3 bytes are the product serial number.
    // If you define a MAC yourself, note the first byte must be even.
    Write_W5500_String(SHAR, Phy_Addr, 6);

    // Set the device’s IP address. IP_Addr is a 4-byte unsigned char array (user-defined).
    // Note: The gateway IP must be in the same subnet as the local IP, otherwise the device won't find the gateway.
    Write_W5500_String(SIPR, IP_Addr, 4);

    // Configure TX and RX buffer sizes; refer to the W5500 datasheet
    for (i = 0; i < 8; i++)
    {
        Write_W5500_SOCK_Byte(i, Sn_RXBUF_SIZE, 0x02); // Socket RX memory size = 2 KB
        Write_W5500_SOCK_Byte(i, Sn_TXBUF_SIZE, 0x02); // Socket TX memory size = 2 KB
    }

    // Configure retry time; default is 2000 (200 ms)
    // Each unit is 100 µs. Set to 2000 (0x07D0) at init, which equals 200 ms.
    // Write_W5500_Word(RTR, 0x07D0);

    // Configure retry count; default is 8
    // If retransmissions exceed this value, a timeout interrupt is generated (Sn_IR TIMEOUT bit set to 1).
    // Write_W5500_Byte(RCR, 8);

    // Enable interrupts; see the W5500 datasheet to choose types as needed.
    // IMR_CONFLICT: IP address conflict exception interrupt
    // IMR_UNREACH: Destination unreachable exception interrupt in UDP communication
    // Other socket event interrupts can be added as required.
    // Write_W5500_Byte(IMR, IM_IR7 | IM_IR6);
    // Write_W5500_Byte(SIMR, S0_IMR);
    // Write_W5500_SOCK_Byte(0, Sn_IMR, IMR_SENDOK | IMR_TIMEOUT | IMR_RECV | IMR_DISCON | IMR_CON);
}


//
void Socket0_Config(uint16_t usTCPPort)
{
    // set Socket n Port Number
    Write_W5500_SOCK_Word(0, Sn_PORT, usTCPPort);

    Write_W5500_SOCK_Word(0, RTR, 2000);
    // Set retry count; default is 8
    Write_W5500_SOCK_Byte(0, RCR, 3);
    // Set Maximum Segment Size as 1460
    Write_W5500_SOCK_Word(0, Sn_MSSR, 1460);
}
//
uint8_t Socket_Listen(SOCKET s)
{
    /* Set Socket n in TCP mode */
    Write_W5500_SOCK_Byte(s, Sn_MR, MR_TCP | ND_MC_MMB); // |ND_MC_MMB);//2017.10
    /* Open Socket n */
    Write_W5500_SOCK_Byte(s, Sn_CR, OPEN);

    bsp_DelayMS(5); /* Wait for a moment */
    if (Read_W5500_SOCK_Byte(s, Sn_SR) != SOCK_INIT)
    {
        Write_W5500_SOCK_Byte(s, Sn_CR, CLOSE); /* Close Socket n */
        return FALSE;
    }
    Write_W5500_SOCK_Byte(s, Sn_CR, LISTEN); // Set socket to listen mode
    bsp_DelayMS(5); // Delay 5 ms
    if (Read_W5500_SOCK_Byte(s, Sn_SR) != SOCK_LISTEN) // If socket configuration failed
    {
        Write_W5500_SOCK_Byte(s, Sn_CR, CLOSE); // If not successful, close socket
        return FALSE; // Return FALSE (0x00)
    }
    //
    Write_W5500_SOCK_Byte(0, Sn_KPALVTR, 0x02); //
    return TRUE;
}


/*******************************************************************************
* Function Name : Detect_Gateway
* Description   : Check the gateway server
* Input         : None
* Output        : None
* Return Value  : Returns TRUE (0xFF) on success, FALSE (0x00) on failure
* Notes         : None
*******************************************************************************/
uint8_t Detect_Gateway(void)
{
    uint8_t i;
    uint8_t ip_adde[4];

    i = 0;
    ip_adde[0] = IP_Addr[0] + 1;
    ip_adde[1] = IP_Addr[1] + 1;
    ip_adde[2] = IP_Addr[2] + 1;
    ip_adde[3] = IP_Addr[3] + 1;

    // Check the gateway and obtain the gateway’s MAC address
    Write_W5500_SOCK_4Byte(0, Sn_DIPR, ip_adde); // Write an IP different from the local IP to the destination address register
    Write_W5500_SOCK_Byte(0, Sn_MR, MR_TCP);     // Set socket to TCP mode
    Write_W5500_SOCK_Byte(0, Sn_CR, OPEN);       // Open the socket
    bsp_DelayMS(5);                               // Delay 5 ms

    if (Read_W5500_SOCK_Byte(0, Sn_SR) != SOCK_INIT) // If opening the socket failed
    {
        Write_W5500_SOCK_Byte(0, Sn_CR, CLOSE);      // Close the socket if not successful
        return FALSE;                                 // Return FALSE (0x00)
    }

    Write_W5500_SOCK_Byte(0, Sn_CR, CONNECT);    // Set socket to CONNECT mode

    do
    {
        i = 0;
        i = Read_W5500_SOCK_Byte(0, Sn_IR);      // Read Socket0 interrupt register
        if (i != 0)
            Write_W5500_SOCK_Byte(0, Sn_IR, i);
        bsp_DelayMS(5);                           // Delay 5 ms
        if ((i & IR_TIMEOUT) == IR_TIMEOUT)
            return FALSE;
        else if (Read_W5500_SOCK_Byte(0, Sn_DHAR) != 0xFF)
        {
            Write_W5500_SOCK_Byte(0, Sn_CR, CLOSE); // Close socket
            return TRUE;
        }
    } while (1);
}
/*******************************************************************************
* Function Name : Process_Socket_Data
* Description   : W5500 receives data and echoes the received data
* Input         : s: socket number
* Output        : None
* Return Value  : None
* Notes         : This routine first calls S_rx_process() to read data from the
*                 W5500 socket RX buffer into Rx_Buffer, then copies it to
*                 Temp_Buffer for processing. After processing, it copies the
*                 data from Temp_Buffer to Tx_Buffer and calls S_tx_process()
*                 to send it.
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
    unsigned short size;
    size = Read_SOCK_Data_Buffer(s, Rx_Buffer);
    memcpy(Tx_Buffer, Rx_Buffer, size);
    Write_SOCK_Data_Buffer(s, Tx_Buffer, size);
}
/*******************************************************************************
* Function Name : W5500_Interrupt_Process
* Description   : W5500 interrupt handling framework
* Input         : None
* Output        : None
* Return Value  : None
* Notes         : None
*******************************************************************************/
void W5500_Interrupt_Process(void)
{
    uint8_t i, j;

IntDispose:
    // W5500_INT_Flag = 0; // Clear interrupt flag
    i = Read_W5500_Byte(IR);            // Read interrupt flag register
    Write_W5500_Byte(IR, (i & 0xf0));   // Write back to clear interrupt flags

    if ((i & CONFLICT) == CONFLICT)     // Handle IP address conflict exception
    {
        // Add your code here
    }

    if ((i & UNREACH) == UNREACH)       // Handle destination unreachable in UDP mode
    {
        // Add your code here
    }

    i = Read_W5500_Byte(SIR);           // Read socket interrupt flag register
    if ((i & S0_INT) == S0_INT)         // Handle Socket0 events
    {
        i = Read_W5500_SOCK_Byte(0, Sn_IR);   // Read Socket0 interrupt register
        Write_W5500_SOCK_Byte(0, Sn_IR, j);   // Write back to clear interrupt flags
        if (i & IR_CON)                        // In TCP mode, Socket0 connected successfully
        {
            S0_State |= S_CONN;                // Network status 0x02: socket connected, normal data transfer possible
        }
        if (i & IR_DISCON)                     // In TCP mode, handle socket disconnect
        {
            Write_W5500_SOCK_Byte(0, Sn_CR, CLOSE); // Close socket and wait to reopen
            Socket0_Config(MB_TCP_PORT_USE_DEFAULT); // Initialize socket 0
            S0_State = 0;                           // Network status 0x00: connection failed
        }
        if (i & IR_SEND_OK)                    // Socket0 data sent; S_tx_process() can be started again
        {
            S0_Data |= S_TRANSMITOK;           // One packet transmission completed
        }
        if (i & IR_RECV)                       // Data received; S_rx_process() can be started
        {
            S0_Data |= S_RECEIVE;              // One packet received
        }
        if (i & IR_TIMEOUT)                    // Handle connection or data transfer timeout
        {
            Write_W5500_SOCK_Byte(0, Sn_CR, CLOSE); // Close socket and wait to reopen
            S0_State = 0;                           // Network status 0x00: connection failed
        }
    }

    if (Read_W5500_Byte(SIR) != 0)
        goto IntDispose;
}
//
// W5500HardwareInitialize
void init_chip(void)
{
    bsp_InitW5500Gpio();
    W5500HardwareReset();
    Load_Net_Parameters();
    W5500_Init(); // Initialize W5500 registers
    // Detect_Gateway();
    // Enable W5500 Keep-alive
    setkeepalive(0);
}


