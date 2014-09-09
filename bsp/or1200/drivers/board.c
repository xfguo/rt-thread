/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 */

#include <rthw.h>
#include <rtthread.h>
#include "support.h"
#include "board.h"
#include "serial.h"
#include "timer.h"

void rt_hw_timer_handler(void)
{
	/* enter interrupt, done in assembly */
	
	TimerClearInterrupt();
	rt_tick_increase();

	/* leave interrupt, done in assembly */
} 

#ifdef RT_USING_CONSOLE
/*
 * RT-Thread Console Interface, used by rt_kprintf
 */
/**
 * This function is used to display a string on console, normally, it's
 * invoked by rt_kprintf
 *
 * @param str the displayed string
 */
 void rt_hw_console_output(const char* str)
 {
	while(*str){
		/* Transmit Character */
		if(*str == '\n'){
			/* Wait for transfer fifo empty*/
			while((REG8(UART_BASE + UART_LSR) & UART_LSR_THRE) != UART_LSR_THRE);
			REG8(UART_BASE + UART_TX) = '\r';
		}
		
		/* Wait for transfer fifo empty*/
		while((REG8(UART_BASE + UART_LSR) & UART_LSR_THRE) != UART_LSR_THRE);
		
		/* transfer a character */
		REG8(UART_BASE + UART_TX) = *str;
		/* point to next character */
		str++;
	}
 }

/* init console to support rt_kprintf */
static void rt_hw_console_init()
{
	rt_int32_t divisor;
	float float_divisor;
 
	/* Reset receiver and transmiter */
	//start up FIFO ,clear RCVR FIFO and XMIT FIFO ,and set FIFO size for 4 bytes
	REG8(UART_BASE + UART_FCR) = UART_FCR_ENABLE_FIFO | UART_FCR_CLEAR_RCVR | UART_FCR_CLEAR_XMIT | UART_FCR_TRIGGER_4;
 
	/* Disable all interrupts */
	REG8(UART_BASE + UART_IER) = 0x0;
        
	/* Enable RX and TX interrupt */
	// REG8(UART_BASE + UART_IER) = UART_IER_RDI | UART_IER_THRI;
 
	/* Set 8 bit char, 1 stop bit, no parity */
	REG8(UART_BASE + UART_LCR) = UART_LCR_WLEN8 & ~(UART_LCR_STOP | UART_LCR_PARITY);
 
	/* Set baud rate */
	float_divisor = (float)SYS_CLK/(16 * UART_BAUD_RATE);
	float_divisor += 0.50f; // Ensure round up
	divisor = (rt_int32_t)float_divisor;
        
	/* set corresponding register value */
	REG8(UART_BASE + UART_LCR) |= UART_LCR_DLAB;
	REG8(UART_BASE + UART_DLM) = (divisor >> 8) & 0x000000ff;
	REG8(UART_BASE + UART_DLL) = divisor & 0x000000ff;
	REG8(UART_BASE + UART_LCR) &= ~(UART_LCR_DLAB);
}
#endif

void rt_hw_board_init()
{
	/* init console before using rt_kprint() */
	rt_hw_console_init();	
	
	/* init UserISR interrupt routines */
}
