/*
 * w25n01gvzeig.c
 *
 *  Created on: Sep 9, 2025
 *      Author: Admin
 */

#include "w25n01gvzeig.h"



static w25n01gvzeig_driver st_w25n01gvzeig_driver;

/// @brief WAIT W25N01GV IDLE
static void w25n01gv_wait_busy_done(w25n01gv_uint16_t wait_delay_us)
{
    w25n01gvzeig_SR3 SR3;
    while (1) {
        w25n01gv_read_status(W25N01GVZEIG_SR3_ADDR, (w25n01gv_uint8_t *)&SR3);
        if (SR3.busy == 0)
            break;

        if (st_w25n01gvzeig_driver.delay_us)
            st_w25n01gvzeig_driver.delay_us(wait_delay_us);
    }
}

static void w25n01gvzeig_memcpy(void *dst, const void *src, w25n01gv_uint32_t n)
{
    /* if register memcpy function, use it */
    if (st_w25n01gvzeig_driver.memcpy)
        st_w25n01gvzeig_driver.memcpy(dst, src, n);
    else {
        w25n01gv_uint8_t *dst_u8 = (w25n01gv_uint8_t *)dst;
        w25n01gv_uint8_t *src_u8 = (w25n01gv_uint8_t *)src;

        for (int i = 0; i < n; i++) {
            dst_u8[i] = src_u8[i];
        }
    }
}

/// @brief
/// @param tx_buf
/// @param tx_cnt
/// @param tx_io_struct
/// @return
static int w25n01gv_tx(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t tx_io_struct)
{
    if (st_w25n01gvzeig_driver.spi_tx)
        return st_w25n01gvzeig_driver.spi_tx(tx_buf, tx_cnt, tx_io_struct);

    return W25N01GVZEIG_NULL_POINTER;
}

/// @brief
/// @param tx_buf
/// @param tx_cnt
/// @param rx_buf
/// @param rx_cnt
/// @param tx_io_struct spi tx signal time sequence
/// @param rx_io_struct spi rx signal time sequence
/// @return
static int w25n01gv_tx_rx(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, w25n01gv_uint8_t tx_io_struct, w25n01gv_uint8_t rx_io_struct)
{
    if (st_w25n01gvzeig_driver.spi_tx_rx)
        return st_w25n01gvzeig_driver.spi_tx_rx(tx_buf, tx_cnt, rx_buf, rx_cnt, tx_io_struct, rx_io_struct);

    return W25N01GVZEIG_NULL_POINTER;
}

/// @brief normal spi and quad spi load data to page buffer
/// @param column_addr
/// @param data
/// @param load_cnt
/// @param random_load
/// @param quad 0 normal spi, 1 quad spi
/// @return
static int w25n01gv_program_data_load_normal_quad(w25n01gv_uint16_t column_addr, w25n01gv_uint8_t *data, w25n01gv_uint16_t load_cnt, w25n01gv_uint8_t random_load, w25n01gv_uint8_t quad)
{
    if (load_cnt == 0) return W25N01GVZEIG_SUCCESS;
    if (load_cnt > W25N01GV_PAGE_SIZE) return W25N01GVZEIG_LOAD_OVER_PAGE;

    if (quad) {
        w25n01gvzeig_SR1 SR1;
        int rslt = read_SR1(&SR1);
        if (rslt != W25N01GVZEIG_SUCCESS)
            return rslt;

        if (SR1.write_protect_enable == 1) /* if WP-E bit in SR1 is set, means use write protect pins, which makes quad operation not avaliable */
            return W25N01GVZEIG_QUAD_DISABLE;
    }

    w25n01gv_uint8_t tx_io_struct = SPI_INSTRUCTION_NEXT_ADDR | SPI_ADDRESS_ON_IO0;

    w25n01gv_write_enable();

    /* if register malloc-free function, use it */
    if (st_w25n01gvzeig_driver.malloc && st_w25n01gvzeig_driver.free) {
        w25n01gv_uint8_t *tx_buf = st_w25n01gvzeig_driver.malloc(load_cnt + sizeof(column_addr) + 1); /* instruction size is 1 byte */
        if (tx_buf) {                                                                                 /* if malloc memory success */
            tx_io_struct |= SPI_NEXT_DATA;
            if (quad) {
                tx_io_struct |= SPI_NEXT_DATA_ON_IO0123;
                tx_buf[0] = (random_load == 0) ? W25N01GV_QUAD_PROGRAM_DATA_LOAD : W25N01GV_RANDOM_QUAD_PROGRAM_DATA_LOAD;
            } else {
                tx_io_struct |= SPI_NEXT_DATA_ON_IO0;
                tx_buf[0] = (random_load == 0) ? W25N01GV_PROGRAM_DATA_LOAD : W25N01GV_RANDOM_PROGRAM_DATA_LOAD;
            }

            tx_buf[1] = column_addr >> 8;
            tx_buf[2] = column_addr;

            w25n01gvzeig_memcpy(tx_buf + 3, data, load_cnt);

            w25n01gv_tx(tx_buf, load_cnt + sizeof(column_addr) + 1, tx_io_struct);

            st_w25n01gvzeig_driver.free(tx_buf);

            return W25N01GVZEIG_SUCCESS;
        }
        /* if malloc memory failed, use random data load multi-times */
    }

    /* if no malloc-free function registered, use random data load multi-times,
        load max 50 bytes once!
     */
    w25n01gv_uint16_t column_addr_everytime = column_addr;
    w25n01gv_uint8_t tx_buf[W25N01GV_MAX_RANDOM_LOAD_BYTES];
    w25n01gv_uint16_t already_load_cnt = 0;

    tx_io_struct |= SPI_NEXT_DATA;
    if (quad) {
        tx_io_struct |= SPI_NEXT_DATA_ON_IO0123;
        tx_buf[0] = W25N01GV_RANDOM_QUAD_PROGRAM_DATA_LOAD;
    } else {
        tx_io_struct |= SPI_NEXT_DATA_ON_IO0;
        tx_buf[0] = W25N01GV_RANDOM_PROGRAM_DATA_LOAD;
    }

    tx_buf[1] = column_addr_everytime >> 8;
    tx_buf[2] = column_addr_everytime;
    while (load_cnt > 0) {
        /* calculate load bytes this time */
        w25n01gv_uint8_t load_cnt_this_time = (load_cnt > (W25N01GV_MAX_RANDOM_LOAD_BYTES - 3)) ? (W25N01GV_MAX_RANDOM_LOAD_BYTES - 3) : load_cnt;

        w25n01gvzeig_memcpy(tx_buf + 3, data + already_load_cnt, load_cnt_this_time);

        already_load_cnt += load_cnt_this_time; /* update load count */
        load_cnt -= load_cnt_this_time;         /* update left bytes to load, if meet 0 means all bytes loaded */

        w25n01gv_tx(tx_buf, load_cnt_this_time + 3, tx_io_struct); /* load... */
    }

    return W25N01GVZEIG_SUCCESS;
}

