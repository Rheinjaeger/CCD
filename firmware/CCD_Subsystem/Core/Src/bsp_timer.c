/*
 * bsp_timer.c
 *
 *  Created on: Sep 10, 2025
 *      Author: Admin
 */
#include "stm32l0xx_hal.h"
#include "bsp_timer.h"
/* These 2 global variables are used by the bsp_DelayMS() function */
static volatile uint32_t s_uiDelayCount = 0;
static volatile uint8_t s_ucTimeOutFlag = 0;

/* Define software timer structure variables */
static SOFT_TMR s_tTmr[TMR_COUNT] = {0};

/*
    Global runtime in units of 1 ms
    Maximum representable time is 24.85 days.
    If your product runs continuously longer than this, you must consider overflow issues.
*/
__IO int32_t g_iRunTime = 0;

static __IO uint8_t g_ucEnableSystickISR = 0;   /* Wait for variable initialization */

static void bsp_SoftTimerDec(SOFT_TMR *_tmr);

/*
*********************************************************************************************************
*   Function Name: bsp_InitTimer
*   Description  : Configure SysTick interrupt and initialize software timer variables
*   Parameter    : None
*   Return Value : None
*********************************************************************************************************
*/

void bsp_InitTimer(void)
{
    uint8_t i;

    /* Clear all software timers */
    for (i = 0; i < TMR_COUNT; i++)
    {
        s_tTmr[i].Count = 0;
        s_tTmr[i].PreLoad = 0;
        s_tTmr[i].Flag = 0;
        s_tTmr[i].Mode = TMR_ONCE_MODE; /* Default is one-shot mode */
    }

    /*
        Configure SysTick interrupt period to 1 ms and enable SysTick interrupt.

        SystemCoreClock is the system core clock defined in the firmware.
        For STM32H7, it is usually 400 MHz.

        The parameter of SysTick_Config() specifies the number of core clock cycles
        after which a SysTick interrupt is triggered.
            -- SystemCoreClock / 1000  : interrupt frequency 1000 Hz (period = 1 ms)
            -- SystemCoreClock / 500   : interrupt frequency 500 Hz  (period = 2 ms)
            -- SystemCoreClock / 2000  : interrupt frequency 2000 Hz (period = 500 µs)

        For most applications, we use 1 ms as the period.
        For low-speed CPUs or low-power applications, you can set the period to 10 ms.
    */
    SysTick_Config(SystemCoreClock / 1000);

    g_ucEnableSystickISR = 1;       /* 1 means SysTick interrupt is enabled */
}


/*
*********************************************************************************************************
*   Function Name: bsp_Idle
*   Description  : Function executed when CPU is idle.
*                  Typically, in the main program inside for/while loops,
*                  the CPU_IDLE() macro calls this function.
*                  By default, this function does nothing.
*                  Users can add tasks such as feeding the watchdog
*                  or putting the CPU into sleep mode.
*   Parameter    : None
*   Return Value : None
*********************************************************************************************************
*/
void bsp_Idle(void)
{

    //--- Put CPU into sleep, to be woken up by SysTick or other interrupts

    // For example, in the uIP protocol, you can insert the uIP polling function

    // GT811_Scan();   // Touch screen scan
}
/*
*********************************************************************************************************
*   Function Name: SysTick_ISR
*   Description  : SysTick interrupt service routine, executed once every 1 ms
*   Parameter    : None
*   Return Value : None
*********************************************************************************************************
*/

extern void bsp_RunPer1ms(void);
extern void bsp_RunPer10ms(void);
void SysTick_ISR(void)
{
    static uint8_t s_count = 0;
    uint8_t i;

    /* Called every 1 ms (used only for bsp_DelayMS) */
    if (s_uiDelayCount > 0)
    {
        if (--s_uiDelayCount == 0)
        {
            s_ucTimeOutFlag = 1;
        }
    }

    /* Decrement all software timer counters every 1 ms */
    for (i = 0; i < TMR_COUNT; i++)
    {
        bsp_SoftTimerDec(&s_tTmr[i]);
    }

    /* Increase global runtime every 1 ms */
    g_iRunTime++;
    if (g_iRunTime == 0x7FFFFFFF)    /* This variable is int32_t, max value is 0x7FFFFFFF */
    {
        g_iRunTime = 0;
    }

    bsp_RunPer1ms();        /* Call this function every 1 ms, defined in bsp.c */

    if (++s_count >= 10)
    {
        s_count = 0;

        bsp_RunPer10ms();   /* Call this function every 10 ms, defined in bsp.c */
    }
}


