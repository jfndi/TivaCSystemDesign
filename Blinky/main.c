/*
 * Project Blinky
 *	main.c
 *
 *  Created on: Oct 1, 2023
 *      Author: Jean-François Ndi
 *
 * Objective:
 * ==========
 *  The main objective of this experiment is to configure the Tiva GPIO pins to
 *  blink the green on-board LED, connected to PF, using a C program.
 *
 * Additional information:
 * =======================
 *  The EK-TM4C123GXL has three on-board LEDs which are connected to the GPIO
 *  PF1, PF2 and PF3 of the TM4C123GH6PM micro-controller. The software toggles
 *  the PF3 output at fixed time interval computed within the code. A HIGH on
 *  PF3 turns the LED ON.
 *
 *  The TM4C123GH6PM features a GPIO module that is composed of six physical
 *  GPIO blocks. Each block corresponds to an individual GPIO port (Port A,
 *  Port B, Port C, Port D, Port E and Port F).
 *
 * Calculation of Delay:
 * =====================
 *  The number of counts required to get a specific time delay is given by:
 *      Number of Counts = Time delay required * System Clock Frequency
 *  For a time delay of 500ms and a frequency of 40MHz:
 *      Number of Counts = 20*10^6.
 */
#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_types.h"
#include "inc/hw_memmap.h"

#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/rom.h"

#include "utils/uartstdio.h"

/*
 * Configure the UART and its pins. This must be called before UARTprintf().
 */
void configureUART(void)
{
    /*
     * Enable the GPIO peripheral used by the UART.
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*
     * Enable UART0.
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /*
     * Configure GPIO pins for UART mode.
     */
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1);

    /*
     * Use the internal 16MHZ oscillator as the UART clock source.
     */
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    /*
     * Initialize the UART for console I/O.
     */
    UARTStdioConfig(0, 9600, 16000000);
}

int main(void)
{
    /*
     * Configure the system clock.
     */
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|
                   SYSCTL_OSC_MAIN);

    /*
     * Initialize the UART.
     */
    configureUART();
    UARTprintf("--->> Configured clock rate %d.\n", SysCtlClockGet());

    /*
     * Here we enable GPIOF run mode clock gating by setting bit 5 of register
     * 60.
     *
     * In order to achieve that goal, the function uses a macro that essentially
     * implement the following algorithm. More info in the data sheet.
     *
     * - bit_word_offset = (byte_offset * 32) + (bit_number * 4)
     * - bit_word_addr = bit_band_base + bit_word_offset.
     *
     * In our case the bit band base address is 0x42000000, the byte offset is
     * 0x400FE608 - 0x40000000 = 0xEF608 and the bit number is 5.
     *
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    /*
     * Here we check register 108 (using the same bit band technique) until it
     * indicates that the GPIIOF is ready.
     */
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {}

    /*
     * Set the green LED as output.
     */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    /*
     * Endless loop.
     */
    while (1)
    {
        /*
         * Green LED ON.
         */
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08);

        /*
         * Delay.
         */
        SysCtlDelay(20000000);

        /*
         * Green LED OFF.
         */
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);

        /*
         * Delay.
         */
        SysCtlDelay(20000000);
    }
}
