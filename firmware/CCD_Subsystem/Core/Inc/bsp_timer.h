/*
 * bsp_timer.h
 *
 *  Created on: Sep 10, 2025
 *      Author: Admin
 */

#ifndef SRC_BSP_TIMER_H_
#define SRC_BSP_TIMER_H_

/* Macros to enable/disable global interrupts */
#define ENABLE_INT()        __set_PRIMASK(0) /* Enable global interrupts */
#define DISABLE_INT()       __set_PRIMASK(1) /* Disable global interrupts */
/*
    Define several software timer global variables here.
    Note: You must add __IO (i.e., volatile) because these variables are accessed
    by both interrupts and the main program, which may otherwise lead to compiler
    optimization errors.
*/
#define TMR_COUNT   4       /* Number of software timers (timer ID range 0–3) */

/* Timer mode enumeration; members must be volatile to avoid issues with compiler optimizations */
typedef enum
{
    TMR_ONCE_MODE = 0,      /* One-shot mode */
    TMR_AUTO_MODE = 1       /* Auto (periodic) mode */
} TMR_MODE_E;

/* Timer structure; members must be volatile to avoid issues with compiler optimizations */
typedef struct
{
    volatile uint8_t Mode;      /* Counter mode, one-shot */
    volatile uint8_t Flag;      /* Timeout flag */
    volatile uint32_t Count;    /* Counter */
    volatile uint32_t PreLoad;  /* Counter preload value */
} SOFT_TMR;

/* Functions provided for use by other C files */
void bsp_InitTimer(void);
void bsp_DelayMS(uint32_t n);
void bsp_DelayUS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);
int32_t bsp_GetRunTime(void);
int32_t bsp_CheckRunTime(int32_t _LastTime);

#endif /* SRC_BSP_TIMER_H_ */
