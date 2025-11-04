
#ifndef W25N01GVZEIG_H
#define W25N01GVZEIG_H

/*

- Part one: NAND Flash overview:
    W25N01GV has 1Gbit memory = 65536 * 2048 bytes
    one page size equals 2048 bytes(user) +
    one block size = 64 pages = 64 * 2048 bytes = 128KB
    W25N01GV has 1024 blocks = 1024 * 128KB = 128MB = 1Gbit

    W25N01GV supports JEDEC JEDEC standard manufacturer and device ID
    one page for unique ID
    one page for parameter
    ten pages for OTP
    user configurable internal ECC
    support bad block management

    “Continuous Read Mode (BUF=0),  and the “Buffer Read Mode (BUF=1)”
    W25N01GVxxIG: Default BUF=1 after power up
    W25N01GVxxIT: Default BUF=0 after power up

- Part two: NAND address
    according to datasheet, W25N01GV address is split to Page address and Column address.

    Column address covers one actual page, maximum bit index in one page is 2111(2048+64-1),
    2111 in dec could be expressed as 1000 0111 1111 in binary,
    if access user storage only, maximum bit index in one page is 2047(2048-1),
    2047 in dec could be expressed as 0111 1111 1111 in binary.

-------------------------------------------------------------------------------
    Column address(CA) [11:0]

   | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
   | EXT|                 Byte Address(0-2047)       |
-------------------------------------------------------------------------------

    Page address consists of two parts: Page Addr and Block Addr
    Page Addr covers all pages in one block, maximum page index in one block is 63'd,
    63'd could be expressed as 0011 1111'b, we need 6 bits to address page offset in block.
    Block Addr covers all blocks in one NAND, maximum block index in one NAND is 1023'd,
    1023'd could be expressed as 0011 1111 1111'b, we need 10 bits to address block offset in NAND.

-------------------------------------------------------------------------------
    Page Address(PA) [12:27] = Page Addr [12:17] + Block Addr [18:27]

    27 | 26 | 25 | 24 | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 | 15 | 14 | 13 | 12 | 11 |
    |           Block Addr (0-1023)                 |          Page Addr (0-63)        |

-------------------------------------------------------------------------------

- Part three: SPI Interface
    W25N01GV supports SPI Mode 0 and Mode 3 for standard SPI.
    MSB first

    W25N01GV supports Dual SPI operation,The Dual SPI Read instructions are ideal for
quickly downloading code to RAM upon power-up (code-shadowing).When using Dual SPI instructions, the DI and DO pins become
bidirectional I/O pins: IO0 and IO1.

    W25N01GV supports Quad SPI operation when using "Fast Read Quad Output(6Bh)",
“Fast Read Quad I/O (EBh)” and “Quad Program Data Load (32h/34h)”. When using Quad SPI
instructions the DI and DO pins become bidirectional IO0 and IO1, and the /WP and /HOLD pins become
IO2 and IO3 respectively.

- Part four: Hold Pin
Hold operation:
    when SPI bus must used for other operation and read/write NAND process is under-going,
    MCU could pull down HOLD pin to pause operation, wait SPI bus is free to continue NAND operation.

- Part five: Write Protect
Write protect:
    WP pin pull high to enable write protection.
    write SR-1 register.

    After power-up the device is automatically placed in a write-disabled state with the Status Register Write
    Enable Latch (WEL) set to a 0.
    A Write Enable instruction must be issued before a:
        1) Program Execute,
        2) Block Erase,
        3) Bad Block Management instruction

    After completing a
        1) program
        2) erase
    instruction the Write Enable Latch (WEL) is automatically cleared to a write-disabled state of 0.

    Tips!!!: Must issue write enable command before any write/erase operation on NAND

    Software controlled write protection allows a portion
    or the entire memory array to be configured as read only.

    The WP-E bit in Protection Register (SR-1) is used to enable the hardware protection. When WP-E is set
    to 1, bringing /WP low in the system will block any Write/Program/Erase command to the W25N01GV, the
    device will become read-only. The Quad SPI operations are also disabled when WP-E is set to 1.

    Use hardware WP progress:
    1) set WP-E bit in SR-1 register to 1
    2) pull down WP pin to disable any write/erase operation on NAND

    W25N01GV has 3 status control registers:
    1) Protection register(SR-1)
    2) Configuration register(SR-2)
    3) Status Register(SR-3)

    The Read Status Register instruction (05h / 0Fh) can be used to provide:
    1) status on the availability of the flash memory array,
    2) the state of write protection,
    3) Read modes,
    4) Protection Register/OTP area lock status,
    5) Erase/Program results,
    6) ECC usage/status.

    The Write Status Register instruction can be used to configure:
    1) the device write protection features,
    2) Software/Hardware write protection,
    3) Read modes,
    4) enable/disable ECC,
    5) Protection Register/OTP area lock.

    Write access to the Status Register is controlled by:
    1) the state of the non-volatile Status Register Protect bits (SRP0, SRP1),
    2) the Write Enable instruction,
    3) when WP-E is set to 1, the /WP pin.

    *** Protection Register / Status Register-1 (Volatile Writable, OTP lockable) ***
    | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
    | SRP0 |  BP3 |  BP2 |  BP1 |  BP0 |  TB  | WP-E | SRP1 |

    1) Block Protect Bits
    Block Protect Bits(BP3, BP2, BP1, BP0, TB) - volatile writable, OTP lockable
    Block Protect Bits can be set using the Write Status Register Instruction.
    The default values for the Block Protection bits are 1 after power up to protect the entire array.

    2) Write Protection Enable Bit(WP-E)
    WP-E - volatile writable, OTP lockable
    The WP-E bit, in conjunction with SRP1 & SRP0, controls the method of write protection.
    1) When WP-E = 0 (default value), the device is in Software Protection mode, /WP & /HOLD pins are multiplexed as IO pins,
    and Quad program/read functions are enabled all the time.
    2) When WP-E is set to 1, the device is in Hardware Protection mode, all Quad functions are disabled,
    and /WP & /HOLD pins become dedicated control input pins.

    3) Status Register Protect Bits
    The SRP bits control the method of write protection

-------------------------------------------------------------------------------------
        Software Protection (Driven by Controller, Quad Program/Read is enabled)
    SRP1    SRP0    WP-E    /WP /IO2        Descriptions
    0       0       0           X           No /WP functionality, /WP pin will always function as IO2
    0       1       0           0           SR-1 cannot be changed (/WP = 0 during Write Status) /WP pin will function as IO2 for Quad operations
    0       1       0           1           SR-1 can be changed (/WP = 1 during Write Status) /WP pin will function as IO2 for Quad operations
    1       0       0           X           Power Lock Down(1) SR-1, /WP pin will always function as IO2
    1       1       0           X           Enter OTP mode to protect SR-1 (allow SR1-L=1), /WP pin will always function as IO2
-------------------------------------------------------------------------------------
        Hardware Protection (System Circuit / PCB layout, Quad Program/Read is disabled)
    SRP1    SRP0    WP-E    /WP only        Descriptions
    0       X       1       VCC             SR-1 can be changed
    1       0       1       VCC             Power Lock-Down(1) SR-1
    1       1       1       VCC             Enter OTP mode to protect SR-1 (allow SR1-L=1)
    X       X       1       GND             All "Write/Program/Erase" commands are blocked, entire device (SRs, Array, OTP area) is read-only
-------------------------------------------------------------------------------------

*** Configuration Register / Status Register-2 (Volatile Writable) ***
    | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
    |OTP-L |OTP-E |SR1-L |ECC-E |  BUF | (R)  | (R)  |  (R) |

    1) One Time Program Lock Bit (OTP-L) – OTP lockable
    In addition to the main memory array, W25N01GV also provides an OTP area for the system to store critical data that cannot be changed once it’s locked.\
    The OTP area consists of 10 pages of 2,112-Byte each. The default data in the OTP area are FFh.
    Only Program command can be issued to the OTP area to change the data from “1” to “0”
    and data is not reversible (“0” to “1”) by the Erase command. Once the correct data
    is programmed in and verified, the system developer can set OTP-L bit to 1, so that the entire OTP area
    will be locked to prevent further alteration to the data.

    2) Enter OTP Access Mode Bit (OTP-E) – Volatile Writable
    The OTP-E bit must be set to 1 in order to use the standard Program/Read commands to access the OTP
    area as well as to read the Unique ID / Parameter Page information.
    The default value after power up or a RESET command is 0.

    3) Status Register-1 Lock Bit (SR1-L) – OTP lockable
    The SR1-L lock bit is used to OTP lock the values in the Protection Register (SR-1). Depending on the
    settings in the SR-1, the device can be configured to have a portion of or up to the entire array to be writeprotected,
    and the setting can be OTP locked by setting SR1-L bit to 1. SR1-L bit can only be set to 1
    permanently when SRP1 & SRP0 are set to (1,1), and OTP Access Mode must be entered (OTP-E=1) to
    execute the programming. Please refer to 8.2.26 for detailed information.

    4) ECC Enable Bit (ECC-E) – Volatile Writable
    W25N01GV has a built-in ECC algorithm that can be used to preserve the data integrity. Internal ECC
    calculation is done during page programming, and the result is stored in the extra 64-Byte area for each
    page. During the data read operation, ECC engine will verify the data values according to the previously
    stored ECC information and to make necessary corrections if needed. The verification and correction status
    is indicated by the ECC Status Bits. ECC function is enabled by default when power on (ECC-E=1), and it
    will not be reset to 0 by the Device Reset command.

    5) Buffer Read / Continuous Read Mode Bit (BUF) – Volatile Writable

    The Buffer Read Mode (BUF=1) requires a Column Address to start outputting the existing data inside the
    Data Buffer, and once it reaches the end of the data buffer (Byte 2,111), DO (IO1) pin will become high-Z state.

    The Continuous Read Mode (BUF=0) doesn’t require the starting Column Address. The device will always
    start output the data from the first column (Byte 0) of the Data buffer, and once the end of the data buffer
    (Byte 2,048) is reached, the data output will continue through the next memory page. With Continuous
    Read Mode, it is possible to read out the entire memory array using a single read command. Please refer
    to respective command descriptions for the dummy cycle requirements for each read commands under
    different read modes.

    For W25N01GVxxIG part number, the default value of BUF bit after power up is 1
    For W25N01GVxxIT part number, the default value of BUF bit after power up is 0

---------------------------------------------------------------------------------------------
    BUF     ECC-E       Read Mode(Starting from Buffer)     ECC Status      Data Output Structure
    1       0               Buffer Read                     N/A             2,048 + 64
    1       1               Buffer Read                     Page based      2,048 + 64
    0       0               Continuous Read                 N/A             2,048
    0       1               Continuous Read                 Operation based 2,048
---------------------------------------------------------------------------------------------

    *** Status Register-3 (Status Only) ***
    | bit7 | bit6 | bit5 | bit4 | bit3 | bit2 | bit1 | bit0 |
    | (R)  |LUT-F |ECC-1 |ECC-0 |P-FAIL|E-FAIL|  WEL | BUSY |

    1) Look-Up Table Full (LUT-F) – Status Only
    When 20 bad blocks have been replaced in BBM LUT, bit LUT-F is set to 1

    2) Cumulative ECC Status (ECC-1, ECC-0) – Status Only
    ECC function is used in NAND flash memory to correct limited memory errors during read operations. The
    ECC Status Bits (ECC-1, ECC-0) should be checked after the completion of a Read operation to verify the
    data integrity. The ECC Status bits values are don’t care if ECC-E=0. These bits will be cleared to 0 after a
    power cycle or a RESET command.

----------------------------------------------------------------------------------
    ECC     Status            Descriptions
    ECC-1 |  ECC-0
    0           0           Entire data output is successful, without any ECC correction.

    0           1           Entire data output is successful, with 1~4 bit/page ECC corrections
                            in either a single page or multiple pages.

    1           0           Entire data output contains more than 4 bits errors only in a single page
                            which cannot be repaired by ECC.
                            In the Continuous Read Mode, an additional command can be used to read out
                            the Page Address (PA) which had the errors.

    1           1           Entire data output contains more than 4 bits errors/page in multiple pages.
                            In the Continuous Read Mode, the additional command can only provide the
                            last Page Address (PA) that had failures, the user cannot obtain the PAs for
                            other failure pages. Data is not suitable to use.
----------------------------------------------------------------------------------

    3) Program/Erase Failure (P-FAIL, E-FAIL) – Status Only
    The Program/Erase Failure Bits are used to indicate whether the internally-controlled Program/Erase
    operation was executed successfully or not.

    4) Write Enable Latch (WEL) – Status Only
    Write Enable Latch (WEL) is a read only bit in the status register (SR2) that is set to 1 after executing a Write
    Enable Instruction. The WEL status bit is cleared to 0 when the device is write disabled. A write disable
    state occurs upon power-up or after any of the following instructions: Write Disable, Program Execute, Block
    Erase, Page Data Read, Program Execute and Bad Block Management for OTP pages.

    5) Erase/Program In Progress (BUSY) – Status Only
    During busy time the device will ignore further instructions except for
    the Read Status Register and Read JEDEC ID instructions. When the
    program, erase or write status register instruction has completed,
    the BUSY bit will be cleared to a 0 state
    indicating the device is ready for further instructions.

    we assume spi has configures MSB first
*/