/// @brief before fast read, page read command should be sent, wait internal page data ready,
//  then issue fast read command to launch fast read
/// @param column_address
/// @param data_buf
/// @param read_cnt
/// @param fast_read_mode 0 normal, 1 normal with 4byte, 2,...
/// @return
static int w25n01gv_fast_read_option(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt, enum w25n01gvzeig_fast_read_mode fast_read_mode)
{
    w25n01gv_wait_busy_done(W25N01GV_PAGE_READ_US);

    w25n01gvzeig_SR1 SR1;
    w25n01gvzeig_SR2 SR2;

    w25n01gv_uint8_t tx_buf[6];
    w25n01gv_uint8_t fast_read_with_4byte = 0;

    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA;

    int rslt;
    switch (fast_read_mode) {
        case (enum w25n01gvzeig_fast_read_mode)normal:
            rx_io_struct |= SPI_NEXT_DATA_ON_IO0;
            tx_buf[0] = W25N01GV_FAST_READ;
            break;

        case (enum w25n01gvzeig_fast_read_mode)normal_with_4byte:
            rx_io_struct |= SPI_NEXT_DATA_ON_IO0;
            tx_buf[0]            = W25N01GV_FAST_READ_WITH_4BYTE_ADDRESS;
            fast_read_with_4byte = 1;
            break;

        case (enum w25n01gvzeig_fast_read_mode)dual:
            rx_io_struct |= SPI_NEXT_DATA_ON_IO01;
            tx_buf[0] = W25N01GV_FAST_READ_DUAL;
            break;

        case (enum w25n01gvzeig_fast_read_mode)dual_with_4byte:
            rx_io_struct |= SPI_NEXT_DATA_ON_IO01;
            tx_buf[0]            = W25N01GV_FAST_READ_DUAL_WITH_4BYTE_ADDRESS;
            fast_read_with_4byte = 1;
            break;

        case (enum w25n01gvzeig_fast_read_mode)quad:
            rx_io_struct |= SPI_NEXT_DATA_ON_IO0123;
            rslt = read_SR1(&SR1);
            if (rslt != W25N01GVZEIG_SUCCESS) return rslt;
            if (SR1.write_protect_enable == 1) /* if WP-E set to 1, means use write protect pin, hold pin, which disable quad spi function */
                return W25N01GVZEIG_QUAD_DISABLE;

            tx_buf[0] = W25N01GV_FAST_READ_QUAD;
            break;

        case (enum w25n01gvzeig_fast_read_mode)quad_with_4byte:
            rx_io_struct |= SPI_NEXT_DATA_ON_IO0123;
            rslt = read_SR1(&SR1);
            if (rslt != W25N01GVZEIG_SUCCESS) return rslt;
            if (SR1.write_protect_enable == 1)
                return W25N01GVZEIG_QUAD_DISABLE;

            tx_buf[0]            = W25N01GV_FAST_READ_QUAD_WITH_4BYTE_ADDRESS;
            fast_read_with_4byte = 1;
            break;

        default:
            return W25N01GVZEIG_PARAMETER_MISMATCH;
            break;
    }

    rslt = read_SR2(&SR2);
    if (rslt != W25N01GVZEIG_SUCCESS) return rslt;

    if (SR2.read_buf_continue_mode == 0) {
        tx_buf[1] = W25N01GV_DUMMY;
        tx_buf[2] = W25N01GV_DUMMY;
        tx_buf[3] = W25N01GV_DUMMY;
        tx_buf[4] = W25N01GV_DUMMY;
        if (fast_read_with_4byte) {
            tx_buf[5] = W25N01GV_DUMMY;
            w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);
        } else {
            w25n01gv_tx_rx(tx_buf, sizeof(tx_buf) - 1, data_buf, read_cnt, tx_io_struct, rx_io_struct);
        }

    } else {
        rx_io_struct |= SPI_INSTRUCTION_NEXT_ADDR;
        rx_io_struct |= SPI_ADDRESS_ON_IO0;

        tx_buf[1] = column_address >> 8;
        tx_buf[2] = column_address;
        tx_buf[3] = W25N01GV_DUMMY;
        if (fast_read_with_4byte) {
            tx_buf[4] = W25N01GV_DUMMY;
            tx_buf[5] = W25N01GV_DUMMY;
            w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);
        } else {
            w25n01gv_tx_rx(tx_buf, sizeof(tx_buf) - 2, data_buf, read_cnt, tx_io_struct, rx_io_struct);
        }
    }

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_reset()
{
    w25n01gv_wait_busy_done(W25N01GV_BLOCK_ERASE_US);

    w25n01gv_uint8_t data         = W25N01GV_DEVICE_RESET;
    w25n01gv_uint8_t tx_io_struct = 0; /* only instruction */

    return w25n01gv_tx(&data, 1, tx_io_struct);
}

