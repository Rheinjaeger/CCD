/*
 * w5500.h
 *
 *  Created on: Sep 9, 2025
 *      Author: Admin
 */

#ifndef W5500_H_
#define W5500_H_

/*
        PA4 = CS
        PA5 = SCK
        PA6 = MISO
        PA7 = MOSI
        PA9  = RST
        PA8  = INT
*/

#define SPI_DATASIZE_8
//#define SPI_DATASIZE_7
//#define SPI_DATASIZE_6
//#define SPI_DATASIZE_5

// W5500 IO definitions
// Define the W5500 CS pin
#define W5500_SCS              GPIO_PIN_4
#define W5500_SCS_PORT         GPIOA
// Define the W5500 RST pin
#define W5500_RST              GPIO_PIN_9
#define W5500_RST_PORT         GPIOA
// Define the W5500 INT pin
#define W5500_INT              GPIO_PIN_8
#define W5500_INT_PORT         GPIOA

//
#define SPIx_SCK_PIN                     GPIO_PIN_5
#define SPIx_SCK_GPIO_PORT               GPIOA
#define SPIx_SCK_SOURCE                  GPIO_PinSource13
#define SPIx_SCK_AF                      GPIO_AF_5

#define SPIx_MISO_PIN                    GPIO_PIN_6
#define SPIx_MISO_GPIO_PORT              GPIOA
#define SPIx_MISO_SOURCE                 GPIO_PinSource14
#define SPIx_MISO_AF                     GPIO_AF_5

#define SPIx_MOSI_PIN                    GPIO_PIN_7
#define SPIx_MOSI_GPIO_PORT              GPIOA
#define SPIx_MOSI_SOURCE                 GPIO_PinSource15
#define SPIx_MOSI_AF                     GPIO_AF_5

// Communication board SPIx interface
#define SPIx                             SPI2
#define SPIx_CLK                         RCC_APB1Periph_SPI2
#define SPIx_IRQn                        SPI2_IRQn

// Network parameter variable definitions
extern uint8_t Gateway_IP[4]; // Gateway IP address
extern uint8_t Sub_Mask[4];   // Subnet mask
extern uint8_t Phy_Addr[6];   // Physical address (MAC)
extern uint8_t IP_Addr[4];    // Local IP address

extern uint8_t S0_DIP[4];     // Destination IP address for socket 0

extern uint8_t UDP_DIPR[4];   // Destination host IP address in UDP (broadcast) mode
extern uint8_t UDP_DPORT[2];  // Destination host port number in UDP (broadcast) mode

extern uint8_t S0_Mode;       // Operating mode of socket 0

extern uint8_t S0_State;      // Status record for socket 0

extern uint8_t S0_Data;       // Receive/transmit status for socket 0

typedef uint8_t SOCKET;       // Custom data type for socket number

uint8_t Read_W5500_Byte(uint16_t reg);
uint8_t Read_W5500_SOCK_Byte(SOCKET s, uint16_t reg);
void Write_W5500_SOCK_Byte(SOCKET s, uint16_t reg, uint8_t dat);
uint16_t Read_SOCK_Data_Buffer(SOCKET s, uint8_t *dat_ptr);
void Write_SOCK_Data_Buffer(SOCKET s, uint8_t *dat_ptr, uint16_t size);

void Socket0_Config(uint16_t usTCPPort);
uint8_t Socket_Listen(SOCKET s);
void Process_Socket_Data(SOCKET s);
void W5500_Interrupt_Process(void);
void init_chip(void);

#endif /* W5500_H_ */