#define W25N01GV_SECTOR_SIZE              512
#define W25N01GV_SECTORS_PER_PAGE         4
#define W25N01GV_DATA_BYTES_PER_PAGE      (W25N01GV_SECTOR_SIZE * W25N01GV_SECTORS_PER_PAGE) /* user memory */
#define W25N01GV_SPARE_BYTES_PER_PAGE     64                                                 /* spare area for tages,ecc, et al. */
#define W25N01GV_PAGE_SIZE                (W25N01GV_DATA_BYTES_PER_PAGE + W25N01GV_SPARE_BYTES_PER_PAGE)
#define W25N01GV_PAGES_PER_BLOCK          64
#define W25N01GV_BLOCK_SIZE               (W25N01GV_PAGE_SIZE * W25N01GV_PAGES_PER_BLOCK)
#define W25N01GV_BLOCKS_PER_NAND          1024
#define W25N01GV_NAND_SIZE                (W25N01GV_BLOCK_SIZE * W25N01GV_BLOCKS_PER_NAND)

#define W25N01GV_UNIQUE_ID_PAGES          1
#define W25N01GV_PARAMETER_PAGES          1
#define W25N01GV_OTP_PAGES                10

#define W25N01GV_UNIQUE_ID_PAGE_ADDR      0
#define W25N01GV_UNIQUE_ID_PAGE_SIZE      (32 * 16)
#define W25N01GV_PARAMETER_PAGE_ADDR      1
#define W25N01GV_PARAMETER_PAGE_COPY_CNT  3
#define W25N01GV_PARAMETER_PAGE_COPY_SIZE 256

