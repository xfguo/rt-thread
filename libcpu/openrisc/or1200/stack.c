/*
 * File      : stack.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://openlab.rt-thread.com/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2013-06-08	  				   
 *
 */

#include <rtthread.h>
#include "spr-defs.h"
#include "support.h"

/**
 * This function will initialize thread stack
 *
 * @param tentry the entry of thread
 * @param parameter the parameter of entry
 * @param stack_addr the beginning stack address
 * @param texit the function will be called when thread exit
 *
 * @return stack address
 */
rt_uint8_t *rt_hw_stack_init(void *tentry, void *parameter, rt_uint8_t *stack_addr, void *texit)
{
	rt_uint32_t *stk = (rt_uint32_t* )(stack_addr);
	
	stk -= 32;	
        
	// *stk-- = (rt_uint32_t)(parameter);               /* fp+0 -> parameter 0    */
	// fp     = stk;
	// *stk-- = (rt_uint32_t)fp;                    /* sp+4 -> previous fp    */
	// *stk-- = (rt_uint32_t)0;                     /* sp+0 -> return address */
         
	*stk-- = (rt_uint32_t)31;                     /* r31 = 0  */
	*stk-- = (rt_uint32_t)30;                     /* r30 = 0  */
	*stk-- = (rt_uint32_t)29;                     /* r29 = 0  */
	*stk-- = (rt_uint32_t)28;                     /* r28 = 0  */
	*stk-- = (rt_uint32_t)27;                     /* r27 = 0  */
	*stk-- = (rt_uint32_t)26;                     /* r26 = 0  */
	*stk-- = (rt_uint32_t)25;                     /* r25 = 0  */
	*stk-- = (rt_uint32_t)24;                     /* r24 = 0  */
	*stk-- = (rt_uint32_t)23;                     /* r23 = 0  */
	*stk-- = (rt_uint32_t)22;                     /* r22 = 0  */
	*stk-- = (rt_uint32_t)21;                     /* r21 = 0  */
	*stk-- = (rt_uint32_t)20;                     /* r20 = 0  */
	*stk-- = (rt_uint32_t)19;                     /* r19 = 0  */
	*stk-- = (rt_uint32_t)18;                     /* r18 = 0  */
	*stk-- = (rt_uint32_t)17;                     /* r17 = 0  */
	*stk-- = (rt_uint32_t)16;                     /* r16 = 0  */
	*stk-- = (rt_uint32_t)15;                     /* r15 = 0  */
	*stk-- = (rt_uint32_t)14;                     /* r14 = 0  */
	*stk-- = (rt_uint32_t)13;                     /* r13 = 0  */
	*stk-- = (rt_uint32_t)12;                     /* r12 = 0  */
	*stk-- = (rt_uint32_t)11;                     /* r11 = 0 return value */
	*stk-- = (rt_uint32_t)10;                     /* r10 = 0 */
	*stk-- = (rt_uint32_t)texit;                     /* r09 = texit, link register */
	*stk-- = (rt_uint32_t)8;                     /* r08 = 0 function paramters*/
	*stk-- = (rt_uint32_t)7;                     /* r07 = 0 */
	*stk-- = (rt_uint32_t)6;                     /* r06 = 0 */
	*stk-- = (rt_uint32_t)5;                     /* r05 = 0 */
	*stk-- = (rt_uint32_t)4;                     /* r04 = 0 */
	*stk-- = (rt_uint32_t)(parameter);               /* r03 = arg0 */
	*stk-- = (rt_uint32_t)0;                    /* r02 = frame pointer */
	*stk-- = (rt_uint32_t)(SPR_SR_IEE | SPR_SR_TEE | SPR_SR_SM);         /* status word */
	*stk   = (rt_uint32_t)(tentry);                /* program counter */
        
        /* return task's current stack address */
	return (rt_uint8_t *)stk;
}
