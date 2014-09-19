#include "spr-defs.h"
#include "ptrace.h"


/* function prototypes */
	/* internal functions */
	.global		_reset
	.global		_align
	.global		_rt_hw_context_switch
	.global		timer_switch_interrupt_do
	.global		_timer_isr
	.global		isr_switch_interrupt_do
	.global		_usr_isr
	.global		rt_hw_context_switch_to

	.extern		rtthread_startup
	.extern		rt_hw_timer_handler
	.extern		rt_thread_switch_interrupt_flag
	.extern		rt_interrupt_to_thread
	.extern		rt_interrupt_from_thread
	.extern		rt_interrupt_enter
	.extern		rt_hw_trap_irq
	.extern		rt_interrupt_leave

/* parameter defines */
#define REDZONE 128
#define STK_FRAME_SIZE (128 + REDZONE)

/* Macro Definitions */
#define CLEAR_GPR(gpr) \
	l.movhi	gpr, 0x0

#define LOAD_SYMBOL_TO_GPR(gpr, symbol) 	\
	.global	symbol;			\
	l.movhi	gpr, hi(symbol);	\
	l.ori	gpr, gpr, lo(symbol)

/* Really goes to configurable interrupt handler */
#define EXCEPTION_HANDLER(symbol);						\
	l.addi	r1, r1, -STK_FRAME_SIZE;					\
	l.sw	PT_GPR3(r1), r3; /* we mass r3, so save r3 to stack first */ 	\
	LOAD_SYMBOL_TO_GPR(r3, symbol);      					\
	l.jr	r3;	\
	l.nop

/* Store the cpu context on the stack */
#define STORE_CONTEXT		;\
	l.sw	PT_GPR2(r1), r2	;\
	l.sw	PT_GPR4(r1), r4	;\
	l.sw	PT_GPR5(r1), r5	;\
	l.sw	PT_GPR6(r1), r6	;\
	l.sw	PT_GPR7(r1), r7	;\
	l.sw	PT_GPR8(r1), r8	;\
	l.sw	PT_GPR9(r1), r9	;\
	l.sw	PT_GPR10(r1), r10	;\
	l.sw	PT_GPR11(r1), r11	;\
	l.sw	PT_GPR12(r1), r12	;\
	l.sw	PT_GPR13(r1), r13	;\
	l.sw	PT_GPR14(r1), r14	;\
	l.sw	PT_GPR15(r1), r15	;\
	l.sw	PT_GPR16(r1), r16	;\
	l.sw	PT_GPR17(r1), r17	;\
	l.sw	PT_GPR18(r1), r18	;\
	l.sw	PT_GPR19(r1), r19	;\
	l.sw	PT_GPR20(r1), r20	;\
	l.sw	PT_GPR21(r1), r21	;\
	l.sw	PT_GPR22(r1), r22	;\
	l.sw	PT_GPR23(r1), r23	;\
	l.sw	PT_GPR24(r1), r24	;\
	l.sw	PT_GPR25(r1), r25	;\
	l.sw	PT_GPR26(r1), r26	;\
	l.sw	PT_GPR27(r1), r27	;\
	l.sw	PT_GPR28(r1), r28	;\
	l.sw	PT_GPR29(r1), r29	;\
	l.sw	PT_GPR30(r1), r30	;\
	l.sw	PT_GPR31(r1), r31	;