#define W25N01GV_PAGE_ADDR_LEN            6
#define W25N01GV_BLOCK_ADDR_LEN           10

/* get offset in one page from offset in specified sector */
#define PAGE_ADDR_FROM_SECTOR(SECTOR_INDEX, BYTE_OFFSET_IN_SECTOR) (SECTOR_INDEX * W25N01GV_SECTOR_SIZE + BYTE_OFFSET_IN_SECTOR);

/* acknowledge which sector from page addr */
#define SECTOR_INDEX_FROM_PAGE_ADDR(PAGE_ADDR) (PAGE_ADDR / W25N01GV_SECTOR_SIZE)

/* acknowledge sector addr from page addr */
#define SECTOR_ADDR_FROM_PAGE_ADDR(PAGE_ADDR) (PAGE_ADDR % W25N01GV_SECTOR_SIZE)

#define W25N01GV_MANUFACTURER_ID              0xef
#define W25N01GV_DEVICE_ID                    0xaa21

#define W25N01GV_DUMMY                        0x00
#define W25N01GV_MAX_BBM_LUT                  20
#define W25N01GV_MAX_RANDOM_LOAD_BYTES        50

#define W25N01GV_BLOCK_ERASE_US               10000
#define W25N01GV_PROGRAM_US                   700
#define W25N01GV_BBM_US                       700
#define W25N01GV_OTP_LOCK_US                  700
#define W25N01GV_PAGE_READ_US                 60
#define W25N01GV_PAGE_READ_NO_ECC_US          25

/* instruction set table operation code */
#define W25N01GV_DEVICE_RESET                         0xff
#define W25N01GV_JEDEC_ID                             0x9f
#define W25N01GV_READ_STATUS                          0x0f
#define W25N01GV_WRITE_STATUS                         0x1f
#define W25N01GV_WRITE_ENABLE                         0x06
#define W25N01GV_WRITE_DISABLE                        0x04
#define W25N01GV_BBM                                  0xa1 /* bad block management */
#define W25N01GV_READ_BBM_LUT                         0xa5
#define W25N01GV_LAST_ECC_FAIL_PAGE_ADDRESS           0xa9
#define W25N01GV_BLOCK_ERASE                          0xd8
#define W25N01GV_PROGRAM_DATA_LOAD                    0x02
#define W25N01GV_RANDOM_PROGRAM_DATA_LOAD             0x84
#define W25N01GV_QUAD_PROGRAM_DATA_LOAD               0x32
#define W25N01GV_RANDOM_QUAD_PROGRAM_DATA_LOAD        0x34
#define W25N01GV_PROGRAM_EXECUTE                      0x10
#define W25N01GV_PAGE_DATA_READ                       0x13
#define W25N01GV_READ                                 0x03
#define W25N01GV_FAST_READ                            0x0b
#define W25N01GV_FAST_READ_WITH_4BYTE_ADDRESS         0x0c
#define W25N01GV_FAST_READ_DUAL                       0x3b
#define W25N01GV_FAST_READ_DUAL_WITH_4BYTE_ADDRESS    0x3c
#define W25N01GV_FAST_READ_QUAD                       0x6b
#define W25N01GV_FAST_READ_QUAD_WITH_4BYTE_ADDRESS    0x6c
#define W25N01GV_FAST_READ_DUAL_IO                    0xbb
#define W25N01GV_FAST_READ_DUAL_IO_WITH_4BYTE_ADDRESS 0xbc
#define W25N01GV_FAST_READ_QUAD_IO                    0xeb
#define W25N01GV_FAST_READ_QUAD_IO_WITH_4BYTE_ADDRESS 0xec

/* register address */
#define W25N01GVZEIG_SR1_ADDR 0xa0
#define W25N01GVZEIG_SR2_ADDR 0xb0
#define W25N01GVZEIG_SR3_ADDR 0xc0

/* otp page address */
#define W25N01GV_OTP_UNIQUE_PAGE_ADDR    0x00
#define W25N01GV_OTP_PARAMETER_PAGE_ADDR 0x01
#define W25N01GV_OTP_PAGE0_ADDR          0x02
#define W25N01GV_OTP_PAGE1_ADDR          0x03
#define W25N01GV_OTP_PAGE2_ADDR          0x04
#define W25N01GV_OTP_PAGE3_ADDR          0x05
#define W25N01GV_OTP_PAGE4_ADDR          0x06
#define W25N01GV_OTP_PAGE5_ADDR          0x07
#define W25N01GV_OTP_PAGE6_ADDR          0x08
#define W25N01GV_OTP_PAGE7_ADDR          0x09
#define W25N01GV_OTP_PAGE8_ADDR          0x0a
#define W25N01GV_OTP_PAGE9_ADDR          0x0b

