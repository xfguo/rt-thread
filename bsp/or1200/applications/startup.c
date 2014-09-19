/*
 * File      : startuo.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 */


#include <rthw.h>
#include <rtthread.h>
#include "board.h"
#include "timer.h"

extern int rt_application_init(void);
extern void rt_hw_interrupt_init(void);

#ifdef RT_USING_DEVICE
	extern rt_err_t rt_hw_serial_init(void);
#endif

#ifdef RT_USING_FINSH
	#include <finsh.h>
	extern void finsh_system_init(void);
	extern void finsh_set_device(const char* device);
#endif

#ifdef __GNUC__
	extern unsigned char ___bss_start;
	extern unsigned char __end;
#endif

#ifdef RT_USING_UIP
	#include "uip_rtt.h"
	#include "uip-conf.h"
	#include "uip.h"
	#include "uip_arp.h"
	#include "uipif.h"
#endif

extern void rt_hw_board_init();

/**
 * This function will startup RT-Thread RTOS.
 */
void rtthread_startup(void)
{
	/* init hardware interrupt */
	rt_hw_interrupt_init();

	/* init board */
	rt_hw_board_init();
	
	/* version display */
	rt_show_version();

	/* init tick */
	rt_system_tick_init();

	/* init kernel object */
	rt_system_object_init();

	/* init timer system */
	rt_system_timer_init();

#ifdef RT_USING_HEAP
	rt_system_heap_init((void*)&__end, (void*)&___bss_start);
#endif

	/* init scheduler system */
	rt_system_scheduler_init();

#ifdef RT_USING_HOOK /* if the hook is used */
	/* set idle thread hook */
#endif

#ifdef RT_USING_DEVICE
	/* init hardware serial device */
	rt_hw_serial_init();
	
	/* init all device */
	rt_device_init_all();
#endif	
	timer_init();

	/* init application */
	//rt_application_init();
	
#ifdef RT_USING_FINSH
	/* init finsh */
	finsh_system_init();
	finsh_set_device("uart1");
#endif

#ifdef RT_USING_UIP
	//extern void uip_sys_init(void);
	eth_system_device_init();    
	ethoc_initialize(0, 0x92000000);
	uip_sys_init();
	/* re-init device driver */
	rt_device_init_all();
#endif 

	/* init idle thread */
	rt_thread_idle_init();

	/* start scheduler */
	rt_system_scheduler_start();

	/* never reach here */
	return;
}

















