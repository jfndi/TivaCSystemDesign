/*
 * Project HibernateWakeup
 *	main.c
 *
 *  Created on: Oct 3, 2023
 *      Author: Jean-François Ndi
 *
 * Objective:
 * ==========
 *  The main objective of this experiment is to understand and configure the
 *  Hibernation module of the TM4C123GH6PM device to place the device in a low
 *  power state and then wake up the device on RTC interrupt.
 *  The TM4C123GH6PM consists of a battery-backed hibernation module that
 *  provides logic to switch power off to the main processor while the
 *  processor is idle. In this experiment, the hibernation module is turned on
 *  by the software code. This will be indicated by the green LED connected to
 *  a GPIO port (PF3). When the LED is ON, the device is in wake up mode and
 *  when the LED is OFF the device is in hibernate mode. The hibernation can be
 *  woken up by two ways:
 *      - An external signal, SW2 input to the GPIO wake up pin
 *      - RTC wake up
 *  In this experiment, the RTC is used to wake up the processor after 5
 *  seconds.
 * Steps:
 * ======
 *  The C program code enables and configure the system clcok to 40MHz. The
 *  green LED indicates if the device is in hibernate mode or wake up mode.
 *  For this purpose, the GPIO port F is enabled and pin 3 is configured as
 *  output.
 *  The C code then enables the Hibernation module and configures the clock
 *  that feeds the module. It enables the GPIO pin state to be retained during
 *  hibernation and provides a 4-seconds delay for the user to observe the LED
 *  which is on. In hibernation, the processor uses the clock from the external
 *  crystal or oscillator. Hence the clock source used in hibernation mode is
 *  16MHz.
 *  The program configures the device to be woken up either by an inpu on the
 *  wake-up pin or by the RTC. It sets the RTC wake up parameters and turns
 *  the RTC on. The wake-up time is set to 5 seconds. The wake-up pin is
 *  connected to SW2 on the board. When SW2 is pressed and held, the device is
 *  woken up from hibernation.
 */
#include <stdint.h>
#include <stdbool.h>

#include <utils/ustdlib.h>
#include <utils/uartstdio.h>

#include <inc/hw_types.h>
#include <inc/hw_memmap.h>

#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/debug.h>
#include <driverlib/hibernate.h>
#include <driverlib/gpio.h>
#include <driverlib/uart.h>

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
     * Wait until GPIO A is ready.
     */
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA))
    {}

    /*
     * Enable UART0.
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /*
     * Wait until UART 0 is ready.
     */
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    {}

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
    UARTprintf("---->> Configured clock rate %d.\n", SysCtlClockGet());

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
    UARTprintf("---->> Enable GPIO F.\n");
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    /*
     * Here we check register 108 (using the same bit band technique) until it
     * indicates that the GPIIOF is ready.
     */
    UARTprintf("---->> Wait for GPIO F to be ready.\n");
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {}

    /*
     * Set the green LED as output.
     */
    UARTprintf("---->> Set the green LED as output.\n");
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    /*
     * Turn the green LED ON.
     */
    UARTprintf("---->> Turn the green LED ON.\n");
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x08);

    /*
     * Enable the Hibernation peripheral.
     */
    UARTprintf("---->> Enable the Hibernation peripheral.\n");
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);

    /*
     * Enable Hibernation module for operation.
     */
    UARTprintf("---->> Enable Hibernation module for operation.\n");
    HibernateEnableExpClk(SysCtlClockGet());

    /*
     * Enable GPIO retention after wake from hibernation.
     */
    UARTprintf("---->> Enables GPIO retention after wake from hibernation.\n");
    HibernateGPIORetentionEnable();

    /*
     * Wait for 4 seconds.
     */
    UARTprintf("---->> Wait for 4 seconds.\n");
    SysCtlDelay(64000000);

    /*
     * Set the value of the real time clock (RTC) counter.
     */
    UARTprintf("---->> Set the value of the real time clock (RTC) counter.\n");
    HibernateRTCSet(0);

    /*
     * Enable the RTC feature of the Hibernation module.
     */
    UARTprintf("---->> Enable the RTC feature of the Hibernation module.\n");
    HibernateRTCEnable();

    /*
     * Set the value of the RTC match register.
     */
    UARTprintf("---->> Set the value of the RTC match register.\n");
    HibernateRTCMatchSet(0, 5);

    /*
     * Configure the wake conditions for the Hibernation module.
     */
    UARTprintf("---->> Configure the wake conditions for the Hibernation module.\n");
    HibernateWakeSet(HIBERNATE_WAKE_PIN|HIBERNATE_WAKE_RTC);

    /*
     * Turn the green LED OFF.
     */
    UARTprintf("---->> Turn the green LED OFF.\n");
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);

    /*
     * Request hibernation mode.
     */
    UARTprintf("---->> Request hibernation mode.\n");
    HibernateRequest();

    while (1) {}
}