/* return value */
#define W25N01GVZEIG_SUCCESS                 0
#define W25N01GVZEIG_SPI_INTERFACE_NULL      -1
#define W25N01GVZEIG_BUSY                    -2
#define W25N01GVZEIG_SR_ADDR_MISMATCH        -3
#define W25N01GVZEIG_SR_BIT_READONLY         -4
#define W25N01GVZEIG_NULL_POINTER            -5
#define W25N01GVZEIG_MALLOC_FREE_NULL        -6
#define W25N01GVZEIG_MALLOC_FAIL             -7
#define W25N01GVZEIG_LOAD_OVER_PAGE          -8
#define W25N01GVZEIG_QUAD_DISABLE            -9
#define W25N01GVZEIG_FAST_READ_MODE_WRONG    -10
#define W25N01GVZEIG_SOFT_SPI_INTERFACE_NULL -11
#define W25N01GVZEIG_SPI_MODE_MISMATCH       -12
#define W25N01GVZEIG_PARAMETER_MISMATCH      -13
#define W25N01GVZEIG_BUFFER_TOO_SMALL        -14

#define W25N01GVZEIG_LITTLE_ENDIAN           0
#define W25N01GVZEIG_BIG_ENDIAN              1

/* all bit field in SR1,2,3 which can be written */
#define W25NGVZEIG_SR1_BIT_WR_SRP   0
#define W25NGVZEIG_SR1_BIT_WR_WPE   1
#define W25NGVZEIG_SR1_BIT_WR_TB    2
#define W25NGVZEIG_SR1_BIT_WR_BP    3
#define W25NGVZEIG_SR2_BIT_WR_OTP_L 4
#define W25NGVZEIG_SR2_BIT_WR_OTP_E 5
#define W25NGVZEIG_SR2_BIT_WR_SR1_L 6
#define W25NGVZEIG_SR2_BIT_WR_ECC_E 7
#define W25NGVZEIG_SR2_BIT_WR_BUF   8

#define SPI_INSTRUCTION_NEXT_ADDR   (1 << 0)
#define SPI_ADDRESS_ON_IO0          (1 << 1)
#define SPI_ADDRESS_ON_IO01         (1 << 2)
#define SPI_ADDRESS_ON_IO0123       (1 << 3)
#define SPI_NEXT_DATA               (1 << 4)
#define SPI_NEXT_DATA_ON_IO0        (1 << 5)
#define SPI_NEXT_DATA_ON_IO01       (1 << 6)
#define SPI_NEXT_DATA_ON_IO0123     (1 << 7)

/* fast read option */
enum w25n01gvzeig_fast_read_mode {
    normal,
    normal_with_4byte,
    dual,
    dual_with_4byte,
    quad,
    quad_with_4byte,
    dual_io,
    dual_io_with_4byte,
    quad_io,
    quad_io_with_4byte
};

enum Read_Mode {
    Continuous,
    Buffer
};

enum w25n01gv_soft_spi_io_line {
    cs,
    io0,
    io1,
    io2,
    io3
};

enum w2501gvzeig_memory_protection_field {
    None,
    cover_ff80_ffff_256KB,
    cover_ff00_ffff_512KB,
    cover_fe00_ffff_1MB,
    cover_fc00_ffff_2MB,
    cover_f800_ffff_4MB,
    cover_f000_ffff_8MB,
    cover_e000_ffff_16MB,
    cover_c000_ffff_32MB,
    cover_8000_ffff_64MB,
    cover_0000_007f_256KB,
    cover_0000_00ff_512KB,
    cover_0000_01ff_1MB,
    cover_0000_03ff_2MB,
    cover_0000_07ff_4MB,
    cover_0000_0fff_8MB,
    cover_0000_1fff_16MB,
    cover_0000_3fff_32MB,
    cover_0000_7fff_64MB,
    All
};

enum w2501gvzeig_busy_status {
    idle,
    busy
};

enum w25n01gv_otp_page {
    unique_id_page,
    parameter_page,
    otp_page0,
    otp_page1,
    otp_page2,
    otp_page3,
    otp_page4,
    otp_page5,
    otp_page6,
    otp_page7,
    otp_page8,
    otp_page9
};

typedef unsigned char w25n01gv_uint8_t;
typedef unsigned short w25n01gv_uint16_t;
typedef unsigned int w25n01gv_uint32_t;

/// @brief spi interface, return value: 0 success, other failed
typedef int (*w25n01gv_spi_tx_interface)(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t tx_io_struct);
typedef int (*w25n01gv_spi_tx_rx_interface)(w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt, w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, w25n01gv_uint8_t tx_io_struct, w25n01gv_uint8_t rx_io_struct);

// use soft spi io pins to realize:
// fast read dual io
// fast read dual io with 4 byte address
// fast read quad io
// fast read quad io with 4 byte address
// since io operation used 4 bit clock(not enough for 1 byte)
/// @brief delay control
typedef int (*w25n01gv_delay_us)(w25n01gv_uint16_t delay_us);

typedef void *(*w25n01gv_malloc)(w25n01gv_uint32_t size);
typedef void (*w25n01gv_free)(void *pos);
typedef void (*w25n01gv_memcpy)(void *str1, const void *str2, w25n01gv_uint32_t n);

/* address structual */
typedef struct W25N01GVZEIG_ADDR_STRUCTUAL {
    w25n01gv_uint32_t column_address : 12;
    w25n01gv_uint32_t page_addr : 6;
    w25n01gv_uint32_t block_addr : 10;
    w25n01gv_uint32_t reserved : 4;
} w25n01gvzeig_addr_structual;

union w25n01gvzeig_address {
    w25n01gv_uint32_t u32_addr;
    w25n01gvzeig_addr_structual addr_structual;
};

/* part 1 (16 bytes) in spare area 64 bytes */
typedef struct W25N01GVZEIG_SPARE_PART_STRUCTUAL {
    w25n01gv_uint8_t bad_block_mark[2];
    w25n01gv_uint8_t user_data_part2[2];
    w25n01gv_uint8_t user_data_part1[4];
    w25n01gv_uint8_t ecc_for_sector[6];
    w25n01gv_uint8_t ecc_for_spare[2];
} w25n01gvzeig_spare_part_structual;

/* spare area 64 bytes */
typedef struct W25N01GVZEIG_SPARE {
    w25n01gvzeig_spare_part_structual spare[4];
} w25n01gvzeig_spare;

