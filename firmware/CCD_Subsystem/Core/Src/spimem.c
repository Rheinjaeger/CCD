/*
 * spimem.c
 *
 *  Created on: Oct 31, 2025
 *      Author: Admin
 */


#include "spimem.h"
#include "w25n01gvzeig.h"
#include <string.h>
#include <stdlib.h>


extern SPI_HandleTypeDef hspi2;

static enum w25n01gv_spi_mode w25n01gv_current_spi_mode = (enum w25n01gv_spi_mode)normal_4line;

/* ---------------------------------soft spi driver begin-------------------------------------------- */
#ifndef HARD_SPI
/// @brief
/// @param io 0 input, 1 output
/// @return
static int w25n01gv_spi_soft_init(w25n01gv_uint8_t io)
{
    //rcu_periph_clock_enable(SPI_NAND_CS_RCU | SPI_NAND_MISO_RCU | SPI_NAND_MOSI_RCU | SPI_NAND_CLK_RCU | SPI_NAND_WP_RCU | SPI_NAND_HOLD_RCU);
	__HAL_RCC_GPIOB_CLK_ENABLE();

    gpio_init(SPI_NAND_CS_GROUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_NAND_CS_PIN);
    gpio_init(SPI_NAND_CLK_GROUP, io ? GPIO_MODE_OUT_PP : GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_NAND_CLK_PIN);
    gpio_init(SPI_NAND_MISO_GROUP, io ? GPIO_MODE_OUT_PP : GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_NAND_MISO_PIN);
    gpio_init(SPI_NAND_MOSI_GROUP, io ? GPIO_MODE_OUT_PP : GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_NAND_MOSI_PIN);
    gpio_init(SPI_NAND_WP_GROUP, io ? GPIO_MODE_OUT_PP : GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_NAND_WP_PIN);
    gpio_init(SPI_NAND_HOLD_GROUP, io ? GPIO_MODE_OUT_PP : GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_NAND_HOLD_PIN);

    if (io) {
    	HAL_GPIO_WritePin(SPI_NAND_CS_GROUP, SPI_NAND_CS_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(SPI_NAND_CLK_GROUP, SPI_NAND_CLK_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(SPI_NAND_MISO_GROUP, SPI_NAND_MISO_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(SPI_NAND_MOSI_GROUP, SPI_NAND_MOSI_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(SPI_NAND_WP_GROUP, SPI_NAND_WP_PIN, GPIO_PIN_SET);
    	HAL_GPIO_WritePin(SPI_NAND_HOLD_GROUP, SPI_NAND_HOLD_PIN, GPIO_PIN_SET);
    }

    return 0;
}

static int w25n01gv_spi_soft_io_rw(enum w25n01gv_soft_spi_io_line io_line, w25n01gv_uint8_t *rw_bit)
{
    switch (io_line) {
        case (enum w25n01gv_soft_spi_io_line)cs:
            if (*rw_bit & 0x02)
                gpio_bit_write(SPI_NAND_CS_GROUP, SPI_NAND_CS_PIN, *rw_bit & 0x01); /* write */
            else
                *rw_bit = gpio_output_bit_get(SPI_NAND_CS_GROUP, SPI_NAND_CS_PIN);
            break;

        case (enum w25n01gv_soft_spi_io_line)io0:
            if (*rw_bit & 0x02)
                gpio_bit_write(SPI_NAND_MOSI_GROUP, SPI_NAND_MOSI_PIN, *rw_bit & 0x01); /* write */
            else
                *rw_bit = gpio_input_bit_get(SPI_NAND_MOSI_GROUP, SPI_NAND_MOSI_PIN);
            break;

        case (enum w25n01gv_soft_spi_io_line)io1:
            if (*rw_bit & 0x02)
                gpio_bit_write(SPI_NAND_MISO_GROUP, SPI_NAND_MISO_PIN, *rw_bit & 0x01); /* write */
            else
                *rw_bit = gpio_input_bit_get(SPI_NAND_MISO_GROUP, SPI_NAND_MISO_PIN);
            break;

        case (enum w25n01gv_soft_spi_io_line)io2:
            if (*rw_bit & 0x02)
                gpio_bit_write(SPI_NAND_WP_GROUP, SPI_NAND_WP_PIN, *rw_bit & 0x01); /* write */
            else
                *rw_bit = gpio_input_bit_get(SPI_NAND_WP_GROUP, SPI_NAND_WP_PIN);
            break;

        case (enum w25n01gv_soft_spi_io_line)io3:
            if (*rw_bit & 0x02)
                gpio_bit_write(SPI_NAND_HOLD_GROUP, SPI_NAND_HOLD_PIN, *rw_bit & 0x01); /* write */
            else
                *rw_bit = gpio_input_bit_get(SPI_NAND_HOLD_GROUP, SPI_NAND_HOLD_PIN); /* read */
            break;

        default:
            return W25N01GVZEIG_PARAMETER_MISMATCH;
            break;
    }

    return W25N01GVZEIG_SUCCESS;
}

/// @brief
/// @param tx_byte
/// @param normal_dual_quad 0 normal; 1 dual; 2 quad
/// @return
static int w25n01gv_soft_spi_tx_byte(w25n01gv_uint8_t tx_byte, w25n01gv_uint8_t normal_dual_quad)
{
    w25n01gv_uint8_t tx_byte_wrap;
    HAL_GPIO_WritePin(SPI_NAND_CLK_GROUP, SPI_NAND_CLK_PIN, GPIO_PIN_RESET);

    /* MSB */
    switch (normal_dual_quad) {
        case 0:
            for (int i = 0; i < 8; i++) {
                tx_byte_wrap = SOFT_SPI_RW_BIT(1, ((tx_byte & 0x80) >> 7));
                w25n01gv_spi_soft_io_rw(io0, &tx_byte_wrap);
                tx_byte >>= 1;
            }
            break;

        case 1:
            for (int i = 0; i < 4; i++) {
                tx_byte_wrap = SOFT_SPI_RW_BIT(1, ((tx_byte & 0x80) >> 7));
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io1, &tx_byte_wrap);
                tx_byte >>= 1;
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io0, &tx_byte_wrap);
                tx_byte >>= 1;
            }
            break;

        case 2:
            for (int i = 0; i < 2; i++) {
                tx_byte_wrap = SOFT_SPI_RW_BIT(1, ((tx_byte & 0x80) >> 7));
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io3, &tx_byte_wrap);
                tx_byte >>= 1;
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io2, &tx_byte_wrap);
                tx_byte >>= 1;
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io1, &tx_byte_wrap);
                tx_byte >>= 1;
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io0, &tx_byte_wrap);
                tx_byte >>= 1;
            }
            break;

        default:
            break;
    }

    rt_thread_delay(50);
    HAL_GPIO_WritePin(SPI_NAND_CLK_GROUP, SPI_NAND_CLK_PIN, GPIO_PIN_SET);

    return W25N01GVZEIG_SUCCESS;
}

