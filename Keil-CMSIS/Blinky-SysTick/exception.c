#include <TM4C123GH6PM.h>

#include "bsp.h"

/*
 * We override the exception handlers (They are defined with a weak linkage in
 * startup_TM4C123.s). Why? Because the default implementation is an infinite
 * tight loop. This means that when the corresponding exception is called the
 * device become completely locked up, incurring a denial of service. The new
 * implementation calls assert_failed which internally calls NVIC_SystemReset()
 * giving the system a chance to "recover".
 * Additionally, the exception handlers are defined as __NO_RETURN. This ensures
 * that the stack is not used at the exception entry avoiding nested infinite
 * exceptions.
 */

void Unused_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);

__NO_RETURN void NMI_Handler(void)
{
    assert_failed("NMI", __LINE__);
}

__NO_RETURN void HardFault_Handler(void)
{
    assert_failed("HardFault", __LINE__);
}

__NO_RETURN void MemManage_Handler(void)
{
    assert_failed("MemMange", __LINE__);
}

__NO_RETURN void BusFault_Handler(void)
{
    assert_failed("BusFault", __LINE__);
}

__NO_RETURN void UsageFault_Handler(void)
{
    assert_failed("UsageFault", __LINE__);
}

__NO_RETURN void Unused_Handler(void)
{
    assert_failed("Unused", __LINE__);
}