/* Restore the cpu context from the stack */
#define RESTORE_CONTEXT	;\
	l.lwz	r2, PT_GPR2(r1)	;\
	l.lwz	r3, PT_GPR3(r1)	;\
	l.lwz	r4, PT_GPR4(r1)	;\
	l.lwz	r5, PT_GPR5(r1)	;\
	l.lwz	r6, PT_GPR6(r1)	;\
	l.lwz	r7, PT_GPR7(r1)	;\
	l.lwz	r8, PT_GPR8(r1)	;\
	l.lwz	r9, PT_GPR9(r1)	;\
	l.lwz	r10, PT_GPR10(r1)	;\
	l.lwz	r11, PT_GPR11(r1)	;\
	l.lwz	r12, PT_GPR12(r1)	;\
	l.lwz	r13, PT_GPR13(r1)	;\
	l.lwz	r14, PT_GPR14(r1)	;\
	l.lwz	r15, PT_GPR15(r1)	;\
	l.lwz	r16, PT_GPR16(r1)	;\
	l.lwz	r17, PT_GPR17(r1)	;\
	l.lwz	r18, PT_GPR18(r1)	;\
	l.lwz	r19, PT_GPR19(r1)	;\
	l.lwz	r20, PT_GPR20(r1)	;\
	l.lwz	r21, PT_GPR21(r1)	;\
	l.lwz	r22, PT_GPR22(r1)	;\
	l.lwz	r23, PT_GPR23(r1)	;\
	l.lwz	r24, PT_GPR24(r1)	;\
	l.lwz	r25, PT_GPR25(r1)	;\
	l.lwz	r26, PT_GPR26(r1)	;\
	l.lwz	r27, PT_GPR27(r1)	;\
	l.lwz	r28, PT_GPR28(r1)	;\
	l.lwz	r29, PT_GPR29(r1)	;\
	l.lwz	r30, PT_GPR30(r1)	;\
	l.lwz	r31, PT_GPR31(r1)	;

/* Linked from 0x0 */
.section .vectors, "ax"
/* ===========================[ exceptions ]============================= */
/* ---[ 0x100: RESET exception ]----------------------------------------- */
	.org	0x100
	LOAD_SYMBOL_TO_GPR(r3, _reset)
	l.jr	r3
	l.nop

/* ---[ 0x200: BUS exception ]------------------------------------------- */
	.org	0x200

/* ---[ 0x300: Data Page Fault exception ]------------------------------- */
	.org	0x300

/* ---[ 0x400: Insn Page Fault exception ]------------------------------- */
	.org	0x400

/* ---[ 0x500: Timer exception ]----------------------------------------- */
	.org	0x500

	EXCEPTION_HANDLER(_timer_isr)

/* ---[ 0x600: Aligment exception ]-------------------------------------- */
	.org	0x600
	EXCEPTION_HANDLER(_align)

/* ---[ 0x700: Illegal insn exception ]---------------------------------- */
	.org	0x700

/* ---[ 0x800: External interrupt exception ]---------------------------- */
	.org	0x800

	EXCEPTION_HANDLER(_usr_isr)

/* ---[ 0x900: DTLB miss exception ]------------------------------------- */
	.org	0x900

/* ---[ 0xa00: ITLB miss exception ]------------------------------------- */
	.org	0xa00

/* ---[ 0xb00: Range exception ]----------------------------------------- */
	.org	0xb00

/* ---[ 0xc00: Syscall exception ]--------------------------------------- */
	.org	0xc00
	EXCEPTION_HANDLER(_rt_hw_context_switch)

/* ---[ 0xd00: Trap exception ]------------------------------------------ */
	.org	0xd00

/* ---[ 0xe00: Trap exception ]------------------------------------------ */
	.org	0xe00

/* ---[ 0xf00: Reserved exception ]-------------------------------------- */
	.org	0xf00

/* ---[ 0x1000: Reserved exception ]------------------------------------- */
	.org	0x1000

/* ---[ 0x1100: Reserved exception ]------------------------------------- */
	.org	0x1100

/* ---[ 0x1200: Reserved exception ]------------------------------------- */
	.org	0x1200

/* ---[ 0x1300: Reserved exception ]------------------------------------- */
	.org	0x1300

/* ---[ 0x1400: Reserved exception ]------------------------------------- */
	.org	0x1400

/* ---[ 0x1500: Reserved exception ]------------------------------------- */
	.org	0x1500

