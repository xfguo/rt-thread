/* Linker script for OR1200 program */
OUTPUT_ARCH(or1k)
__DYNAMIC = 0;

/* Linking for loading into external RAM */
MEMORY    
{
	vectors	: ORIGIN = 0x00000000, LENGTH = 0x2000 - 0x100
	ram	: ORIGIN = 0x00002000, LENGTH = 0x20000 - 0x00002000
}

/*
  The following section defines where to put the different input sections.
  .text contains the code.
  .data contains the initialized data.
  .bss contains uninitialized data.
  .sdata contains small constant data.
*/

/* Stack information variables */
_min_stack      = 0x4000;   /* 16K - minimum stack space to reserve */

SECTIONS{	
	.vectors :
	{
		*(.vectors)
	} > vectors

	.text : {
		_stext = .;
		*(.text)
		_etext  =  .;
		__CTOR_LIST__ = .;
		LONG((__CTOR_END__ - __CTOR_LIST__) / 4 - 2)
		*(.ctors)
		LONG(0)
		__CTOR_END__ = .;
		__DTOR_LIST__ = .;
		LONG((__DTOR_END__ - __DTOR_LIST__) / 4 - 2)
		*(.dtors)
		LONG(0)
		__DTOR_END__ = .;
		*(.lit)
		*(.shdata)
		
		/* section information for finsh shell */
		. = ALIGN(4);
		__fsymtab_start = .;
		KEEP(*(FSymTab))
		__fsymtab_end = .;
		. = ALIGN(4);
		__vsymtab_start = .;
		KEEP(*(VSymTab))
		__vsymtab_end = .;
		. = ALIGN(4);
				
		_endtext = .;
	}  > ram

	.rodata : {
		*(.rodata);
		*(.rodata.*)
	} > ram
	
	.shbss :
	{
		*(.shbss)
	} > ram
	
	.talias :
	{ 
	}  > ram
	
	.data : {
		*(.data)
	} > ram

	.bss SIZEOF(.data) + ADDR(.data) :
	{
		___bss_start = ALIGN(0x8);
		*(.bss)
		*(COMMON)
		__end = ALIGN(0x8);
	} > ram
	
	/* ensure there is enough room for stack */
	.stack (NOLOAD): {
		. = ALIGN(4);
		. = . + _min_stack ;
		. = ALIGN(4);
		_stack = . ;
	} > ram
	
	.heap (NOLOAD): {
		. = ALIGN(4);
		__heap_start = .;
		. += 0x1000;
		__heap_end = .;
		. = ALIGN(4);
	} > ram

	.stab  0 (NOLOAD) : {
		[ .stab ]
	}
	
	.stabstr  0 (NOLOAD) :{
		[ .stabstr ]
	}
}
