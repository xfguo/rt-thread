#ifndef OC_SPI_H_INCLUDED
#define OC_SPI_H_INCLUDED

#include <rtdevice.h>

#include "board.h"
#include "support.h"

/* register defines */
#define OC_SPI_REG_RX               (0x00)
#define OC_SPI_REG_RX0              (0x00)
#define OC_SPI_REG_RX1              (0x04)
#define OC_SPI_REG_RX2              (0x08)
#define OC_SPI_REG_RX3              (0x0c)
#define OC_SPI_REG_TX               (0x00)
#define OC_SPI_REG_TX0              (0x00)
#define OC_SPI_REG_TX1              (0x04)
#define OC_SPI_REG_TX2              (0x08)
#define OC_SPI_REG_TX3              (0x0c)
#define OC_SPI_REG_CTRL             (0x10)
#define OC_SPI_REG_DIVIDER          (0x14)
#define OC_SPI_REG_SS               (0x18)

#define OC_SPI_CTRL_ASS             (1 << 13)
#define OC_SPI_CTRL_IE              (1 << 12)
#define OC_SPI_CTRL_LSB             (1 << 11)
#define OC_SPI_CTRL_TX_NEG          (1 << 10)
#define OC_SPI_CTRL_RX_NEG          (1 << 9)
#define OC_SPI_CTRL_GO_BSY          (1 << 8)
#define OC_SPI_CTRL_CHAR_LEN_MASK   (0x7f)

struct oc_spi_bus
{
    struct rt_spi_bus parent;
    rt_uint32_t spi_base_addr;

    /* SPI CTRL register with config value, when we send, just fill the 
     * GO_BSY & length.
     */
    rt_uint32_t spi_reg_ctrl;
};

struct oc_spi_cs
{
    rt_uint8_t slave_sel;
};

/* public function list */
rt_err_t oc_spi_register(
    rt_uint32_t spi_base_addr,
    struct oc_spi_bus * oc_spi,
    const char * spi_bus_name
);

#endif /* OC_SPI_H_INCLUDED */
