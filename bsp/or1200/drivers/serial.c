#include <rthw.h>
#include <rtthread.h>

#include "support.h"
#include "board.h"
#include "serial.h"

#include "spr-defs.h"

/**
 * @addtogroup OR1200
 */

/*@{*/

struct rt_ns16550
{
	struct rt_device parent;

	/* buffer for reception */
	rt_uint8_t read_index, save_index;
	rt_uint8_t rx_buffer[RT_UART_RX_BUFFER_SIZE];
}uart_device;

void rt_hw_serial_isr_1(int arg)
{

    /*clear pending interrupt flag*/
    mtspr(SPR_PICSR, mfspr(SPR_PICSR) & (~(1<<2)) );

    rt_base_t level;
    
    struct rt_ns16550* uart = &uart_device;
    

    unsigned int interrupt_id;
    interrupt_id = REG8(UART_BASE + UART_IIR);

    if ( interrupt_id & UART_IIR_RDI )/* Receive Line Status */
    {
	
		/* If no error on RLS, normal ready, save into the data buffer. */
		/* Note: read RBR will clear the interrupt */
		uart->rx_buffer[uart->save_index] = REG8(UART_BASE + UART_RX);
		level = rt_hw_interrupt_disable();
		uart->save_index ++;
		if (uart->save_index >= RT_UART_RX_BUFFER_SIZE)
	   		uart->save_index = 0;
		rt_hw_interrupt_enable(level);
		/* invoke callback */
		if(uart->parent.rx_indicate != RT_NULL)
		{
		    rt_size_t length;
		    if (uart->read_index > uart->save_index)
	                length = RT_UART_RX_BUFFER_SIZE - uart->read_index + uart->save_index;
	        else
	            length = uart->save_index - uart->read_index;
	
	        uart->parent.rx_indicate(&uart->parent, length);
	
		}
	}
   
	return;
}

/*****************************************************************************
** Function name:		rt_uart_init
** Descriptions:		
** parameters:			dev
** Returned value:		None
*****************************************************************************/
static rt_err_t rt_uart_init(rt_device_t dev)
{
    return RT_EOK;
}

static rt_err_t rt_uart_open(rt_device_t dev, rt_uint16_t oflag)
{
	RT_ASSERT(dev != RT_NULL);
	
    if(dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
	    /* Enable the UART Interrupt */
            rt_hw_interrupt_install(2, rt_hw_serial_isr_1,RT_NULL);
            /*enable receiver data interrupt*/
            REG8(UART_BASE + UART_IER) = UART_IER_RDI;
	}

	return RT_EOK;
}

static rt_err_t rt_uart_close(rt_device_t dev)
{
	RT_ASSERT(dev != RT_NULL);

	if (dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
	    /* Disable the UART Interrupt */
            rt_hw_interrupt_mask(2);
	        
        }

	return RT_EOK;
}

static rt_size_t rt_uart_read(rt_device_t dev, rt_off_t pos, void* buffer, rt_size_t size)
{
	rt_uint8_t* ptr;
	struct rt_ns16550 *uart = (struct rt_ns16550*)dev;
	RT_ASSERT(uart != RT_NULL);

	/* point to buffer */
	ptr = (rt_uint8_t*) buffer;
	if (dev->flag & RT_DEVICE_FLAG_INT_RX)
	{
		while (size)
		{
			/* interrupt receive */
			rt_base_t level;

			/* disable interrupt */
			level = rt_hw_interrupt_disable();
			if (uart->read_index != uart->save_index)
			{
				*ptr = uart->rx_buffer[uart->read_index];

				uart->read_index ++;
				if (uart->read_index >= RT_UART_RX_BUFFER_SIZE)
					uart->read_index = 0;
			}
			else
			{
				/* no data in rx buffer */

				/* enable interrupt */
				rt_hw_interrupt_enable(level);
				break;
			}

			/* enable interrupt */
			rt_hw_interrupt_enable(level);

			ptr ++;
			size --;
		}

		return (rt_uint32_t)ptr - (rt_uint32_t)buffer;
	}

	return 0;
}

static rt_size_t rt_uart_write(rt_device_t dev, rt_off_t pos, const void* buffer, rt_size_t size)
{
	char *ptr;
	ptr = (char*)buffer;
    unsigned char interrupt_id;
 
    interrupt_id = REG8(UART_BASE + UART_IIR);

	if (dev->flag & RT_DEVICE_FLAG_STREAM)
	{
		/* stream mode */
		while (size)
		{
			if (*ptr == '\n')
			{
				/* THRE status, contain valid data */
                                while (!( interrupt_id & UART_IIR_RDI ));
      
				/* write data */
				REG8(UART_BASE + UART_RX) = '\r';
			}

				/* THRE status, contain valid data */
         			   while (!( interrupt_id & UART_IIR_RDI ));
			/* write data */
			REG8(UART_BASE + UART_RX)= *ptr;
			
			ptr ++;
			size --;
		}
	}
	else
	{
    		while ( size != 0 )
    		{
			    /* THRE status, contain valid data */
         		   while (!( interrupt_id & UART_IIR_RDI ));
			    /* write data */
			    REG8(UART_BASE + UART_RX) = *ptr;

			    ptr++;
			    size--;
	    }
	}

	return (rt_size_t) ptr - (rt_size_t) buffer;
}

void rt_hw_serial_init(void)
{
	struct rt_ns16550* uart;

	/* get uart device */
	uart = &uart_device;

	/* device initialization */
	uart->parent.type = RT_Device_Class_Char;
	rt_memset(uart->rx_buffer, 0, sizeof(uart->rx_buffer));
	uart->read_index = uart->save_index = 0;

	/* device interface */
	uart->parent.init 	    = rt_uart_init;
	uart->parent.open 	    = rt_uart_open;
	uart->parent.close      = rt_uart_close;
	uart->parent.read 	    = rt_uart_read;
	uart->parent.write      = rt_uart_write;
	uart->parent.control    = RT_NULL;
	uart->parent.user_data  = RT_NULL;

	rt_device_register(&uart->parent,
		"uart1", RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_STREAM | RT_DEVICE_FLAG_INT_RX);
}

/******************************************************************************
**                            End Of File
******************************************************************************/
