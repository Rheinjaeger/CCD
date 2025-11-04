/*
 * spimem.h
 *
 *  Created on: Oct 31, 2025
 *      Author: Admin
 */

#ifndef INC_SPIMEM_H_
#define INC_SPIMEM_H_


#include "stm32l0xx_hal.h"
#ifndef HARD_SPI
#define HARD_SPI
#endif

//#define SPI_NAND_CS_RCU          RCU_GPIOB
#define SPI_NAND_CS_GROUP        GPIOB
#define SPI_NAND_CS_PIN          GPIO_PIN_12

//#define SPI_NAND_CLK_RCU         RCU_GPIOB
#define SPI_NAND_CLK_GROUP       GPIOB
#define SPI_NAND_CLK_PIN         GPIO_PIN_13

//#define SPI_NAND_MISO_RCU        RCU_GPIOB
#define SPI_NAND_MISO_GROUP      GPIOB
#define SPI_NAND_MISO_PIN        GPIO_PIN_14

//#define SPI_NAND_MOSI_RCU        RCU_GPIOB
#define SPI_NAND_MOSI_GROUP      GPIOB
#define SPI_NAND_MOSI_PIN        GPIO_PIN_15

//#define SPI_NAND_WP_RCU          RCU_GPIOG
#define SPI_NAND_WP_GROUP        GPIOB
#define SPI_NAND_WP_PIN          GPIO_PIN_11

//#define SPI_NAND_HOLD_RCU        RCU_GPIOG
#define SPI_NAND_HOLD_GROUP      GPIOG
#define SPI_NAND_HOLD_PIN        GPIO_PIN_3

//#define SPI_NAND_SPI_RCU         RCU_SPI0
//#define SPI_NAND_SPI             SPI0

#define SOFT_SPI_RW_BIT(RW, BIT) (RW << 1 | BIT) /* RW is 0 for read, 1 for write */
#define SOFT_SPI_CS_LOW_DELAY_US 2

#include "w25n01gvzeig.h"

enum w25n01gv_spi_mode {
    normal_4line,
    normal_3line,
    dual_tx,
    dual_rx,
    quad_tx,
    quad_rx
};

/// @brief
/// @param tx_buf
/// @param tx_cnt
/// @return
int board_w25n01gv_spi_tx_interface(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t tx_io_struct);

/// @brief
/// @param tx_buf
/// @param tx_cnt
/// @param rx_buf
/// @param rx_cnt
/// @return
int board_w25n01gv_spi_tx_rx_interface(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, w25n01gv_uint8_t tx_io_struct, w25n01gv_uint8_t rx_io_struct);

/// @brief
/// @param delay_us
/// @return
int board_w25n01gv_delay_us(w25n01gv_uint16_t delay_us);

/// @brief
/// @param size
/// @return
void *board_w25n01gv_malloc(w25n01gv_uint32_t size);

/// @brief
/// @param pos
void board_w25n01gv_free(void *pos);

/// @brief
/// @param str1
/// @param str2
/// @param n
void board_w25n01gv_memcpy(void *str1, const void *str2, w25n01gv_uint32_t n);

/// @brief w25n01 NAND Flash init
/// @param spi_mode 0 hard spi, 1 soft spi
/// @return
int board_w25n01gv_init();


#endif /* INC_SPIMEM_H_ */
