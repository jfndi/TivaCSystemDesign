#undef __FPU_PRESENT
#include <stdlib.h>
#include <TM4C123GH6PM.h>

#include "delay.h"
#include "led.h"
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
     * Turn ON the blue LED.
     */
    GPIOF_AHB->DATA_Bytes[LED_BLUE] = LED_BLUE;
    
    /*
     * Enter the endless loop.
     */
    while (1)
    {
        delay(1000000);
        
        /*
         * Turn the LED OFF.
         */
        GPIOF_AHB->DATA_Bytes[LED_BLUE] = 0;
        
        delay(500000);
        
        /*
         * Turn the LED ON.
         */
        GPIOF_AHB->DATA_Bytes[LED_BLUE] = LED_BLUE;
    }
}
