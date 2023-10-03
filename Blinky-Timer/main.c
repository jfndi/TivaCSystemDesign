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
 *  configured in periodic mode, the timer load regsiter is loaded with a
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