/* ---[ 0x1600: Reserved exception ]------------------------------------- */
	.org	0x1600

/* ---[ 0x1700: Reserved exception ]------------------------------------- */
	.org	0x1700

/* ---[ 0x1800: Reserved exception ]------------------------------------- */
	.org	0x1800

/* ---[ 0x1900: Reserved exception ]------------------------------------- */
	.org	0x1900

/* ---[ 0x1a00: Reserved exception ]------------------------------------- */
	.org	0x1a00

/* ---[ 0x1b00: Reserved exception ]------------------------------------- */
	.org	0x1b00

/* ---[ 0x1c00: Reserved exception ]------------------------------------- */
	.org	0x1c00

/* ---[ 0x1d00: Reserved exception ]------------------------------------- */
	.org	0x1d00

/* ---[ 0x1e00: Reserved exception ]------------------------------------- */
	.org	0x1e00

/* ---[ 0x1f00: Reserved exception ]------------------------------------- */
	.org	0x1f00

/* ============================== [ entry ] ============================== */

	.section .text
_reset :
	l.movhi	r0, 0x0000	; #r0 = 0
	l.ori	r0, r0, 0x0000	;

	/* set up super mode */
	l.addi	r3, r0, SPR_SR_SM
	l.mtspr	r0, r3, SPR_SR

	/* Instruction cache enable */
	/* Check if IC present and skip enabling otherwise */
	l.mfspr	r24, r0, SPR_UPR
	l.andi	r26, r24, SPR_UPR_ICP
	l.sfeq	r26, r0
	l.bf	._enable_cache
	l.nop

	/* Disable IC */
	l.mfspr	r6, r0, SPR_SR
	l.addi	r5, r0, -1
	l.xori	r5, r5, SPR_SR_ICE
	l.and	r5, r6, r5
	l.mtspr	r0, r5, SPR_SR

	/* Establish cache block size
	   If BS=0, 16;
	   If BS=1, 32;
	   r14 contain block size
	 */
	l.mfspr	r24, r0, SPR_ICCFGR
	l.andi	r26, r24, SPR_ICCFGR_CBS
	l.srli	r28, r26, 7
	l.ori	r30, r0, 16
	l.sll	r14, r30, r28

	/* Establish number of cache sets
	   r16 contains number of cache sets
	   r28 contains log(# of cache sets)
	 */
	l.andi	r26, r24, SPR_ICCFGR_NCS
	l.srli	r28, r26, 3
	l.ori	r30, r0, 1
	l.sll	r16, r30, r28

	/* Invalidate IC */
	l.addi	r6, r0, 0
	l.sll	r5, r14, r28
._invalidate_icache_loop:
	l.mtspr	r0, r6, SPR_ICBIR
	l.sfne	r6, r5
	l.bf	._invalidate_icache_loop
	l.add	r6, r6, r14

	/* Enable IC */
	l.mfspr	r6, r0, SPR_SR
	l.ori	r6, r6, SPR_SR_ICE
	l.mtspr	r0, r6, SPR_SR
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop
	l.nop

._enable_cache:
	/* Data cache enable */
	/* Check if DC present and skip enabling otherwise */
	l.mfspr	r24, r0, SPR_UPR
	l.andi	r26, r24, SPR_UPR_DCP
	l.sfeq	r26, r0
	l.bf	._clear_bss
	l.nop
	
	/* Disable DC */
	l.mfspr	r6, r0, SPR_SR
	l.addi	r5, r0, -1
	l.xori	r5, r5, SPR_SR_DCE
	l.and	r5, r6, r5
	l.mtspr	r0, r5, SPR_SR
	
	/* Establish cache block size
	   If BS=0, 16;
	   If BS=1, 32;
	   r14 contain block size
	 */
	l.mfspr	r24, r0, SPR_DCCFGR
	l.andi	r26, r24, SPR_DCCFGR_CBS
	l.srli	r28, r26, 7
	l.ori	r30, r0, 16
	l.sll	r14, r30, r28
	
	/* Establish number of cache sets
	   r16 contains number of cache sets
	   r28 contains log(# of cache sets)
	*/
	l.andi	r26, r24, SPR_DCCFGR_NCS
	l.srli	r28, r26, 3
	l.ori	r30, r0, 1
	l.sll	r16, r30, r28
	
	/* Invalidate DC */
	l.addi	r6, r0, 0
	l.sll	r5, r14, r28