/// @brief
/// @param rx_byte byte on io lines to be read
/// @param normal_dual_quad 0 normal; 1 dual; 2 quad
/// @return
static inline int w25n01gv_soft_spi_rx_byte(w25n01gv_uint8_t *rx_byte, w25n01gv_uint8_t normal_dual_quad)
{
    *rx_byte                     = 0;
    w25n01gv_uint8_t io_read_bit = SOFT_SPI_RW_BIT(0, 0);
    HAL_GPIO_WritePin(SPI_NAND_CLK_GROUP, SPI_NAND_CLK_PIN, GPIO_PIN_RESET);

    switch (normal_dual_quad) {
        case 0:
            /* spi rx need tx send? */
            for (int i = 0; i < 8; i++) {
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io1, &io_read_bit);
                *rx_byte |= (io_read_bit << (7 - i)); /* read MSB first */
            }
            break;

        case 1:
            for (int i = 0; i < 4; i++) {
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io1, &io_read_bit);
                *rx_byte |= (io_read_bit << ((3 - i) * 2 + 1)); /* read MSB first */
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io0, &io_read_bit);
                *rx_byte |= (io_read_bit << ((3 - i) * 2));
            }
            break;

        case 2:
            for (int i = 0; i < 2; i++) {
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io3, &io_read_bit);
                *rx_byte |= (io_read_bit << ((1 - i) * 4 + 3)); /* read MSB first */
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io2, &io_read_bit);
                *rx_byte |= (io_read_bit << ((1 - i) * 4 + 2));
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io1, &io_read_bit);
                *rx_byte |= (io_read_bit << ((1 - i) * 4 + 1));
                w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)io0, &io_read_bit);
                *rx_byte |= (io_read_bit << ((1 - i) * 4));
            }
            break;

        default:
            break;
    }
    rt_thread_delay(50);
    HAL_GPIO_WritePin(SPI_NAND_CLK_GROUP, SPI_NAND_CLK_PIN, GPIO_PIN_SET);

    return W25N01GVZEIG_SUCCESS;
}