/*
*********************************************************************************************************
*   Function Name: bsp_SoftTimerDec
*   Description  : Decrement all timer variables every 1 ms. Must be called periodically by SysTick_ISR.
*   Parameter    : _tmr : Pointer to timer variable
*   Return Value : None
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
    if (_tmr->Count > 0)
    {
        /* If the timer variable decrements to 1, set the timer flag */
        if (--_tmr->Count == 0)
        {
            _tmr->Flag = 1;

            /* If in auto mode, reload the counter automatically */
            if (_tmr->Mode == TMR_AUTO_MODE)
            {
                _tmr->Count = _tmr->PreLoad;
            }
        }
    }
}

/*
*********************************************************************************************************
*   Function Name: bsp_DelayMS
*   Description  : Millisecond-level delay with ±1 ms accuracy
*   Parameter    : n : Delay length in ms. n should be greater than 2
*   Return Value : None
*********************************************************************************************************
*/
void bsp_DelayMS(uint32_t n)
{
    if (n == 0)
    {
        return;
    }
    else if (n == 1)
    {
        n = 2;
    }

    DISABLE_INT();              /* Disable interrupts */

    s_uiDelayCount = n;
    s_ucTimeOutFlag = 0;

    ENABLE_INT();               /* Enable interrupts */

    while (1)
    {
        bsp_Idle();             /* Operations to execute when CPU is idle, see bsp.c and bsp.h */

        /*
            Wait until delay time expires
            Note: Compiler may assume s_ucTimeOutFlag = 0, which can cause optimization issues.
                  Therefore, s_ucTimeOutFlag must be declared as volatile.
        */
        if (s_ucTimeOutFlag == 1)
        {
            break;
        }
    }
}


/*
*********************************************************************************************************
*    Function Name: bsp_DelayUS
*    Description  : Microsecond-level delay. This function must be called after the SysTick timer is started.
*    Parameter    : n : Delay length, in microseconds (1 us)
*    Return Value : None
*********************************************************************************************************
*/
void bsp_DelayUS(uint32_t n)
{
    uint32_t ticks;
    uint32_t told;
    uint32_t tnow;
    uint32_t tcnt = 0;
    uint32_t reload;

    reload = SysTick->LOAD;
    ticks = n * (SystemCoreClock / 1000000);   /* Required number of ticks */

    tcnt = 0;
    told = SysTick->VAL;            /* Counter value upon entry */

    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            /* SYSTICK is a down-counting timer */
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            /* Reload wrap-around */
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;

            /* Exit when elapsed time >= requested delay */
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}

/*
*********************************************************************************************************
*   Function Name: bsp_StartTimer
*   Description  : Start a timer and set its period.
*   Parameter    : _id     : Timer ID, range [0, TMR_COUNT-1]. The user must manage IDs to avoid conflicts.
*                  _period : Timer period in milliseconds
*   Return Value : None
*********************************************************************************************************
*/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
        /* Print the source filename and function name for the error */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
        while(1); /* Parameter error, halt and wait for watchdog reset */
    }

    DISABLE_INT();              /* Disable interrupts */

    s_tTmr[_id].Count  = _period;   /* Runtime counter initial value */
    s_tTmr[_id].PreLoad = _period;  /* Auto-reload value; used only in auto mode */
    s_tTmr[_id].Flag   = 0;         /* Timeout flag */
    s_tTmr[_id].Mode   = TMR_ONCE_MODE; /* One-shot mode */

    ENABLE_INT();               /* Enable interrupts */
}