._invalidate_dcache_loop:
	l.mtspr	r0, r6, SPR_DCBIR
	l.sfne	r6, r5
	l.bf	._invalidate_dcache_loop
	l.add	r6, r6, r14
	
	/* Enable DC */
	l.mfspr	r6, r0, SPR_SR
	l.ori	r6, r6, SPR_SR_DCE
	l.mtspr	r0, r6, SPR_SR

._clear_bss:
	/* Clear BSS */
	LOAD_SYMBOL_TO_GPR(r28, ___bss_start)
	LOAD_SYMBOL_TO_GPR(r30, __end)

._clear_bss_loop:
	l.sw	0(r28), r0
	l.sfltu	r28, r30
	l.bf	._clear_bss_loop
	l.addi	r28, r28, 4

	/* Clear all registers */
	CLEAR_GPR(r1)
	CLEAR_GPR(r2)
	CLEAR_GPR(r3)
	CLEAR_GPR(r4)
	CLEAR_GPR(r5)
	CLEAR_GPR(r6)
	CLEAR_GPR(r7)
	CLEAR_GPR(r8)
	CLEAR_GPR(r9)
	CLEAR_GPR(r10)
	CLEAR_GPR(r11)
	CLEAR_GPR(r12)
	CLEAR_GPR(r13)
	CLEAR_GPR(r14)
	CLEAR_GPR(r15)
	CLEAR_GPR(r16)
	CLEAR_GPR(r17)
	CLEAR_GPR(r18)
	CLEAR_GPR(r19)
	CLEAR_GPR(r20)
	CLEAR_GPR(r21)
	CLEAR_GPR(r22)
	CLEAR_GPR(r23)
	CLEAR_GPR(r24)
	CLEAR_GPR(r25)
	CLEAR_GPR(r26)
	CLEAR_GPR(r27)
	CLEAR_GPR(r28)
	CLEAR_GPR(r29)
	CLEAR_GPR(r30)
	CLEAR_GPR(r31)

	/* Clear timer  */
	l.mtspr	r0, r0, SPR_TTMR

	/* Early Stack initilization */
	LOAD_SYMBOL_TO_GPR(r1, _stack)

	/* Jump to start of program */
	l.jal	rtthread_startup

_align:
	l.ori	r3, r0, 0x600
	l.nop	NOP_REPORT
	l.mfspr	r3, r0, SPR_EPCR_BASE
	l.nop	NOP_REPORT
	l.lwz	r3, 0(r3)
	l.nop	NOP_REPORT
	l.mfspr	r3, r0, SPR_EEAR_BASE
	l.nop	NOP_REPORT
	/* Loop in place, cause simulator to exit */
	l.ori	r3, r0, 1
	l.j	0
	l.nop	NOP_EXIT

/*
------------------------------
	    Task Switch
------------------------------
 Description :
 This routine switches between two different tasks.
 The task state of one is saved on its kernel stack.
 Then the state of the other is restored from its kernel stack.

 There maybe memory management hardware issues

 Finally, we can return to the second task, via the 'return'.
------------------------------
 Uses:
	void rt_hw_context_switch(rt_uint32 from, rt_uint32 to);
	r3 --> from
	r4 --> to
------------------------------
*/