int w25n01gv_jedec_id(w25n01gv_uint8_t *manufacture_id, w25n01gv_uint8_t *jedec_id)
{
    /* we assume spi has configures MSB first */
    w25n01gv_uint8_t tx_buf[2];
    tx_buf[0] = W25N01GV_JEDEC_ID;
    tx_buf[1] = W25N01GV_DUMMY;
    w25n01gv_uint8_t rx_buf[3];

    /* instruction + dummy(data on IO0) + rx data */
    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;

    int rslt = w25n01gv_tx_rx(tx_buf, 2, rx_buf, 3, tx_io_struct, rx_io_struct);
    if (rslt != W25N01GVZEIG_SUCCESS)
        return rslt;

    *manufacture_id = rx_buf[0]; /* receive manufacture id first */

    jedec_id[0] = rx_buf[1]; /* aa */
    jedec_id[1] = rx_buf[2]; /* 21 */

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_read_status(w25n01gv_uint8_t reg_addr, w25n01gv_uint8_t *status)
{
    if (reg_addr != W25N01GVZEIG_SR1_ADDR && reg_addr != W25N01GVZEIG_SR2_ADDR && reg_addr != W25N01GVZEIG_SR3_ADDR)
        return W25N01GVZEIG_SR_ADDR_MISMATCH;

    w25n01gv_uint8_t buf[2];
    buf[0] = W25N01GV_READ_STATUS;
    buf[1] = reg_addr;

    /* instruction + status register address(data on IO0) + rx data */
    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;

    return w25n01gv_tx_rx(buf, 2, status, 1, tx_io_struct, rx_io_struct);
}

int w25n01gv_write_status(w25n01gv_uint8_t reg_addr, w25n01gv_uint8_t item, w25n01gv_uint8_t value)
{
    int rslt                 = W25N01GVZEIG_SUCCESS;
    w25n01gv_uint8_t reg_val = 0;

    switch (reg_addr) {

        case W25N01GVZEIG_SR1_ADDR:
            rslt = w25n01gv_read_status(W25N01GVZEIG_SR1_ADDR, &reg_val);
            if (rslt != W25N01GVZEIG_SUCCESS)
                return rslt;
            w25n01gvzeig_SR1 *SR1_current = (w25n01gvzeig_SR1 *)&reg_val;
            w25n01gvzeig_SR1 *SR1_set     = (w25n01gvzeig_SR1 *)&value;

            switch (item) {
                case W25NGVZEIG_SR1_BIT_WR_SRP:
                    SR1_current->status_register_protect_0 = SR1_set->status_register_protect_0;
                    SR1_current->status_register_protect_1 = SR1_set->status_register_protect_1;
                    break;

                case W25NGVZEIG_SR1_BIT_WR_WPE:
                    SR1_current->write_protect_enable = SR1_set->write_protect_enable;
                    break;

                case W25NGVZEIG_SR1_BIT_WR_TB:
                    SR1_current->top_bottom_protect = SR1_set->top_bottom_protect;
                    break;

                case W25NGVZEIG_SR1_BIT_WR_BP:
                    SR1_current->block_protect_0 = SR1_set->block_protect_0;
                    SR1_current->block_protect_1 = SR1_set->block_protect_1;
                    SR1_current->block_protect_2 = SR1_set->block_protect_2;
                    SR1_current->block_protect_3 = SR1_set->block_protect_3;
                    break;

                default:
                    return W25N01GVZEIG_SR_BIT_READONLY;
                    break;
            }
            break;

        case W25N01GVZEIG_SR2_ADDR:
            rslt = w25n01gv_read_status(W25N01GVZEIG_SR2_ADDR, &reg_val);
            if (rslt != W25N01GVZEIG_SUCCESS)
                return rslt;
            w25n01gvzeig_SR2 *SR2_current = (w25n01gvzeig_SR2 *)&reg_val;
            w25n01gvzeig_SR2 *SR2_set     = (w25n01gvzeig_SR2 *)&value;

            switch (item) {
                case W25NGVZEIG_SR2_BIT_WR_OTP_L:
                    SR2_current->otp_lock = SR2_set->otp_lock;
                    break;

                case W25NGVZEIG_SR2_BIT_WR_OTP_E:
                    SR2_current->otp_enable = SR2_set->otp_enable;
                    break;

                case W25NGVZEIG_SR2_BIT_WR_SR1_L:
                    SR2_current->status_reg1_lock = SR2_set->status_reg1_lock;
                    break;

                case W25NGVZEIG_SR2_BIT_WR_ECC_E:
                    SR2_current->ecc_enable = SR2_set->ecc_enable;
                    break;

                case W25NGVZEIG_SR2_BIT_WR_BUF:
                    SR2_current->read_buf_continue_mode = SR2_set->read_buf_continue_mode;
                    break;

                default:
                    return W25N01GVZEIG_SR_BIT_READONLY;
                    break;
            }
            break;

        case W25N01GVZEIG_SR3_ADDR:
            return W25N01GVZEIG_SR_BIT_READONLY;
            break;

        default:
            return W25N01GVZEIG_SR_ADDR_MISMATCH;
            break;
    }

    /* use spi to write status register */
    w25n01gv_uint8_t buf[3];
    buf[0] = W25N01GV_WRITE_STATUS;
    buf[1] = reg_addr;
    buf[2] = reg_val;

    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;

    return w25n01gv_tx(buf, sizeof(buf), tx_io_struct);
}

int w25n01gv_write_enable()
{
    w25n01gv_uint8_t buf          = W25N01GV_WRITE_ENABLE;
    w25n01gv_uint8_t tx_io_struct = 0;

    return w25n01gv_tx(&buf, 1, tx_io_struct);
}

int w25n01gv_write_disable()
{
    w25n01gv_wait_busy_done(W25N01GV_BLOCK_ERASE_US); /* wait current job done */
    w25n01gv_uint8_t buf          = W25N01GV_WRITE_DISABLE;
    w25n01gv_uint8_t tx_io_struct = 0;

    return w25n01gv_tx(&buf, 1, tx_io_struct);
}

int w25n01gv_bad_block_management(w25n01gv_uint16_t lba, w25n01gv_uint16_t pba)
{
    w25n01gv_uint8_t buf[5];

    /* high byte first */
    buf[0] = W25N01GV_BBM;
    buf[1] = (lba >> 8);
    buf[2] = lba;
    buf[3] = (pba >> 8);
    buf[4] = pba;

    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;

    return w25n01gv_tx(buf, sizeof(buf), tx_io_struct);
}

int w25n01gv_read_bbm_lut(w25n01gv_uint16_t *lba, w25n01gv_uint16_t *pba)
{
    w25n01gv_wait_busy_done(W25N01GV_BBM_US); /* wait until idle or command will be ignored */

    w25n01gv_uint8_t tx_buf[2];
    tx_buf[0] = W25N01GV_READ_BBM_LUT;
    tx_buf[1] = W25N01GV_DUMMY;

    w25n01gv_uint8_t lba_pba_arr[W25N01GV_MAX_BBM_LUT * 2 * sizeof(w25n01gv_uint16_t)];

    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;
    int rslt                      = w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), lba_pba_arr, sizeof(lba_pba_arr), tx_io_struct, rx_io_struct);
    if (rslt != W25N01GVZEIG_SUCCESS)
        return rslt;

    for (int i = 0; i < W25N01GV_MAX_BBM_LUT; i++) {
        lba[i] = lba_pba_arr[4 * i] << 8 | lba_pba_arr[4 * i + 1]; /* receive high byte first */
        pba[i] = lba_pba_arr[4 * i + 2] << 8 | lba_pba_arr[4 * i + 3];
    }

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_last_ecc_fail_page_address(w25n01gv_uint16_t *page_addres)
{
    if (!page_addres) return W25N01GVZEIG_NULL_POINTER;

    w25n01gv_uint8_t tx_buf[2];
    tx_buf[0] = W25N01GV_LAST_ECC_FAIL_PAGE_ADDRESS;
    tx_buf[1] = W25N01GV_DUMMY;

    w25n01gv_uint8_t rx_buf[2];
    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;
    int rslt                      = w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), rx_buf, sizeof(rx_buf), tx_io_struct, rx_io_struct);
    if (rslt != W25N01GVZEIG_SUCCESS)
        return rslt;

    *page_addres = rx_buf[0] << 8 | rx_buf[1];

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_block_erase(w25n01gv_uint16_t block_page_addr)
{
    w25n01gv_write_enable(); /* enable write beofre block erase */

    w25n01gv_uint8_t tx_buf[4];
    tx_buf[0] = W25N01GV_BLOCK_ERASE;
    tx_buf[1] = W25N01GV_DUMMY;
    tx_buf[2] = block_page_addr >> 8;
    tx_buf[3] = block_page_addr;

    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;

    return w25n01gv_tx(tx_buf, sizeof(tx_buf), tx_io_struct);
}