/*
*********************************************************************************************************
*   Function Name: bsp_StartAutoTimer
*   Description  : Start an auto-reload timer and set its period.
*   Parameter    : _id     : Timer ID, range [0, TMR_COUNT-1]. The user must manage IDs to avoid conflicts.
*                  _period : Timer period in units of 10 ms
*   Return Value : None
*********************************************************************************************************
*/
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
    if (_id >= TMR_COUNT)
    {
        /* Print the source filename and function name for the error */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
        while(1); /* Parameter error, halt and wait for watchdog reset */
    }

    DISABLE_INT();        /* Disable interrupts */

    s_tTmr[_id].Count  = _period;   /* Runtime counter initial value */
    s_tTmr[_id].PreLoad = _period;  /* Auto-reload value (used only in auto mode) */
    s_tTmr[_id].Flag   = 0;         /* Timeout flag */
    s_tTmr[_id].Mode   = TMR_AUTO_MODE; /* Auto (periodic) mode */

    ENABLE_INT();         /* Enable interrupts */
}

/*
*********************************************************************************************************
*   Function Name: bsp_StopTimer
*   Description  : Stop a timer
*   Parameter    : _id : Timer ID, range [0, TMR_COUNT-1]. The user must manage IDs to avoid conflicts.
*   Return Value : None
*********************************************************************************************************
*/
void bsp_StopTimer(uint8_t _id)
{
    if (_id >= TMR_COUNT)
    {
        /* Print the source filename and function name for the error */
        BSP_Printf("Error: file %s, function %s()\r\n", __FILE__, __FUNCTION__);
        while(1); /* Parameter error, halt and wait for watchdog reset */
    }

    DISABLE_INT();    /* Disable interrupts */

    s_tTmr[_id].Count = 0;             /* Runtime counter initial value */
    s_tTmr[_id].Flag  = 0;             /* Timeout flag */
    s_tTmr[_id].Mode  = TMR_ONCE_MODE; /* One-shot mode */

    ENABLE_INT();      /* Enable interrupts */
}

/*
*********************************************************************************************************
*   Function Name: bsp_CheckTimer
*   Description  : Check whether a timer has timed out
*   Parameter    : _id : Timer ID, range [0, TMR_COUNT-1]. The user must manage IDs to avoid conflicts.
*   Return Value : Returns 0 if not timed out, 1 if timed out
*********************************************************************************************************
*/
uint8_t bsp_CheckTimer(uint8_t _id)
{
    if (_id >= TMR_COUNT)
    {
        return 0;
    }

    if (s_tTmr[_id].Flag == 1)
    {
        s_tTmr[_id].Flag = 0;
        return 1;
    }
    else
    {
        return 0;
    }
}

/*
*********************************************************************************************************
*   Function Name: bsp_GetRunTime
*   Description  : Get CPU run time in milliseconds.
*                  Maximum representable duration is 24.85 days; if your product runs longer,
*                  you must handle overflow.
*   Parameter    : None
*   Return Value : CPU runtime in milliseconds
*********************************************************************************************************
*/
int32_t bsp_GetRunTime(void)
{
    int32_t runtime;

    DISABLE_INT();   /* Disable interrupts */

    runtime = g_iRunTime;  /* Modified in SysTick ISR; protect by disabling interrupts */

    ENABLE_INT();    /* Enable interrupts */

    return runtime;
}

/*
*********************************************************************************************************
*   Function Name: bsp_CheckRunTime
*   Description  : Compute the difference between the current runtime and a given timestamp.
*                  Handles counter wrap-around.
*   Parameter    : _LastTime : Previous timestamp
*   Return Value : Time difference in milliseconds
*********************************************************************************************************
*/
int32_t bsp_CheckRunTime(int32_t _LastTime)
{
    int32_t now_time;
    int32_t time_diff;

    DISABLE_INT();   /* Disable interrupts */

    now_time = g_iRunTime;  /* Modified in SysTick ISR; protect by disabling interrupts */

    ENABLE_INT();    /* Enable interrupts */

    if (now_time >= _LastTime)
    {
        time_diff = now_time - _LastTime;
    }
    else
    {
        time_diff = 0x7FFFFFFF - _LastTime + now_time;
    }

    return time_diff;
}


/*
*********************************************************************************************************
*   Function Name: SysTick_Handler
*   Description  : System tick timer interrupt service routine. This function is referenced in the startup file.
*   Parameter    : None
*   Return Value : None
*********************************************************************************************************
*/
/*
void SysTick_Handler(void)
{
    HAL_IncTick();

    if (g_ucEnableSystickISR == 0)
    {
        return;
    }

    SysTick_ISR();
}
*/