/* task switch function */
_rt_hw_context_switch:
	/* l.sys exception for now so we are in supervisor mode */
	/* exception  - recover pc from epcr */

	l.mfspr	r3, r0, SPR_EPCR_BASE	/* save program counter that was put in exception register */
	l.sw	PT_PC(r1), r3

	l.mfspr	r3, r0, SPR_ESR_BASE	/* save status register that was put in exception register */
	l.sw	PT_SR(r1), r3

	STORE_CONTEXT

	/* Store current stack pointer   */
	LOAD_SYMBOL_TO_GPR(r3, rt_interrupt_from_thread)
	l.lwz   r3, 0(r3)
	l.sw	0(r3), r1		/* from->sp = r1 */

	/* Load next stack pointer */
	LOAD_SYMBOL_TO_GPR(r4, rt_interrupt_to_thread)
	l.lwz   r4, 0(r4)
	l.lwz	r1, 0(r4)		/* r1 = to->sp */

	l.lwz	r2, PT_PC(r1)		/* load context for task to be resumed */
	l.mtspr	r0, r2, SPR_EPCR_BASE

	l.lwz	r2, PT_SR(r1)
	l.mtspr	r0, r2, SPR_ESR_BASE

	RESTORE_CONTEXT

	l.addi	r1, r1, STK_FRAME_SIZE
	l.rfe
	l.nop

/*
------------------------------
	Start First Task
------------------------------
 Description :
   Starts the highest priority task that is available to run

 raw_start_first_task() MUST:
  a) Call raw_task_switch_hook() then, 
  b) Set OSRunning to TRUE, 
  c) Switch to the HPT

------------------------------
 Uses :
	void rt_hw_context_switch_to(rt_uint32_t to);
	r3 --> to
------------------------------
*/
rt_hw_context_switch_to:
	/* Load first task stack pointer */
	l.lwz	r1, 0(r3)                /* r1 = to->sp */

	l.lwz	r2, PT_PC(r1)                /* load context for task to be resumed */
	l.mtspr	r0, r2, SPR_EPCR_BASE
	l.lwz	r2, PT_SR(r1)
	l.mtspr	r0, r2, SPR_ESR_BASE

	RESTORE_CONTEXT

	l.addi	r1, r1, STK_FRAME_SIZE
	l.rfe
	l.nop

/*
------------------------------
	TickISR
------------------------------
 Description :

------------------------------
 Uses :
------------------------------
*/
_timer_isr:
	l.mfspr	r3, r0, SPR_EPCR_BASE     /* save program counter that was put in exception register */
	l.sw	PT_PC(r1), r3
	l.mfspr	r3, r0, SPR_ESR_BASE      /* save status register that was put in exception register */
	l.sw	PT_SR(r1), r3

	STORE_CONTEXT

	/* Enter interrupt */
	l.jal	rt_interrupt_enter
	l.nop

	/* Timer interrupt handler */
	l.jal	rt_hw_timer_handler
	l.nop

	/* Leave interrupt */
	l.jal	rt_interrupt_leave
	l.nop

	LOAD_SYMBOL_TO_GPR(r3, rt_thread_switch_interrupt_flag);	/* r3 = &rt_thread_switch_interrupt_flag */
	l.lbz	r3, 0(r3)             /* r3 = rt_thread_switch_interrupt_flag */

	/* if (rt_thread_switch_interrupt_flag == 1) */
	l.sfeqi	r3, 1
	l.bf	timer_switch_interrupt_do
	l.nop

	l.lwz	r2, PT_PC(r1)		/* load context for task to be resumed */
	l.mtspr	r0, r2, SPR_EPCR_BASE
	l.lwz	r2, PT_SR(r1)
	l.mtspr	r0, r2, SPR_ESR_BASE

	/* if (rt_thread_switch_interrupt_flag != 1), return from timer interrupt */
	RESTORE_CONTEXT

	l.addi	r1, r1, STK_FRAME_SIZE
	l.rfe
	l.nop