static inline int w25n01gv_soft_spi_tx_order(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t tx_struct)
{
    /* step 1: instruction byte always sent on io0 (normal) */
    w25n01gv_soft_spi_tx_byte(tx_buf[0], 0);

    int data_start = 1;

    /* step 2: send page/column address(16 bit) if any */
    if (tx_struct & SPI_INSTRUCTION_NEXT_ADDR) {
        if (tx_struct & SPI_ADDRESS_ON_IO0) {
            w25n01gv_soft_spi_tx_byte(tx_buf[1], 0);
            w25n01gv_soft_spi_tx_byte(tx_buf[2], 0);
        } else if (tx_struct & SPI_ADDRESS_ON_IO01) {
            w25n01gv_soft_spi_tx_byte(tx_buf[1], 1);
            w25n01gv_soft_spi_tx_byte(tx_buf[2], 1);
        } else if (tx_struct & SPI_ADDRESS_ON_IO0123) {
            w25n01gv_soft_spi_tx_byte(tx_buf[1], 2);
            w25n01gv_soft_spi_tx_byte(tx_buf[2], 2);
        }
        data_start = 3;
    }

    /* step 3: send data(dummy, user data, status register address) if any */
    if (tx_struct & SPI_NEXT_DATA) {
        if (tx_struct & SPI_NEXT_DATA_ON_IO0) {
            for (int i = data_start; i < tx_cnt; i++) {
                w25n01gv_soft_spi_tx_byte(tx_buf[i], 0);
            }
        } else if (tx_struct & SPI_NEXT_DATA_ON_IO01) {
            for (int i = data_start; i < tx_cnt; i++) {
                w25n01gv_soft_spi_tx_byte(tx_buf[i], 1);
            }
        } else if (tx_struct & SPI_NEXT_DATA_ON_IO0123) {
            for (int i = data_start; i < tx_cnt; i++) {
                w25n01gv_soft_spi_tx_byte(tx_buf[i], 2);
            }
        }
    }

    return W25N01GVZEIG_SUCCESS;
}

/// @brief
/// @param rx_buf
/// @param rx_cnt
/// @param io_struct
/// @return
static inline int w25n01gv_soft_spi_rx_order(w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, w25n01gv_uint8_t io_struct)
{
    /* step 1: receive data if any */
    if (io_struct & SPI_NEXT_DATA) {
        if (io_struct & SPI_NEXT_DATA_ON_IO0) {
            for (int i = 0; i < rx_cnt; i++) {
                w25n01gv_soft_spi_rx_byte(rx_buf + i, 0);
            }
        } else if (io_struct & SPI_NEXT_DATA_ON_IO01) {
            for (int i = 0; i < rx_cnt; i++) {
                w25n01gv_soft_spi_rx_byte(rx_buf + i, 1);
            }
        } else if (io_struct & SPI_NEXT_DATA_ON_IO0123) {
            for (int i = 0; i < rx_cnt; i++) {
                w25n01gv_soft_spi_rx_byte(rx_buf + i, 2);
            }
        }
    }

    return W25N01GVZEIG_SUCCESS;
}