/* status register 1 */
typedef struct W25NGVZEIG_STATUS_REGISTER_1 {
    w25n01gv_uint8_t status_register_protect_1 : 1;
    w25n01gv_uint8_t write_protect_enable : 1;
    w25n01gv_uint8_t top_bottom_protect : 1;
    w25n01gv_uint8_t block_protect_0 : 1;
    w25n01gv_uint8_t block_protect_1 : 1;
    w25n01gv_uint8_t block_protect_2 : 1;
    w25n01gv_uint8_t block_protect_3 : 1;
    w25n01gv_uint8_t status_register_protect_0 : 1;
} w25n01gvzeig_SR1;

/* status register 2 */
typedef struct W25NGVZEIG_STATUS_REGISTER_2 {
    w25n01gv_uint8_t reserved : 3;
    w25n01gv_uint8_t read_buf_continue_mode : 1;
    w25n01gv_uint8_t ecc_enable : 1;
    w25n01gv_uint8_t status_reg1_lock : 1;
    w25n01gv_uint8_t otp_enable : 1;
    w25n01gv_uint8_t otp_lock : 1;
} w25n01gvzeig_SR2;

/* status register 3 */
typedef struct W25NGVZEIG_STATUS_REGISTER_3 {
    w25n01gv_uint8_t busy : 1;
    w25n01gv_uint8_t write_enable_latch : 1;
    w25n01gv_uint8_t erase_failure : 1;
    w25n01gv_uint8_t program_failure : 1;
    w25n01gv_uint8_t ecc_status_0 : 1;
    w25n01gv_uint8_t ecc_status_1 : 1;
    w25n01gv_uint8_t bbm_lut_full : 1; /* bad block management look up table full */
    w25n01gv_uint8_t reserved : 1;
} w25n01gvzeig_SR3;

typedef struct W25N01GVZEIG_DEVICE_INFO {
    w25n01gv_uint16_t data_bytes_per_page;
    w25n01gv_uint16_t spare_bytes_per_page;
    w25n01gv_uint16_t sector_size;

    w25n01gv_uint16_t pages_per_block;
    w25n01gv_uint16_t blocks_per_nand;
} w25n01gvzeig_deivce_info;

typedef struct W25N01GVZEIG_DRIVER {
    w25n01gv_spi_tx_interface spi_tx;
    w25n01gv_spi_tx_rx_interface spi_tx_rx; /* write reg then read value, keep CS low during write and read */

    w25n01gv_delay_us delay_us;

    w25n01gv_malloc malloc;
    w25n01gv_free free;
    w25n01gv_memcpy memcpy;
} w25n01gvzeig_driver;

/* W25N01GVxxIG/IT Instruction begin */

/* Tips: some instructions detail varies under buffer-mode and continus-mode */

/// @brief reset device
/// @return
int w25n01gv_reset();

/// @brief fetch NAND manufacture id and JEDEC id
/// @param manufacture_id
/// @param jedec_id jedec_id[0] = aa, jedec_id[1] = 21
/// @return
int w25n01gv_jedec_id(w25n01gv_uint8_t *manufacture_id, w25n01gv_uint8_t *jedec_id);

/// @brief The Read Status Register instruction may be used at any time, even while a Program or Erase cycle is in
/// progress.This allows the BUSY status bit to be checked to determine when the cycle is complete and if the
/// device can accept another instruction.
/// @param reg_addr
/// @param status
/// @return
int w25n01gv_read_status(w25n01gv_uint8_t reg_addr, w25n01gv_uint8_t *status);

/// @brief The Write Status Register instruction allows the Status Registers to be written. The writable Status Register
/// bits include: SRP[1:0], TB, BP[3:0] and WP-E bit in Status Register-1; OTP-L, OTP-E, SR1-L, ECC-E and
/// BUF bit in Status Register-2. All other Status Register bit locations are read-only and will not be affected
/// by the Write Status Register instruction.
/// @param reg_addr
/// @param status
/// @return
int w25n01gv_write_status(w25n01gv_uint8_t reg_addr, w25n01gv_uint8_t item, w25n01gv_uint8_t value);

/// @brief The Write Enable instruction sets the Write Enable Latch (WEL) bit in the Status Register to a 1.
/// The WEL bit must be set prior to every Page Program, Quad Page Program, Block Erase and Bad Block Management instruction.
/// @return
int w25n01gv_write_enable();

/// @brief The Write Disable instruction (Figure 10) resets the Write Enable Latch (WEL) bit in the Status Register to 0.
///  Note that the WEL bit is automatically reset after Power-up and upon
/// completion of the Page Program, Quad Page Program, Block Erase, Reset and Bad Block Management instructions.
/// @return
int w25n01gv_write_disable();

/// @brief Due to large NAND memory density size and the technology limitation, NAND memory devices are allowed
/// to be shipped to the end customers with certain amount of “Bad Blocks” found in the factory testing. Up to
/// 2% of the memory blocks can be marked as “Bad Blocks” upon shipment, which is a maximum of 20 blocks
/// for W25N01GV. In order to identify these bad blocks, it is recommended to scan the entire memory array
/// for bad block markers set in the factory. A “Bad Block Marker” is a non-FFh data byte stored at Byte 0 of
/// Page 0 for each bad block. An additional marker is also stored in the first byte of the 64-Byte spare area.
/// Once a Bad Block Management command is successfully executed, the specified LBA-PBA link will be
// added to the internal Look Up Table (LUT). Up to 20 links can be established in the non-volatile LUT. If all
// 20 links have been written, the LUT-F bit in the Status Register will become a 1, and no more LBA-PBA
// links can be established. Therefore, prior to issuing the Bad Block Management command, the LUT-F bit
// value can be checked or a “Read BBM Look Up Table” command can be issued to confirm if spare links
// are still available in the LUT.
/// @param lba logical block address
/// @param pba physical block address
/// @return
int w25n01gv_bad_block_management(w25n01gv_uint16_t lba, w25n01gv_uint16_t pba);

/// @brief The internal Look Up Table (LUT) consists of 20 Logical-Physical memory block links (from LBA0/PBA0 to
// LBA19/PBA19). The “Read BBM Look Up Table” command can be used to check the existing address links
// stored inside the LUT.
/// @param lba all LBA(20), output 00h if not used
/// @param pba all PBA(20), output 00h if not used
/// @return
int w25n01gv_read_bbm_lut(w25n01gv_uint16_t *lba, w25n01gv_uint16_t *pba);

