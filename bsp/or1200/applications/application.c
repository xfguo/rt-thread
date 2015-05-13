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

#include <drivers/spi.h>

/* FIXME: define this to avoid finsh bug about size_t redefine*/
#define __x86_64__ 
#include "finsh.h"

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

static long spi_flash_rdid(void)
{
	struct rt_spi_device * spi_device;
	
	spi_device = (struct rt_spi_device *)rt_device_find("spi_flash");

	/* config spi */
	{
		struct rt_spi_configuration cfg;
		cfg.mode = RT_SPI_MODE_0 | RT_SPI_MSB; /* SPI Compatible Modes 0 */
		cfg.max_hz = 1000*100; /* 400kbit/s */
		rt_spi_configure(spi_device, &cfg);
	}
	
	/* execute RDID op */
	{
	        struct rt_spi_message message;

		rt_uint8_t send_buffer[4];
		rt_uint8_t recv_buffer[4];

		rt_memset(send_buffer, 0, 4);
		send_buffer[0] = 0x9F;

		message.send_buf = send_buffer;
		message.recv_buf = recv_buffer;
		message.length = 5;
		message.cs_take = 1;
		message.cs_release = 1;

		spi_device->bus->ops->xfer(spi_device, &message);

		rt_kprintf("ManuID:%02x | Device ID:%02x%02x\n",
			recv_buffer[1],
			recv_buffer[2],
			recv_buffer[3]
		);
		
	}

	return 0;
}
FINSH_FUNCTION_EXPORT(spi_flash_rdid, Read NOR Flash ID)

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

