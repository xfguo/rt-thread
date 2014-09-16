/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 */

/**
 * @addtogroup or1200
 */
/*@{*/


#include <rtthread.h>
#include "timer.h"
#include "board.h"
#include "support.h"
#include "spr-defs.h"

rt_thread_t tid1,tid2;

static void thread1_entry(void* parameter)
{
	rt_uint32_t count = 0;
	rt_kprintf("thread1 dynamicly created ok\n");
	do{
		rt_kprintf("thread1 count: %d\n",count++);

		rt_thread_delay(RT_TICK_PER_SECOND);
	}while(1);
}

static void thread2_entry(void* parameter)
{
	rt_uint32_t count = 0;
	rt_kprintf("thread2 dynamicly created ok\n");
	do{
		rt_kprintf("thread2 count: %d\n",count++);

		rt_thread_delay(5*RT_TICK_PER_SECOND);
	}while(1);
}

int rt_application_init()
{
	tid1 = rt_thread_create("thread1", thread1_entry, RT_NULL, 512, 5, 10);
	if (tid1 != RT_NULL){
		rt_thread_startup(tid1);
	}

	tid2 = rt_thread_create("thread2", thread2_entry, RT_NULL, 512, 6, 10);
	if (tid2 != RT_NULL){
		rt_thread_startup(tid2);
	}

	return 0;
}

