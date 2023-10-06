#ifndef __BSP_H_
#define __BSP_H_

#define SYS_CLOCK_HZ 16000000U

#define LED_RED     (1U << 1)
#define LED_BLUE    (1U << 2)
#define LED_GREEN   (1U << 3)

#define LED_ALL     LED_RED|LED_BLUE|LED_GREEN

__attribute__((__noreturn__)) void assert_failed(char const* file, int line);
void SysTick_Handler(void);

#endif