int w25n01gv_program_data_load(w25n01gv_uint16_t column_addr, w25n01gv_uint8_t *data, w25n01gv_uint16_t load_cnt, w25n01gv_uint8_t random_load)
{
    return w25n01gv_program_data_load_normal_quad(column_addr, data, load_cnt, random_load, 0);
}

int w25n01gv_quad_program_data_load(w25n01gv_uint16_t column_addr, w25n01gv_uint8_t *data, w25n01gv_uint16_t load_cnt, w25n01gv_uint8_t random_load)
{
    return w25n01gv_program_data_load_normal_quad(column_addr, data, load_cnt, random_load, 1);
}

int w25n01gv_program_execute(w25n01gv_uint16_t page_address)
{
    w25n01gv_uint8_t tx_buf[4];
    tx_buf[0] = W25N01GV_PROGRAM_EXECUTE;
    tx_buf[1] = W25N01GV_DUMMY;
    tx_buf[2] = page_address >> 8;
    tx_buf[3] = page_address;

    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;

    return w25n01gv_tx(tx_buf, sizeof(tx_buf), tx_io_struct);
}

int w25n01gv_paga_data_read(w25n01gv_uint16_t page_address)
{
    w25n01gv_uint8_t tx_buf[4];
    tx_buf[0] = W25N01GV_PAGE_DATA_READ;
    tx_buf[1] = W25N01GV_DUMMY;
    tx_buf[2] = page_address >> 8;
    tx_buf[3] = page_address;

    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;

    return w25n01gv_tx(tx_buf, sizeof(tx_buf), tx_io_struct);
}

