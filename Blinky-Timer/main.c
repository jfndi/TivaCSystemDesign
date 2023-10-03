/*
 * Project Blinky-Timer
 *	main.c
 *
 *  Created on: Oct 3, 2023
 *      Author: Jean-François Ndi
 *
 * Objective:
 * ==========
 *  The main objective of this experiment is to toggle an LED by configuring
 *  the timer interrupt of the TM4C123GH6PM micro-controller. The Timer is
 *  configured in periodic mode, the timer load register is loaded with a
 *  preset value and the timer count is decremented for each clock cycle. When
 *  the timer count reaches zero, an interrupt is generated. On each interrupt,
 *  the processor reads the current status of the LED connected to a GPIO port
 *  and toggles it.
 *
 * Steps:
 * ======
 *  The software written in C configures and enables the system clock to 40MHz.
 *  It the enables the GPIO Port F and configures pin 3 (PF3) connected to the
 *  green LED as output. The timer is configured in periodic mode and the timer
 *  interrupt is enabled. On interrupt in periodic mode, the timer interrupt
 *  service routine (ISR) reads the GPIO pin connected to the LED. If the
 *  current status is HIGH, the processor sends a LOW and vice-versa.
 *
 * Calculation of timer period ui32Period:
 * =======================================
 *  The number of timer counts required to obtain a given frequency is given by:
 *      Number of clock cycles = System Clock Frequency / Desired Frequency.
 *  We want to toggle the LED at 10HZ an 50% duty cycle.
 *      ui32Period = Number of clock cycles * Duty cycle = 2.10^6.
 */
#include <stdint.h>
#include <stdbool.h>

#include <inc/tm4c123gh6pm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>

#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/interrupt.h>
#include <driverlib/gpio.h>
#include <driverlib/timer.h>
#include <driverlib/uart.h>

#include <utils/uartstdio.h>

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

void Timer0IntHandler(void)
{
    /*
     * Clear the timer interrupt.
     */
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

    /*
     * Read the current state of the GPIO pin and write
     * back the opposite state.
     */
    if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3))
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0);
    else
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8);
}

int main(void)
{
    uint32_t ui32Period;

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
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    /*
     * Enable the Timer peripheral.
     */
    UARTprintf("---->> Enable the Timer peripheral.\n");
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    /*
     * Configure the timer in Periodic mode.
     */
    UARTprintf("---->> Configure the timer is Periodic mode.\n");
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

    /*
     * Register the timer interrupt.
     */
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);

    /*
     * Set the toggle frequency to 10HZ
     */
    UARTprintf("---->> Set the toggle frequency to 10HZ.\n");
    ui32Period = (SysCtlClockGet() / 10) / 2;
    TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

    /*
     * Enable interrupts globally.
     */
    UARTprintf("---->>  Enable interrupts globally.\n");
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    IntMasterEnable();
    TimerEnable(TIMER0_BASE, TIMER_A);

    /*
     * Enters the busy empty loop.
     */
    while (1) {}
}
