#include "support.h"
#include <rtthread.h>

/* For writing into SPR. */
void mtspr(rt_uint32_t spr, rt_uint32_t value)
{
  asm("l.mtspr\t\t%0,%1,0": : "r" (spr), "r" (value));
}
 
/* For reading SPR. */
rt_uint32_t mfspr(rt_uint32_t spr)
{
  rt_uint32_t value;
  asm("l.mfspr\t\t%0,%1,0" : "=r" (value) : "r" (spr));
  return value;
}