int w25n01gv_read(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    w25n01gv_wait_busy_done(W25N01GV_PAGE_READ_US);

    w25n01gvzeig_SR2 SR2;
    int rslt = read_SR2(&SR2);
    if (rslt != W25N01GVZEIG_SUCCESS) return rslt;

    w25n01gv_uint8_t tx_buf[4];
    tx_buf[0] = W25N01GV_READ;
    if (SR2.read_buf_continue_mode == 0) {
        tx_buf[1] = W25N01GV_DUMMY;
        tx_buf[2] = W25N01GV_DUMMY;
        tx_buf[3] = W25N01GV_DUMMY;
    } else {
        tx_buf[1] = column_address >> 8;
        tx_buf[2] = column_address;
        tx_buf[3] = W25N01GV_DUMMY;
    }

    w25n01gv_uint8_t tx_io_struct = SPI_INSTRUCTION_NEXT_ADDR | SPI_ADDRESS_ON_IO0 | SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO0;

    w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_fast_read(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    return w25n01gv_fast_read_option(column_address, data_buf, read_cnt, (enum w25n01gvzeig_fast_read_mode)normal);
}

int w25n01gv_fast_read_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    return w25n01gv_fast_read_option(column_address, data_buf, read_cnt, (enum w25n01gvzeig_fast_read_mode)normal_with_4byte);
}

int w25n01gv_fast_read_dual_output(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    return w25n01gv_fast_read_option(column_address, data_buf, read_cnt, (enum w25n01gvzeig_fast_read_mode)dual);
}

int w25n01gv_fast_read_dual_output_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    return w25n01gv_fast_read_option(column_address, data_buf, read_cnt, (enum w25n01gvzeig_fast_read_mode)dual_with_4byte);
}

int w25n01gv_fast_read_quad_output(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    return w25n01gv_fast_read_option(column_address, data_buf, read_cnt, (enum w25n01gvzeig_fast_read_mode)quad);
}

int w25n01gv_fast_read_quad_output_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    return w25n01gv_fast_read_option(column_address, data_buf, read_cnt, (enum w25n01gvzeig_fast_read_mode)quad_with_4byte);
}

