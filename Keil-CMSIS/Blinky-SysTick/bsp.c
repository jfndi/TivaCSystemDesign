/*
 * Board support package.
 */
#include <TM4C123GH6PM.h>
 
#include "bsp.h"
#include "gpio.h"

#define UNUSED_PARAMETER(X)  ((void)(X))

__NO_RETURN void assert_failed(char const* file, int line)
{   
    UNUSED_PARAMETER(file);
    UNUSED_PARAMETER(line);
    
    NVIC_SystemReset();
}

void SysTick_Handler(void)
{
    GPIOF_AHB->DATA_Bytes[LED_RED] ^= LED_RED;
}
