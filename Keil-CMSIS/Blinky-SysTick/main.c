#undef __FPU_PRESENT
#include <TM4C123GH6PM.h>

#include "bsp.h"
#include "gpio.h"

int main(void)
{
    /*
     * Enable clock-gating for I-O port F.
     */
    SYSCTL->RCGC2 |= (1U << 5);
    
    /*
     * Use the high performance bus for GPIOF.
     */
    SYSCTL->GPIOHBCTL |= (1U << 5);
    
    /*
     * Set all LEDs as output.
     */
    GPIOF_AHB->DIR |= LED_ALL;
    
    /*
     * Enable signals on all LED pins.
     */
    GPIOF_AHB->DEN |= LED_ALL;
    
    /*
     * Set the system tick reload value.
     */
    SysTick->LOAD = SYS_CLOCK_HZ / 2U - 1;
    
    /*
     * Set the current system tick value;
     */
    SysTick->VAL = 0;
    
    /*
     * System control source | Interrupt enabled | Counter enabled.
     */
    SysTick->CTRL = (1U << 2)|(1U << 1)|1;
    
    /*
     * Enable interrupts.
     */
    __enable_irq();
    
    while(1) {}
}