/// @brief To better manage the data integrity, W25N01GV implements internal ECC correction for the entire memory
// array. When the ECC-E bit in the Status/Configuration Register is set to 1 (also power up default), the
// internal ECC algorithm is enabled for all Program and Read operations. During a “Program Execute”
// command for a specific page, the ECC algorithm will calculate the ECC information based on the data
// inside the 2K-Byte data buffer and write the ECC data into the extra 64-Byte ECC area in the same physical
// memory page.During the Read operations, ECC information will be used to verify the data read out from the physical
// memory array and possible corrections can be made to limited amount of data bits that contain errors.The
//  ECC Status Bits(ECC - 1 & ECC - 0) will also be set indicating the result of ECC calculation.
// For the “Continuous Read Mode (BUF=0)” operation, multiple pages of main array data can be read out
// continuously by issuing a single read command. Upon finishing the read operation, the ECC status bits
// should be check to verify if there’s any ECC correction or un-correctable errors existed in the read out data.
// If ECC-1 & ECC-0 equal to (1, 0) or (1, 1), the previous read out data contain one or more pages that
// contain ECC un-correctable errors. The failure page address (or the last page address if it’s multiple pages)
// can be obtained by issuing the “Last ECC failure Page Address” command as illustrated in Figure 13. The
// 16-bit Page Address that contains un-correctable ECC errors will be presented on the DO pin following the
// instruction code “A9h” and 8-bit dummy clocks on the DI pin.
/// @param page_addres
/// @return
int w25n01gv_last_ecc_fail_page_address(w25n01gv_uint16_t *page_addres);

/// @brief The 128KB Block Erase instruction sets all memory within a specified block (64-Pages, 128K-Bytes) to the
// erased state of all 1s(FFh).A Write Enable instruction must be executed before the device will accept the
// Block Erase Instruction(Status Register bit WEL must equal 1).The instruction is initiated by driving the
// CS pin low and shifting the instruction code “D8h” followed by 8 -bit dummy clocks and the 16 - bit page address.
// The /CS pin must be driven high after the eighth bit of the last byte has been latched. If this is not done the
// Block Erase instruction will not be executed. After /CS is driven high, the self-timed Block Erase instruction
// will commence for a time duration of tBE (See AC Characteristics). While the Block Erase cycle is in
// progress, the Read Status Register instruction may still be accessed for checking the status of the BUSY
// bit. The BUSY bit is a 1 during the Block Erase cycle and becomes a 0 when the cycle is finished and the
// device is ready to accept other instructions again. After the Block Erase cycle has finished the Write Enable
// Latch (WEL) bit in the Status Register is cleared to 0. The Block Erase instruction will not be executed
// if the addressed block is protected by the Block Protect(TB, BP2, BP1, and BP0) bits.
/// Page Address has 16bit, bit 15-6 is block addr, bit 5-0 is page addr, ignore page addr and focus block addr
/// @param block_page_addr
/// @return
int w25n01gv_block_erase(w25n01gv_uint16_t block_page_addr);

/// @brief The Program operation allows from one byte to 2,112 bytes (a page) of data to be programmed at previously
// erased(FFh) memory locations.A Program operation involves two steps :
// 1. Load the program data into the Data Buffer.
// 2: Issue “Program Execute” command to transfer the data from Data Buffer to the specified memory page.
// A Write Enable instruction must be executed before the device will accept the Load Program Data
// Instructions (Status Register bit WEL= 1). The “Load Program Data” or “Random Load Program Data”
// instruction is initiated by driving the /CS pin low then shifting the instruction code “02h” or “84h” followed by
// a 16-bit column address (only CA[11:0] is effective) and at least one byte of data into the DI pin. The /CS
// pin must be held low for the entire length of the instruction while data is being sent to the device. If the
// number of data bytes sent to the device exceeds the number of data bytes in the Data Buffer, the extra
// data will be ignored by the device.
// Both “Load Program Data” and “Random Load Program Data” instructions share the same command
// sequence.The difference is that “Load Program Data” instruction will reset the unused the data bytes in
// the Data Buffer to FFh value, while “Random Load Program Data” instruction will only update the data bytes
// that are specified by the command input sequence, the rest of the Data Buffer will remain unchanged.
/// @param column_addr column address indicates offset inside page
/// @param data data buffer pointer
/// @param load_cnt load bytes count
/// @param random_load 0 load program data, 1 random load program data
/// @return
int w25n01gv_program_data_load(w25n01gv_uint16_t column_addr, w25n01gv_uint8_t *data, w25n01gv_uint16_t load_cnt, w25n01gv_uint8_t random_load);

/// @brief The “Quad Load Program Data” and “Quad Random Load Program Data” instructions are identical to the
// “Load Program Data” and “Random Load Program Data” in terms of operation sequence and functionality.
// The only difference is that “Quad Load” instructions will input the data bytes from all four IO pins instead of
// the single DI pin.This method will significantly shorten the data input time when a large amount of data
// needs to be loaded into the Data Buffer.
// Both “Quad Load Program Data” and “Quad Random Load Program Data” instructions share the same
// command sequence.The difference is that “Quad Load Program Data” instruction will reset the unused the
// data bytes in the Data Buffer to FFh value, while “Quad Random Load Program Data” instruction will only
// update the data bytes that are specified by the command input sequence, the rest of the Data Buffer will remain unchanged.
/// @param column_addr
/// @param data
/// @param load_cnt load data byte count
/// @param random_load 0 normal load, 1 random load
/// @return
int w25n01gv_quad_program_data_load(w25n01gv_uint16_t column_addr, w25n01gv_uint8_t *data, w25n01gv_uint16_t load_cnt, w25n01gv_uint8_t random_load);

/// @brief The Program Execute instruction is the second step of the Program operation. After the program data are
// loaded into the 2, 112 - Byte Data Buffer(or 2, 048 bytes when ECC is enabled),
// the Program Execute instruction will program the Data Buffer content into the physical memory page that is specified in the instruction.
// The instruction is initiated by driving the / CS pin low then shifting the instruction code “10h”
// followed by 8 - bit dummy clocks and the 16 - bit Page Address into the DI pin.
// After /CS is driven high to complete the instruction cycle, the self-timed Program Execute instruction will
// commence for a time duration of tpp (See AC Characteristics). While the Program Execute cycle is in
// progress, the Read Status Register instruction may still be used for checking the status of the BUSY bit.
// The BUSY bit is a 1 during the Program Execute cycle and becomes a 0 when the cycle is finished and the
// device is ready to accept other instructions again. After the Program Execute cycle has finished, the Write
// Enable Latch (WEL) bit in the Status Register is cleared to 0. The Program Execute instruction will not be
// executed if the addressed page is protected by the Block Protect(TB, BP2, BP1, and BP0) bits.
// Only 4 partial page program times are allowed on every single page.
// The pages within the block have to be programmed sequentially from the lower order page address to the
// higher order page address within the block.Programming pages out of sequence is prohibited!!!
/// @param page_addr
/// @return
int w25n01gv_program_execute(w25n01gv_uint16_t page_addr);