int w25n01gv_fast_read_dual_io(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    w25n01gv_wait_busy_done(W25N01GV_PAGE_READ_US);

    w25n01gv_uint8_t tx_buf[5];
    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO01;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO01;

    w25n01gvzeig_SR2 SR2;
    read_SR2(&SR2);
    tx_buf[0] = W25N01GV_FAST_READ_DUAL_IO;
    if (SR2.read_buf_continue_mode == 0) {
        tx_buf[1] = W25N01GV_DUMMY;
        tx_buf[2] = W25N01GV_DUMMY;
        tx_buf[3] = W25N01GV_DUMMY;
        tx_buf[4] = W25N01GV_DUMMY;
        w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);
    } else {
        tx_io_struct |= (SPI_INSTRUCTION_NEXT_ADDR | SPI_ADDRESS_ON_IO01);
        tx_buf[1] = column_address >> 8;
        tx_buf[2] = column_address;
        tx_buf[3] = W25N01GV_DUMMY;
        w25n01gv_tx_rx(tx_buf, sizeof(tx_buf) - 1, data_buf, read_cnt, tx_io_struct, rx_io_struct);
    }

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_fast_read_dual_io_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    w25n01gv_wait_busy_done(W25N01GV_PAGE_READ_US);

    w25n01gv_uint8_t tx_buf[6];
    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO01;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO01;

    w25n01gvzeig_SR2 SR2;
    read_SR2(&SR2);
    tx_buf[0] = W25N01GV_FAST_READ_DUAL_IO_WITH_4BYTE_ADDRESS;
    if (SR2.read_buf_continue_mode == 0) {
        tx_buf[1] = W25N01GV_DUMMY;
        tx_buf[2] = W25N01GV_DUMMY;
        tx_buf[3] = W25N01GV_DUMMY;
        tx_buf[4] = W25N01GV_DUMMY;
        tx_buf[5] = W25N01GV_DUMMY;
        w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);
    } else {
        tx_io_struct |= (SPI_INSTRUCTION_NEXT_ADDR | SPI_ADDRESS_ON_IO01);
        tx_buf[1] = column_address >> 8;
        tx_buf[2] = column_address;
        tx_buf[3] = W25N01GV_DUMMY;
        tx_buf[4] = W25N01GV_DUMMY;
        tx_buf[5] = W25N01GV_DUMMY;
        w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);
    }

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_fast_read_quad_io(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    w25n01gv_wait_busy_done(W25N01GV_PAGE_READ_US);

    w25n01gv_uint8_t tx_buf[7];
    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO01;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO01;

    w25n01gvzeig_SR2 SR2;
    read_SR2(&SR2);
    tx_buf[0] = W25N01GV_FAST_READ_QUAD_IO;
    if (SR2.read_buf_continue_mode == 0) {
        tx_buf[1] = W25N01GV_DUMMY;
        tx_buf[2] = W25N01GV_DUMMY;
        tx_buf[3] = W25N01GV_DUMMY;
        tx_buf[4] = W25N01GV_DUMMY;
        tx_buf[5] = W25N01GV_DUMMY;
        tx_buf[6] = W25N01GV_DUMMY;
        w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);
    } else {
        tx_io_struct |= (SPI_INSTRUCTION_NEXT_ADDR | SPI_ADDRESS_ON_IO0123);
        tx_buf[1] = column_address >> 8;
        tx_buf[2] = column_address;
        tx_buf[3] = W25N01GV_DUMMY;
        tx_buf[4] = W25N01GV_DUMMY;
        w25n01gv_tx_rx(tx_buf, sizeof(tx_buf) - 2, data_buf, read_cnt, tx_io_struct, rx_io_struct);
    }

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_fast_read_quad_io_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt)
{
    w25n01gv_wait_busy_done(W25N01GV_PAGE_READ_US);

    w25n01gv_uint8_t tx_buf[8];
    w25n01gv_uint8_t tx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO01;
    w25n01gv_uint8_t rx_io_struct = SPI_NEXT_DATA | SPI_NEXT_DATA_ON_IO01;

    w25n01gvzeig_SR2 SR2;
    read_SR2(&SR2);
    tx_buf[0] = W25N01GV_FAST_READ_QUAD_IO_WITH_4BYTE_ADDRESS;
    if (SR2.read_buf_continue_mode == 0) {
        tx_buf[1] = W25N01GV_DUMMY;
        tx_buf[2] = W25N01GV_DUMMY;
        tx_buf[3] = W25N01GV_DUMMY;
        tx_buf[4] = W25N01GV_DUMMY;
        tx_buf[5] = W25N01GV_DUMMY;
        tx_buf[6] = W25N01GV_DUMMY;
        tx_buf[7] = W25N01GV_DUMMY;
        w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);
    } else {
        tx_io_struct |= (SPI_INSTRUCTION_NEXT_ADDR | SPI_ADDRESS_ON_IO0123);
        tx_buf[1] = column_address >> 8;
        tx_buf[2] = column_address;
        tx_buf[3] = W25N01GV_DUMMY;
        tx_buf[4] = W25N01GV_DUMMY;
        tx_buf[5] = W25N01GV_DUMMY;
        tx_buf[6] = W25N01GV_DUMMY;
        tx_buf[7] = W25N01GV_DUMMY;
        w25n01gv_tx_rx(tx_buf, sizeof(tx_buf), data_buf, read_cnt, tx_io_struct, rx_io_struct);
    }

    return W25N01GVZEIG_SUCCESS;
}

