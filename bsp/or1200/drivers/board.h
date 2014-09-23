/*
 * File      : board.h
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 */
 
#ifndef _BOARD_H_
#define _BOARD_H_

/* System Clock */
#define SYS_CLK			40000000 /* 40MHz */
#define TICKS_PER_SECOND	100

/* UART */
#define UART_BAUD_RATE	38400
#define UART_BASE	0x90000000
#define UART_IRQ	2

/* Ethernet */
#define ETHERNET_BASE		0x92000000
#define ETHERNET_IRQ		4

#endif /* _BOARD_H_ */