/// @brief
/// @param tx_buf
/// @param tx_cnt
/// @param io_struct
/// @return
static inline int w25n01gv_soft_spi_tx(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t io_struct)
{
    /* cs low */
    w25n01gv_uint8_t tx_byte_wrap;
    tx_byte_wrap = SOFT_SPI_RW_BIT(1, 0);
    w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)cs, &tx_byte_wrap);
    board_w25n01gv_delay_us(SOFT_SPI_CS_LOW_DELAY_US);

    w25n01gv_soft_spi_tx_order(tx_buf, tx_cnt, io_struct);

    /* cs high */
    tx_byte_wrap = SOFT_SPI_RW_BIT(1, 1);
    w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)cs, &tx_byte_wrap);

    return W25N01GVZEIG_SUCCESS;
}

/// @brief
/// @param tx_byte
/// @param tx_cnt
/// @param rx_byte
/// @param rx_cnt
/// @param tx_io_struct
/// @return
static inline int w25n01gv_soft_spi_tx_rx(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, w25n01gv_uint8_t tx_io_struct, w25n01gv_uint8_t rx_io_struct)
{
    /* cs low */
    w25n01gv_uint8_t byte_wrap = SOFT_SPI_RW_BIT(1, 0);
    w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)cs, &byte_wrap);
    board_w25n01gv_delay_us(SOFT_SPI_CS_LOW_DELAY_US);

    w25n01gv_soft_spi_tx_order(tx_buf, tx_cnt, tx_io_struct);
    w25n01gv_soft_spi_rx_order(rx_buf, rx_cnt, rx_io_struct);

    /* cs high */
    byte_wrap = SOFT_SPI_RW_BIT(1, 1);
    w25n01gv_spi_soft_io_rw((enum w25n01gv_soft_spi_io_line)cs, &byte_wrap);

    return W25N01GVZEIG_SUCCESS;
}

#endif
/* ---------------------------------soft spi driver end-------------------------------------------- */

/* ---------------------------------hard spi driver begin-------------------------------------------- */

/// @brief start/stop spi transmit
/// @param mode
/// @return
int nand_spi_cs_ctrl(w25n01gv_uint8_t mode)
{
    if (mode == ENABLE)
    	HAL_GPIO_WritePin(SPI_NAND_CS_GROUP, SPI_NAND_CS_PIN, GPIO_PIN_RESET);
    else
    	HAL_GPIO_WritePin(SPI_NAND_CS_GROUP, SPI_NAND_CS_PIN, GPIO_PIN_SET);

    return 0;
}

static int nand_spi_pin_init(void)
{
    /* spi pin */
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    //rcu_periph_clock_enable(SPI_NAND_CS_RCU | SPI_NAND_MISO_RCU | SPI_NAND_MOSI_RCU | SPI_NAND_CLK_RCU | SPI_NAND_WP_RCU | SPI_NAND_HOLD_RCU);
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = SPI_NAND_CS_PIN|SPI_NAND_WP_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPI_NAND_CLK_PIN|SPI_NAND_MISO_PIN|SPI_NAND_MOSI_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
/*
    gpio_init(SPI_NAND_CS_GROUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_NAND_CS_PIN);
    gpio_init(SPI_NAND_CLK_GROUP, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, SPI_NAND_CLK_PIN);
    gpio_init(SPI_NAND_MISO_GROUP, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, SPI_NAND_MISO_PIN);
    gpio_init(SPI_NAND_MOSI_GROUP, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, SPI_NAND_MOSI_PIN);
    gpio_init(SPI_NAND_WP_GROUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_NAND_WP_PIN);
    gpio_init(SPI_NAND_HOLD_GROUP, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, SPI_NAND_HOLD_PIN);
*/
/*
    gpio_bit_write(SPI_NAND_CS_GROUP, SPI_NAND_CS_PIN, SET);
    gpio_bit_write(SPI_NAND_CLK_GROUP, SPI_NAND_CLK_PIN, SET);
    gpio_bit_write(SPI_NAND_MISO_GROUP, SPI_NAND_MISO_PIN, SET);
    gpio_bit_write(SPI_NAND_MOSI_GROUP, SPI_NAND_MOSI_PIN, SET);
    gpio_bit_write(SPI_NAND_WP_GROUP, SPI_NAND_WP_PIN, SET);
    gpio_bit_write(SPI_NAND_HOLD_GROUP, SPI_NAND_HOLD_PIN, SET);
*/
    HAL_GPIO_WritePin(GPIOB,SPI_NAND_CS_PIN|SPI_NAND_CLK_PIN|SPI_NAND_MISO_PIN|SPI_NAND_MOSI_PIN,GPIO_PIN_SET);

    return 0;
}

