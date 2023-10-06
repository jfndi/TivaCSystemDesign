/*
 * Project Potentiometer
 *	main.c
 *
 *  Created on: Oct 5, 2023
 *      Author: Jean-François Ndi
 *
 * Objective:
 * ==========
 *  The main objective of this experiment is to interface a potentiometer with
 *  EK-TM4C123GXL GPIO (PE3) by configuring it as an Analog Input (AN0) and to
 *  observe its corresponding 12-bit digital value. We will also understand the
 *  Analog-to-Digital Conversion (ADC) and processing of the analog signals in
 *  a digital environment.
 * Steps:
 * ======
 *  The software controls the GPIO and ADC peripherals of the processor to
 *  convert the potentiometer input into digital values and store the converted
 *  12-bit digital values in an array variable. A temporary array variable
 *  ui32ADC0Value is initialized in the program for storing the 12-bit digital
 *  output of the ADC? The system clock is set to 40MHz and the GPIO port E
 *  configured and enabled as analog input of the ADC0 module which is used for
 *  A/D conversion. The ADC0 module is configured to interrupt on conversion
 *  and the enabled. The program the waits for an interrupt from the ADC0. The
 *  ADC0 module reads the analog input from the potentiometer and converts to
 *  digital output and interrupt the processor. On interrupt from the ADC0, the
 *  program enables the processor trigger event and stores the converted
 *  digital value in a temporary varaible. The process continues for the analog
 *  input from the potentiometer.
 */
#include <stdint.h>
#include <stdbool.h>

//#include <utils/ustdlib.h>
#include <utils/uartstdio.h>

#include <inc/hw_types.h>
#include <inc/hw_memmap.h>

#include <driverlib/sysctl.h>
#include <driverlib/pin_map.h>
#include <driverlib/debug.h>
#include <driverlib/gpio.h>
#include <driverlib/adc.h>
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

uint32_t ui32ADC0Value[1];

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
     * Initialize the ADC0 module.
     */
    UARTprintf("---->> Initialize the ADC0 module.\n");
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    /*
     * Enable the GPIO for the ADC0 module.
     */
    UARTprintf("---->> Enable the GPIO for the ADC0 module.\n");
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    /*
     * Enable AN0 of ADC0 module.
     */
    UARTprintf("---->> Enable AN0 of ADC0 module.");
    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

    /*
     * ADC0 module, trigger is processor event, sequencer 0.
     */
    UARTprintf("---->> ADC0 module, trigger is processor event, sequencer 0.\n");
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    /*
     * ADC0 module, sequencer 0, for 1 sampling, input form channel 0.
     */
    UARTprintf("---->> ADC0 module, sequencer 0, for 1 sampling, input form channel 0.\n");
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH0);

    /*
     * Enable the sequence 1 for ADC0.
     */
    UARTprintf("---->> Enable the sequence 1 for ADC0.\n");
    ADCSequenceEnable(ADC0_BASE, 1);

    while (1)
    {
        /*
         * Clear interrupt flag for ADC0, sequencer 1.
         */
        ADCIntClear(ADC0_BASE, 1);

        /*
         * Trigger is given for ADC0 module, sequencer 1.
         */
        ADCProcessorTrigger(ADC0_BASE, 1);

        /*
         * Store the converted value for all different sampling.
         */
        ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
    }
}