timer_switch_interrupt_do:
	/* clear rt_thread_switch_interrupt_flag*/
	LOAD_SYMBOL_TO_GPR(r3, rt_thread_switch_interrupt_flag)
	l.sb	0(r3), r0

	/* load from_thread and store it */
	LOAD_SYMBOL_TO_GPR(r3, rt_interrupt_from_thread);	/* r3 = rt_interrupt_from_thread */
	l.lwz	r3, 0(r3)		/* r3 = &rt_interrupt_from_thread->sp */
	l.sw	0(r3), r1		/* from --> sp = r1 */

	/* load to_thread and restore it */
	LOAD_SYMBOL_TO_GPR(r4, rt_interrupt_to_thread);	/* r3 = rt_interrupt_to_thread */
	l.lwz	r4, 0(r4)
	l.lwz	r1, 0(r4)		/* r1 = to --> sp */

	l.lwz	r2, PT_PC(r1)		/* load context for task to be resumed */
	l.mtspr	r0, r2, SPR_EPCR_BASE
	l.lwz	r2, PT_SR(r1)
	l.mtspr	r0, r2, SPR_ESR_BASE

	RESTORE_CONTEXT

	l.addi	r1, r1, STK_FRAME_SIZE
	l.rfe
	l.nop

/*
------------------------------
	_usr_isr
------------------------------
 Description :

------------------------------
 Uses :
------------------------------
*/
_usr_isr:
	l.mfspr	r3, r0, SPR_EPCR_BASE     /* save program counter that was put in exception register */
	l.sw	PT_PC(r1), r3
	l.mfspr	r3, r0, SPR_ESR_BASE      /* save status register that was put in exception register */
	l.sw	PT_SR(r1), r3

	STORE_CONTEXT

	/* Enter interrupt */
	l.jal	rt_interrupt_enter
	l.nop

	/* User interrupt handler */
	l.jal	rt_hw_trap_irq
	l.nop

	/* Leave interrupt*/
	l.jal	rt_interrupt_leave
	l.nop

	LOAD_SYMBOL_TO_GPR(r3, rt_thread_switch_interrupt_flag);	/* r3 = &rt_thread_switch_interrupt_flag */
	l.lbz	r3, 0(r3)	/* r3 = rt_thread_switch_interrupt_flag */

	/* if (rt_thread_switch_interrupt_flag == 1) */
	l.sfeqi	r3, 1
	l.bf	isr_switch_interrupt_do
	l.nop

	/* if (rt_thread_switch_interrupt_flag != 1), return from timer interrupt */
	RESTORE_CONTEXT

	l.addi	r1, r1, STK_FRAME_SIZE
	l.rfe
	l.nop

isr_switch_interrupt_do:
	/* clear rt_thread_switch_interrupt_flag*/
	LOAD_SYMBOL_TO_GPR(r3, rt_thread_switch_interrupt_flag)
	l.sb	0(r3), r0

	/* load from_thread and store it */
	LOAD_SYMBOL_TO_GPR(r3, rt_interrupt_from_thread);	/* r3 = rt_interrupt_from_thread */
	l.lwz	r3, 0(r3)		/* r3 = &rt_interrupt_from_thread->sp */
	l.sw	0(r3), r1		/* from --> sp = r1 */

	/* load to_thread and restore it */
	LOAD_SYMBOL_TO_GPR(r4, rt_interrupt_to_thread);	/* r3 = rt_interrupt_to_thread */
	l.lwz	r4, 0(r4)
	l.lwz	r1, 0(r4)		/* r1 = to --> sp */

	l.lwz	r2, PT_PC(r1)		/* load context for task to be resumed */
	l.mtspr	r0, r2, SPR_EPCR_BASE
	l.lwz	r2, PT_SR(r1)
	l.mtspr	r0, r2, SPR_ESR_BASE

	RESTORE_CONTEXT

	l.addi	r1, r1, STK_FRAME_SIZE
	l.rfe
	l.nop
