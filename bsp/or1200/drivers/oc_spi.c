#include "oc_spi.h"

static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration);
static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message);

static struct rt_spi_ops oc_spi_ops =
{
    configure,
    xfer
};

static rt_err_t configure(struct rt_spi_device* device, struct rt_spi_configuration* configuration)
{
    struct oc_spi_bus * oc_spi_bus = (struct oc_spi_bus *)device->bus;
    rt_uint32_t spi_reg_ctrl;
    rt_uint32_t spi_reg_div;

    spi_reg_ctrl = 0;

    /* SCLK: divider register is 16-bit width */
    spi_reg_div = (SYS_CLK / (2 * configuration->max_hz) - 1) & 0xFFFF; 

    /* write back the real sclk value */
    configuration->max_hz = SYS_CLK / ((spi_reg_div + 1) * 2);

    REG32(oc_spi_bus->spi_base_addr + OC_SPI_REG_DIVIDER) = spi_reg_div;

    /* Tx_NEG */
    spi_reg_ctrl |= 
        ( 
            (!(configuration->mode & RT_SPI_CPOL)) !=
            (!(configuration->mode & RT_SPI_CPHA))
        ) ? OC_SPI_CTRL_TX_NEG : 0;

    /* Rx_NEG */
    spi_reg_ctrl |= 
        ( 
            (!(configuration->mode & RT_SPI_CPOL)) ==
            (!(configuration->mode & RT_SPI_CPHA))
        ) ? OC_SPI_CTRL_RX_NEG : 0;
    
    /* MSB or LSB */
    spi_reg_ctrl |= (configuration->mode & RT_SPI_MSB) ? 0 : OC_SPI_CTRL_LSB;

    oc_spi_bus->spi_reg_ctrl = spi_reg_ctrl;

    return RT_EOK;
};

static rt_uint32_t xfer(struct rt_spi_device* device, struct rt_spi_message* message)
{
    struct oc_spi_bus * oc_spi_bus;
    struct oc_spi_cs * oc_spi_cs;
    rt_uint32_t size;
    rt_uint32_t spi_base_addr;

    const rt_uint8_t * send_ptr;
    rt_uint8_t * recv_ptr;
    
    int i;
    int j;

    oc_spi_bus = (struct oc_spi_bus *)device->bus;
    oc_spi_cs = device->parent.user_data;

    size = message->length;
    spi_base_addr = oc_spi_bus->spi_base_addr;

    /* take CS */
    if(message->cs_take)
    {
        REG32(spi_base_addr + OC_SPI_REG_SS) = (rt_uint32_t)oc_spi_cs->slave_sel;
    }

    send_ptr = message->send_buf;
    recv_ptr = message->recv_buf;

    send_ptr = &send_ptr[size - 1];
    recv_ptr = &recv_ptr[size - 1];
    for (i = 0;i < size;i+=16) {
        rt_uint32_t send_len;

        /* send as much as we can (max = 128-bits) */
        if ((i + 16) > size)
            send_len = size - i;
        else
            send_len = 16;

        for (j = 0;j < send_len;j++) {
            REG8(spi_base_addr + OC_SPI_REG_TX + (j & 0x0c) + (3 - (j & 0x3))) = *send_ptr;
	    --send_ptr;
        }

        REG32(spi_base_addr + OC_SPI_REG_CTRL) = 
            oc_spi_bus->spi_reg_ctrl |
            (OC_SPI_CTRL_CHAR_LEN_MASK & (send_len << 3)) | /* bits len */
            OC_SPI_CTRL_GO_BSY/* transmit start */;
        
        /* wait until it finished */
        while(REG32(spi_base_addr + OC_SPI_REG_CTRL) & OC_SPI_CTRL_GO_BSY);

        if(recv_ptr != RT_NULL)
        {
            for (j = 0;j < send_len;j++) {
                *recv_ptr = REG8(spi_base_addr + OC_SPI_REG_RX + (j & 0x0c) + (3 - (j & 0x3)));
	        --recv_ptr;
            }
        }
    }

    /* release CS */
    if(message->cs_release)
    {
        REG32(spi_base_addr + OC_SPI_REG_SS) = 0x00;
    }

    return message->length;
};

/** \brief init and register oc spi bus.
 *
 * \param SPI device base address.
 * \param oc_spi: oc spi bus struct.
 * \param spi_bus_name: spi bus name, e.g: "spi1"
 * \return
 *
 */
rt_err_t oc_spi_register(
    rt_uint32_t spi_base_addr,
    struct oc_spi_bus * oc_spi,
    const char * spi_bus_name
)
{
    oc_spi->spi_base_addr = spi_base_addr;

    return rt_spi_bus_register(&oc_spi->parent, spi_bus_name, &oc_spi_ops);
}