static int nand_spi_init(void)
{
    //rcu_periph_clock_enable(SPI_NAND_SPI_RCU);
    __HAL_RCC_SPI2_CLK_ENABLE();
 /*
    spi_parameter_struct spi_init_struct;
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE; // w25n01gv support spi mode 0, 3
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_4;
    spi_init(SPI_NAND_SPI, &spi_init_struct);
 */

    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 7;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
       Error_Handler();
    }

    __HAL_SPI_ENABLE(&hspi2);

    return 0;
}

int w25n01gv_spi_hard_init()
{
    nand_spi_pin_init();
    nand_spi_init();

    return 0;
}

/*!
    \brief      send a byte through the SPI interface and return the byte received from the SPI bus
    \param[in]  byte: byte to send
    \param[out] none
    \retval     the value of the received byte
*/
static uint8_t spi_send_byte(uint8_t byte)
{
	uint8_t Rxdata;

    /* send byte through the SPI peripheral */
    HAL_SPI_TransmitReceive(&hspi2, &byte,&Rxdata,1,1000);

    /* return the byte read from the SPI bus */
    return (Rxdata);
}

static uint8_t spi_read_byte(void)
{
    return (spi_send_byte(W25N01GV_DUMMY));
}

static int spi_tx_with_format(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, enum w25n01gv_spi_mode spi_tx_mode)
{
	uint16_t i;
	if (spi_tx_mode == normal_4line) {
        if (w25n01gv_current_spi_mode == normal_4line) {
            ; /* nothing */
        }

    } else if (spi_tx_mode == dual_tx) {
        if (w25n01gv_current_spi_mode == dual_tx) {
            ; /* nothing */
        }
    } else if (spi_tx_mode == quad_tx) {
        if (w25n01gv_current_spi_mode == quad_tx) {
            ; /* nothing */
        }
    }

    /* use hard spi driver */
    for ( i = 0; i < tx_cnt; i++)
        spi_send_byte(tx_buf[i]);

    return 0;
}

static int spi_rx_with_format(w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, enum w25n01gv_spi_mode spi_rx_mode)
{
	uint16_t i;
	if (spi_rx_mode == normal_4line) {
        if (w25n01gv_current_spi_mode == normal_4line) {
            ; /* nothing */
        }

    } else if (spi_rx_mode == dual_rx) {
        if (w25n01gv_current_spi_mode == dual_rx) {
            ; /* nothing */
        }
    } else if (spi_rx_mode == quad_rx) {
        if (w25n01gv_current_spi_mode == quad_rx) {
            ; /* nothing */
        }
            w25n01gv_current_spi_mode = quad_rx;
            return 0;
        }


    /* use hard spi driver */
    for (i = 0; i < rx_cnt; i++)
        rx_buf[i] = spi_read_byte();

    return 0;
}

/* ---------------------------------hard spi driver end-------------------------------------------- */

int board_w25n01gv_spi_tx_interface(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t tx_io_struct)
{
    nand_spi_cs_ctrl(ENABLE);

    /* send instruction normal */
    spi_tx_with_format(tx_buf, 1, normal_4line);
    w25n01gv_uint8_t data_start_pos = 1;
    if (tx_io_struct & SPI_INSTRUCTION_NEXT_ADDR) {
        data_start_pos = 3;
    }

    /* send address */
    if (tx_io_struct & SPI_ADDRESS_ON_IO0) {
        spi_tx_with_format(tx_buf + 1, 2, normal_4line);
    } else if (tx_io_struct & SPI_ADDRESS_ON_IO01) { /* send address with dual tx */
        spi_tx_with_format(tx_buf + 1, 2, dual_tx);
    } else if (tx_io_struct & SPI_ADDRESS_ON_IO0123) { /* send address with quad tx */
        spi_tx_with_format(tx_buf + 1, 2, quad_tx);
    }

    /* send data */
    if (tx_io_struct & SPI_NEXT_DATA_ON_IO0) { /* return normal */
        spi_tx_with_format(tx_buf + data_start_pos, tx_cnt - data_start_pos, normal_4line);
    } else if (tx_io_struct & SPI_NEXT_DATA_ON_IO01) {
        spi_tx_with_format(tx_buf + data_start_pos, tx_cnt - data_start_pos, dual_tx);
    } else if (tx_io_struct & SPI_NEXT_DATA_ON_IO0123) {
        spi_tx_with_format(tx_buf + data_start_pos, tx_cnt - data_start_pos, quad_tx);
    }

    nand_spi_cs_ctrl(DISABLE);

    return 0;
}

