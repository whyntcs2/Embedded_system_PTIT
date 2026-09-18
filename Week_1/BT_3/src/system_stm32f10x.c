#include "stm32f10x.h"

/*
 * The assignment only uses GPIO, so the reset-default HSI clock is sufficient.
 * The startup file calls SystemInit() before main().
 */
void SystemInit(void)
{
    /* Keep the reset-default clock configuration (HSI). */
}