/// @brief The Page Data Read instruction will transfer the data of the specified memory page into the 2,112-Byte
// Data Buffer.The instruction is initiated by driving the / CS pin low then shifting the instruction code
// “13h” followed by 8 - bit dummy clocks and the 16 - bit Page Address into the DI pin.
// After /CS is driven high to complete the instruction cycle, the self-timed Read Page Data instruction will
// commence for a time duration of tRD (See AC Characteristics). While the Read Page Data cycle is in
// progress, the Read Status Register instruction may still be used for checking the status of the BUSY bit.
// The BUSY bit is a 1 during the Read Page Data cycle and becomes a 0 when the cycle is finished and the
// device is ready to accept other instructions again.
// After the 2, 112 bytes of page data are loaded into the Data Buffer, several Read instructions can be issued
// to access the Data Buffer and read out the data.Depending on the BUF bit setting in the Status Register,
// either “Buffer Read Mode” or “Continuous Read Mode” may be used to accomplish the read operations.
/// @param page_addr
/// @return
int w25n01gv_paga_data_read(w25n01gv_uint16_t page_addr);

/// @brief The Read Data instruction allows one or more data bytes to be sequentially read from the Data Buffer after
// executing the Read Page Data instruction. The Read Data instruction is initiated by driving the /CS pin low
// and then shifting the instruction code “03h” followed by the 16-bit Column Address and 8-bit dummy clocks
// or a 24-bit dummy clocks into the DI pin. After the address is received, the data byte of the addressed Data
// Buffer location will be shifted out on the DO pin at the falling edge of CLK with most significant bit (MSB)
// first. The address is automatically incremented to the next higher address after each byte of data is shifted
// out allowing for a continuous stream of data. The instruction is completed by driving /CS high.
// When BUF = 1, the device is in the Buffer Read Mode.The data output sequence will start from
// the Data Buffer location specified by the 16 - bit Column Address and continue to
// the end of the Data Buffer.Once the last byte of data is output, the output pin will become Hi - Z state.
// When BUF = 0, the device is in the Continuous Read Mode,
// the data output sequence will start from the first byte of the Data Buffer and
// increment to the next higher address.When the end of the Data Buffer is reached,
// the data of the first byte of next memory page will be following and continues through
// the entire memory array.This allows using a single Read instruction to read out the
// entire memory array and is also compatible to Winbond’s SpiFlash NOR flash memory command sequence.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_read(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief The Fast Read instruction allows one or more data bytes to be sequentially read from the Data Buffer after
// executing the Read Page Data instruction. The Fast Read instruction is initiated by driving the /CS pin low
// and then shifting the instruction code “0Bh” followed by the 16-bit Column Address and 8-bit dummy clocks
// or a 32-bit dummy clocks into the DI pin. After the address is received, the data byte of the addressed Data
// Buffer location will be shifted out on the DO pin at the falling edge of CLK with most significant bit (MSB)
// first. The address is automatically incremented to the next higher address after each byte of data is shifted
// out allowing for a continuous stream of data. The instruction is completed by driving /CS high.
// When BUF=1, the device is in the Buffer Read Mode.The data output sequence will start from the Data Buffer location
// specified by the 16 - bit Column Address and continue to the end of the Data Buffer.Once the last byte of data is output,
// the output pin will become Hi - Z state.
// When BUF = 0, the device is in the Continuous Read Mode, the data output sequence will start from the first byte
// of the Data Buffer and increment to the next higher address.When the end of the Data Buffer is reached,
// the data of the first byte of next memory page will be following and continues through the entire memory array.
// This allows using a single Read instruction to read out the entire memory array and is also compatible to Winbond’s
// SpiFlash NOR flash memory command sequence.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief The Fast Read instruction allows one or more data bytes to be sequentially read from the Data Buffer after
// executing the Read Page Data instruction. The Fast Read instruction is initiated by driving the /CS pin low
// and then shifting the instruction code “0Ch” followed by the 16-bit Column Address and 24-bit dummy clocks
// (when BUF=1) or a 40-bit dummy clocks (when BUF=0) into the DI pin. After the address is received, the
// data byte of the addressed Data Buffer location will be shifted out on the DO pin at the falling edge of CLK
// with most significant bit (MSB) first. The address is automatically incremented to the next higher address
// after each byte of data is shifted out allowing for a continuous stream of data. The instruction is completed
// by driving /CS high.
// The Fast Read instruction sequence is shown in Figure 21a & 21b. When BUF=1, the device is in the Buffer
// Read Mode. The data output sequence will start from the Data Buffer location specified by the 16-bit Column
// Address and continue to the end of the Data Buffer. Once the last byte of data is output, the output pin will
// become Hi-Z state. When BUF=0, the device is in the Continuous Read Mode, the data output sequence
// will start from the first byte of the Data Buffer and increment to the next higher address. When the end of
// the Data Buffer is reached, the data of the first byte of next memory page will be following and continues
// through the entire memory array. This allows using a single Read instruction to read out the entire memory
// array and is also compatible to Winbond’s SpiFlash NOR flash memory command sequence.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief The Fast Read Dual Output (3Bh) instruction is similar to the standard Fast Read (0Bh) instruction except
// that data is output on two pins; IO0 and IO1.This allows data to be transferred at twice the rate of standard SPI devices.
// The Fast Read Dual Output instruction sequence is shown in Figure 22a & 22b. When BUF=1, the device
// is in the Buffer Read Mode. The data output sequence will start from the Data Buffer location specified by
// the 16-bit Column Address and continue to the end of the Data Buffer. Once the last byte of data is output,
// the output pin will become Hi-Z state. When BUF=0, the device is in the Continuous Read Mode, the data
// output sequence will start from the first byte of the Data Buffer and increment to the next higher address.
// When the end of the Data Buffer is reached, the data of the first byte of next memory page will be following
// and continues through the entire memory array. This allows using a single Read instruction to read out the
// entire memory array and is also compatible to Winbond’s SpiFlash NOR flash memory command sequence.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_dual_output(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief The Fast Read Dual Output (3Ch) instruction is similar to the standard Fast Read (0Bh) instruction except
// that data is output on two pins; IO0 and IO1.This allows data to be transferred at twice the rate of standard SPI devices.
// The Fast Read Dual Output instruction sequence is shown in Figure 23a & 23b. When BUF=1, the device
// is in the Buffer Read Mode. The data output sequence will start from the Data Buffer location specified by
// the 16-bit Column Address and continue to the end of the Data Buffer. Once the last byte of data is output,
// the output pin will become Hi-Z state. When BUF=0, the device is in the Continuous Read Mode, the data
// output sequence will start from the first byte of the Data Buffer and increment to the next higher address.
// When the end of the Data Buffer is reached, the data of the first byte of next memory page will be following
// and continues through the entire memory array. This allows using a single Read instruction to read out the
// entire memory array and is also compatible to Winbond’s SpiFlash NOR flash memory command sequence.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_dual_output_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief The Fast Read Quad Output (6Bh) instruction is similar to the Fast Read Dual Output (3Bh) instruction
// except that data is output on four pins, IO0, IO1, IO2, and IO3. The Fast Read Quad Output Instruction allows
// data to be transferred at four times the rate of standard SPI devices.
// The Fast Read Quad Output instruction sequence is shown in Figure 24a & 24b. When BUF=1, the device
// is in the Buffer Read Mode. The data output sequence will start from the Data Buffer location specified by
// the 16-bit Column Address and continue to the end of the Data Buffer. Once the last byte of data is output,
// the output pin will become Hi-Z state. When BUF=0, the device is in the Continuous Read Mode, the data
// output sequence will start from the first byte of the Data Buffer and increment to the next higher address.
// When the end of the Data Buffer is reached, the data of the first byte of next memory page will be following
// and continues through the entire memory array. This allows using a single Read instruction to read out the
// entire memory array and is also compatible to Winbond’s SpiFlash NOR flash memory command sequence.
// When WP-E bit in the Status Register is set to a 1, this instruction is disabled.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_quad_output(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief The Fast Read Quad Output (6Ch) instruction is similar to the Fast Read Dual Output (3Bh) instruction
// except that data is output on four pins, IO0, IO1, IO2, and IO3. The Fast Read Quad Output Instruction allows
// data to be transferred at four times the rate of standard SPI devices.
// The Fast Read Quad Output instruction sequence is shown in Figure 25a & 25b. When BUF=1, the device
// is in the Buffer Read Mode. The data output sequence will start from the Data Buffer location specified by
// the 16-bit Column Address and continue to the end of the Data Buffer. Once the last byte of data is output,
// the output pin will become Hi-Z state. When BUF=0, the device is in the Continuous Read Mode, the data
// output sequence will start from the first byte of the Data Buffer and increment to the next higher address.
// When the end of the Data Buffer is reached, the data of the first byte of next memory page will be following
// and continues through the entire memory array. This allows using a single Read instruction to read out the
// entire memory array and is also compatible to Winbond’s SpiFlash NOR flash memory command sequence.
// When WP-E bit in the Status Register is set to a 1, this instruction is disabled.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_quad_output_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief The Fast Read Dual I/O (BBh) instruction allows for improved random access while maintaining two IO
// pins, IO0 and IO1. It is similar to the Fast Read Dual Output (3Bh) instruction but with the capability to input
// the Column Address or the dummy clocks two bits per clock. This reduced instruction overhead may allow for code execution (XIP)
// directly from the Dual SPI in some applications.
// The Fast Read Quad Output instruction sequence is shown in Figure 26a & 26b. When BUF=1, the device
// is in the Buffer Read Mode. The data output sequence will start from the Data Buffer location specified by
// the 16-bit Column Address and continue to the end of the Data Buffer. Once the last byte of data is output,
// the output pin will become Hi-Z state. When BUF=0, the device is in the Continuous Read Mode, the data
// output sequence will start from the first byte of the Data Buffer and increment to the next higher address.
// When the end of the Data Buffer is reached, the data of the first byte of next memory page will be following
// and continues through the entire memory array. This allows using a single Read instruction to read out the
// entire memory array and is also compatible to Winbond’s SpiFlash NOR flash memory command sequence.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_dual_io(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_dual_io_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief When WP-E bit in the Status Register is set to a 1, this instruction is disabled.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_quad_io(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/// @brief When WP-E bit in the Status Register is set to a 1, this instruction is disabled.
/// @param column_address read start address inside a page buffer
/// @param data_buf buffer to store read bytes
/// @param read_cnt read bytes count
/// @return
int w25n01gv_fast_read_quad_io_with4byte_addr(w25n01gv_uint16_t column_address, w25n01gv_uint8_t *data_buf, w25n01gv_uint32_t read_cnt);