int board_w25n01gv_spi_tx_rx_interface(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, w25n01gv_uint8_t tx_io_struct, w25n01gv_uint8_t rx_io_struct)
{
    nand_spi_cs_ctrl(ENABLE);

    /* send instruction normal */
    spi_tx_with_format(tx_buf, 1, normal_4line);
    w25n01gv_uint8_t data_start_pos = 1;
    if (tx_io_struct & SPI_INSTRUCTION_NEXT_ADDR) {
        data_start_pos = 3;
    }

    /* send address */
    if (tx_io_struct & SPI_ADDRESS_ON_IO0) {
        spi_tx_with_format(tx_buf + 1, 2, normal_4line);
    } else if (tx_io_struct & SPI_ADDRESS_ON_IO01) { /* send address with dual tx */
        spi_tx_with_format(tx_buf + 1, 2, dual_tx);
    } else if (tx_io_struct & SPI_ADDRESS_ON_IO0123) { /* send address with quad tx */
        spi_tx_with_format(tx_buf + 1, 2, quad_tx);
    }

    /* send data */
    if (tx_io_struct & SPI_NEXT_DATA_ON_IO0) { /* return normal */
        spi_tx_with_format(tx_buf + data_start_pos, tx_cnt - data_start_pos, normal_4line);
    } else if (tx_io_struct & SPI_NEXT_DATA_ON_IO01) {
        spi_tx_with_format(tx_buf + data_start_pos, tx_cnt - data_start_pos, dual_tx);
    } else if (tx_io_struct & SPI_NEXT_DATA_ON_IO0123) {
        spi_tx_with_format(tx_buf + data_start_pos, tx_cnt - data_start_pos, quad_tx);
    }

    /* receive data */
    if (rx_io_struct & SPI_NEXT_DATA_ON_IO0) { /* return normal */
        spi_rx_with_format(rx_buf, rx_cnt, normal_4line);
    } else if (rx_io_struct & SPI_NEXT_DATA_ON_IO01) {
        spi_rx_with_format(rx_buf, rx_cnt, dual_rx);
    } else if (rx_io_struct & SPI_NEXT_DATA_ON_IO0123) {
        spi_rx_with_format(rx_buf, rx_cnt, quad_rx);
    }

    nand_spi_cs_ctrl(DISABLE);

    return 0;
}

int board_w25n01gv_delay_us(w25n01gv_uint16_t delay_us)
{
    w25n01gv_uint16_t tick = 0x1ff;
    while (tick > 0) {
        tick--;
    }

    // rt_thread_delay(delay_us);

    return 0;
}

void *board_w25n01gv_malloc(w25n01gv_uint32_t size)
{

    return malloc(size);
}

void board_w25n01gv_free(void *pos)
{

    free(pos);
}

void board_w25n01gv_memcpy(void *dest, const void *src, w25n01gv_uint32_t n)
{
    memcpy(dest, src, n);
}

int board_w25n01gv_init()
{
    w25n01gvzeig_driver w25n01gvzeig_driver;
    w25n01gv_spi_hard_init();

    w25n01gvzeig_driver.spi_tx    = board_w25n01gv_spi_tx_interface;
    w25n01gvzeig_driver.spi_tx_rx = board_w25n01gv_spi_tx_rx_interface;
    w25n01gvzeig_driver.delay_us  = board_w25n01gv_delay_us;
    w25n01gvzeig_driver.malloc    = board_w25n01gv_malloc;
    w25n01gvzeig_driver.free      = board_w25n01gv_free;
    w25n01gvzeig_driver.memcpy    = board_w25n01gv_memcpy;

    return w25n01gv_init(&w25n01gvzeig_driver);
}
