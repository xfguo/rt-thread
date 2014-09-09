/*
 * File      : interrupt.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 */

/*
 * User interrupt handler software for OR1200
*/

#include "spr-defs.h"
#include "support.h"
#include "interrput.h"
#include <rtthread.h>

extern volatile rt_uint8_t rt_interrupt_nest;

/* switch flag on interrupt and thread pointer to save switch record */
rt_uint32_t rt_interrupt_from_thread;
rt_uint32_t rt_interrupt_to_thread;
rt_uint8_t rt_thread_switch_interrupt_flag;

/* Interrupt handlers table */
struct ihnd int_handlers[MAX_INT_HANDLERS];

/* Initialize interrupt handlers table routine */
int rt_hw_interrupt_init()
{
	int i;
		
	for(i = 0; i < MAX_INT_HANDLERS; i++){
		int_handlers[i].handler = 0;
		int_handlers[i].arg = 0;
  	}
  	mtspr(SPR_PICMR, 0x00000000);
	//set OR1200 to accept exceptions
	mtspr(SPR_SR, mfspr(SPR_SR) | SPR_SR_IEE);

	/* init interrupt nest, and context in thread sp */
	rt_interrupt_nest = 0;
	rt_interrupt_from_thread = 0;
	rt_interrupt_to_thread = 0;
	rt_thread_switch_interrupt_flag = 0;
	
	return 0;
}

/* Add interrupt handler */ 
int rt_hw_interrupt_install(unsigned long vect, void (* handler)(void *), void *arg)
{
	if(vect >= MAX_INT_HANDLERS)
		return -1;

	int_handlers[vect].handler = handler;
	int_handlers[vect].arg = arg;

	mtspr(SPR_PICMR, mfspr(SPR_PICMR) | (0x00000001L << vect));

	return 0;
}

/* Disable interrupt */ 
int rt_hw_interrupt_mask(unsigned long vect)
{
	if(vect >= MAX_INT_HANDLERS)
		return -1;

	mtspr(SPR_PICMR, mfspr(SPR_PICMR) & ~(0x00000001L << vect));

	return 0;
}

/* Enable interrupt */ 
int rt_hw_interrupt_unmask(unsigned long vect)
{
	if(vect >= MAX_INT_HANDLERS)
                return -1;

	mtspr(SPR_PICMR, mfspr(SPR_PICMR) | (0x00000001L << vect));

	return 0;
}

/* Main interrupt handler */
void rt_hw_trap_irq()
{
	unsigned long picsr = mfspr(SPR_PICSR);   //process only the interrupts asserted at signal catch, ignore all during process
	unsigned long i = 0;

	while(i < 32) {
		if((picsr & (0x01L << i)) && (int_handlers[i].handler != 0)) {
			(*int_handlers[i].handler)(int_handlers[i].arg); 
		}
		i++;
	}
	mtspr(SPR_PICSR, 0);	//clear interrupt status: all modules have level interrupts, which have to be cleared by software,
					//thus this is safe, since non processed interrupts will get re-asserted soon enough
}

/*
 * void rt_hw_context_switch_interrupt(rtuint32 from, rtuint32 to);
 */
void rt_hw_context_switch_interrupt(rt_uint32_t from, rt_uint32_t to)
{
        if (rt_thread_switch_interrupt_flag != 1)
        {
                rt_thread_switch_interrupt_flag = 1;
                rt_interrupt_from_thread = from;
        }
        rt_interrupt_to_thread = to;
}

/* Disable or1200 interrupt */
rt_uint32_t rt_hw_interrupt_disable(void)
{
	rt_uint32_t cpu_sr;
 
	cpu_sr = mfspr(SPR_SR);
	mtspr(SPR_SR, cpu_sr & ~(SPR_SR_IEE | SPR_SR_TEE));
	return cpu_sr;
}

/* Enable or1200 interrupt */ 
void rt_hw_interrupt_enable(rt_uint32_t cpu_sr)
{
	mtspr(SPR_SR, cpu_sr); /* | (SPR_SR_IEE | SPR_SR_TEE) */
}

/* thread switch process using system call by l.sys0 */
void rt_hw_context_switch(rt_uint32_t from, rt_uint32_t to)
{
    rt_interrupt_from_thread = from;
    rt_interrupt_to_thread = to;
    __asm__ ("l.sys 0");
    __asm__ ("l.nop");
}