/* W25N01GVxxIG/IT Instruction end */

/// @brief resolve TB, BP3, BP2, BP1, BP0 bit in SR1 if need to set a bunch of blocks under protection
/// @param mpf memory protection field enum value
/// @param SR1 status register-1
/// @return
int resolve_SR1_block_protection_bits_field(enum w2501gvzeig_memory_protection_field mpf, w25n01gvzeig_SR1 *SR1);

/// @brief
/// @param SR1
/// @return
int read_SR1(w25n01gvzeig_SR1 *SR1);

/// @brief
/// @param SR2
/// @return
int read_SR2(w25n01gvzeig_SR2 *SR2);

/// @brief
/// @param SR3
/// @return
int read_SR3(w25n01gvzeig_SR3 *SR3);

/// @brief check if w2501gvzeig is busy
/// @param busy idle = 0 or busy = 1
/// @return
int w2501gv_busy(w25n01gv_uint8_t *busy);

/// @brief fetch w25n1gvzeig device info
/// @param info
/// @return
int w25n01gvzeig_fetch_device_info(w25n01gvzeig_deivce_info *info);

/// @brief
/// @param rx_buf
/// @param rx_cnt
/// @param
/// @return
int w25n01gv_read_otp_page(w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt, enum w25n01gv_otp_page which_otp_page);

/// @brief
/// @param page_index
/// @param tx_buf
/// @param tx_cnt
/// @return
int w25n01gv_write_page(w25n01gv_uint16_t page_index, w25n01gv_uint16_t start_pos, w25n01gv_uint8_t *tx_buf, w25n01gv_uint16_t tx_cnt);

/// @brief
/// @param page_index
/// @param start_pos
/// @param rx_buf
/// @param rx_cnt
/// @return
int w25n01gv_read_page(w25n01gv_uint16_t page_index, w25n01gv_uint16_t start_pos, w25n01gv_uint8_t *rx_buf, w25n01gv_uint16_t rx_cnt);

/// @brief /* tx, tx-rx, delay interface are required, others are internal implement, use your interface if you provide  */
/// @param dev
/// @return
int w25n01gv_init(w25n01gvzeig_driver *dev);

/* Functionality achievement end */

#endif