int resolve_SR1_block_protection_bits_field(enum w2501gvzeig_memory_protection_field mpf, w25n01gvzeig_SR1 *SR1)
{
    if (!SR1) {
        return -1;
    }

    switch (mpf) {
        case (enum w2501gvzeig_memory_protection_field)None:
            SR1->block_protect_0 = 0;
            SR1->block_protect_1 = 0;
            SR1->block_protect_2 = 0;
            SR1->block_protect_3 = 0;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_ff80_ffff_256KB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_007f_256KB:
            SR1->block_protect_0 = 1;
            SR1->block_protect_1 = 0;
            SR1->block_protect_2 = 0;
            SR1->block_protect_3 = 0;

            SR1->top_bottom_protect = (mpf == cover_ff80_ffff_256KB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_ff00_ffff_512KB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_00ff_512KB:
            SR1->block_protect_0 = 0;
            SR1->block_protect_1 = 1;
            SR1->block_protect_2 = 0;
            SR1->block_protect_3 = 0;

            SR1->top_bottom_protect = (mpf == cover_ff00_ffff_512KB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_fe00_ffff_1MB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_01ff_1MB:
            SR1->block_protect_0 = 1;
            SR1->block_protect_1 = 1;
            SR1->block_protect_2 = 0;
            SR1->block_protect_3 = 0;

            SR1->top_bottom_protect = (mpf == cover_fe00_ffff_1MB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_fc00_ffff_2MB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_03ff_2MB:
            SR1->block_protect_0 = 0;
            SR1->block_protect_1 = 0;
            SR1->block_protect_2 = 1;
            SR1->block_protect_3 = 0;

            SR1->top_bottom_protect = (mpf == cover_fc00_ffff_2MB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_f800_ffff_4MB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_07ff_4MB:
            SR1->block_protect_0 = 1;
            SR1->block_protect_1 = 0;
            SR1->block_protect_2 = 1;
            SR1->block_protect_3 = 0;

            SR1->top_bottom_protect = (mpf == cover_f800_ffff_4MB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_f000_ffff_8MB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_0fff_8MB:
            SR1->block_protect_0 = 0;
            SR1->block_protect_1 = 1;
            SR1->block_protect_2 = 1;
            SR1->block_protect_3 = 0;

            SR1->top_bottom_protect = (mpf == cover_f000_ffff_8MB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_e000_ffff_16MB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_1fff_16MB:
            SR1->block_protect_0 = 1;
            SR1->block_protect_1 = 1;
            SR1->block_protect_2 = 1;
            SR1->block_protect_3 = 0;

            SR1->top_bottom_protect = (mpf == cover_e000_ffff_16MB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_c000_ffff_32MB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_3fff_32MB:
            SR1->block_protect_0 = 0;
            SR1->block_protect_1 = 0;
            SR1->block_protect_2 = 0;
            SR1->block_protect_3 = 1;

            SR1->top_bottom_protect = (mpf == cover_c000_ffff_32MB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)cover_8000_ffff_64MB:
        case (enum w2501gvzeig_memory_protection_field)cover_0000_7fff_64MB:
            SR1->block_protect_0 = 1;
            SR1->block_protect_1 = 0;
            SR1->block_protect_2 = 0;
            SR1->block_protect_3 = 1;

            SR1->top_bottom_protect = (mpf == cover_8000_ffff_64MB) ? 0 : 1;
            break;

        case (enum w2501gvzeig_memory_protection_field)All:
            SR1->block_protect_0    = 1;
            SR1->block_protect_1    = 1;
            SR1->block_protect_2    = 1;
            SR1->block_protect_3    = 1;
            SR1->top_bottom_protect = 1;
            break;
        default:
            return -2;
            break;
    }

    return W25N01GVZEIG_SUCCESS;
}

int read_SR1(w25n01gvzeig_SR1 *SR1)
{
    if (!SR1)
        return W25N01GVZEIG_NULL_POINTER;

    int rslt = w25n01gv_read_status(W25N01GVZEIG_SR1_ADDR, (w25n01gv_uint8_t *)SR1);
    if (rslt != W25N01GVZEIG_SUCCESS)
        return rslt;

    return W25N01GVZEIG_SUCCESS;
}

int read_SR2(w25n01gvzeig_SR2 *SR2)
{
    if (!SR2)
        return W25N01GVZEIG_NULL_POINTER;

    int rslt = w25n01gv_read_status(W25N01GVZEIG_SR2_ADDR, (w25n01gv_uint8_t *)SR2);
    if (rslt != W25N01GVZEIG_SUCCESS)
        return rslt;

    return W25N01GVZEIG_SUCCESS;
}

int read_SR3(w25n01gvzeig_SR3 *SR3)
{
    if (!SR3)
        return W25N01GVZEIG_NULL_POINTER;

    int rslt = w25n01gv_read_status(W25N01GVZEIG_SR3_ADDR, (w25n01gv_uint8_t *)SR3);
    if (rslt != W25N01GVZEIG_SUCCESS)
        return rslt;

    return W25N01GVZEIG_SUCCESS;
}

int w2501gv_busy(w25n01gv_uint8_t *busy)
{
    w25n01gvzeig_SR3 SR3;
    int rslt = read_SR3(&SR3);
    if (rslt != W25N01GVZEIG_SUCCESS)
        return rslt;

    *busy = SR3.busy;

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gvzeig_fetch_device_info(w25n01gvzeig_deivce_info *info)
{
    if (!info)
        return W25N01GVZEIG_NULL_POINTER;

    info->data_bytes_per_page  = W25N01GV_DATA_BYTES_PER_PAGE;
    info->spare_bytes_per_page = W25N01GV_SPARE_BYTES_PER_PAGE;
    info->sector_size          = W25N01GV_SECTOR_SIZE;
    info->pages_per_block      = W25N01GV_PAGES_PER_BLOCK;
    info->blocks_per_nand      = W25N01GV_BLOCKS_PER_NAND;

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_read_otp_page(w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, enum w25n01gv_otp_page which_otp_page)
{
    if (!rx_buf)
        return W25N01GVZEIG_NULL_POINTER;

    if ((w25n01gv_uint8_t)which_otp_page < (enum w25n01gv_otp_page)unique_id_page ||
        (w25n01gv_uint8_t)which_otp_page > (enum w25n01gv_otp_page)otp_page9) {
        return W25N01GVZEIG_PARAMETER_MISMATCH;
    }

    if (rx_cnt == 0)
        return W25N01GVZEIG_PARAMETER_MISMATCH;

    /* enter otp area */
    w25n01gvzeig_SR2 SR2_set;
    SR2_set.otp_enable = 1;
    w25n01gv_write_status(W25N01GVZEIG_SR2_ADDR, W25NGVZEIG_SR2_BIT_WR_OTP_E, (*(w25n01gv_uint8_t *)(&SR2_set)));

    /* make sure enter otp area */
    w25n01gv_read_status(W25N01GVZEIG_SR2_ADDR, (w25n01gv_uint8_t *)&SR2_set);
    if (SR2_set.otp_enable == 0) {
        if (st_w25n01gvzeig_driver.delay_us)
            st_w25n01gvzeig_driver.delay_us(W25N01GV_OTP_LOCK_US);
    }

    w25n01gv_uint16_t page_address = (w25n01gv_uint16_t)which_otp_page;
    w25n01gv_paga_data_read(page_address);

    w25n01gv_wait_busy_done(W25N01GV_PAGE_READ_US);

    w25n01gv_read(0, rx_buf, rx_cnt);

    /* leave otp area */
    SR2_set.otp_enable = 0;
    w25n01gv_write_status(W25N01GVZEIG_SR2_ADDR, W25NGVZEIG_SR2_BIT_WR_OTP_E, (*(w25n01gv_uint8_t *)(&SR2_set)));

    return W25N01GVZEIG_SUCCESS;
}

int w25n01gv_write_page(w25n01gv_uint16_t page_index, w25n01gv_uint16_t start_pos, w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt)
{
    /* fectch page address first */
    w25n01gv_uint16_t block_index      = page_index / W25N01GV_PAGES_PER_BLOCK;
    w25n01gv_uint16_t block_page_index = page_index % W25N01GV_PAGES_PER_BLOCK;
    w25n01gv_uint16_t page_address     = block_index << W25N01GV_PAGE_ADDR_LEN | block_page_index;

    /* load data */
    w25n01gv_program_data_load(start_pos, tx_buf, tx_cnt, 0);

    /* start program */
    return w25n01gv_program_execute(page_address);
}

int w25n01gv_read_page(w25n01gv_uint16_t page_index, w25n01gv_uint16_t start_pos, w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt)
{
    /* fectch page address first */
    w25n01gv_uint16_t block_index      = page_index / W25N01GV_PAGES_PER_BLOCK;
    w25n01gv_uint16_t block_page_index = page_index % W25N01GV_PAGES_PER_BLOCK;
    w25n01gv_uint16_t page_address     = block_index << W25N01GV_PAGE_ADDR_LEN | block_page_index;

    /* read page */
    w25n01gv_paga_data_read(page_address);

    return w25n01gv_read(start_pos, rx_buf, rx_cnt);
}

int w25n01gv_init(w25n01gvzeig_driver *driver)
{
    static w25n01gv_uint8_t init_done = 0;
    if (init_done)
        return W25N01GVZEIG_SUCCESS;

    if (!driver)
        return W25N01GVZEIG_NULL_POINTER;

    st_w25n01gvzeig_driver.spi_tx    = 0;
    st_w25n01gvzeig_driver.spi_tx_rx = 0;
    st_w25n01gvzeig_driver.delay_us  = 0;
    st_w25n01gvzeig_driver.malloc    = 0;
    st_w25n01gvzeig_driver.free      = 0;
    st_w25n01gvzeig_driver.memcpy    = 0;

    if (!driver->spi_tx || !driver->spi_tx_rx || !driver->delay_us)
        return W25N01GVZEIG_SPI_INTERFACE_NULL;

    st_w25n01gvzeig_driver = *driver;
    init_done              = 1;

    return W25N01GVZEIG_SUCCESS;
}



