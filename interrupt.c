//==================== Includes ====================//
#include <lpc21xx.h>
#include "defines.h"
#include "Interrupts.h"
#include "types.h"

//==================== External Flags ====================//
extern u32 eint0_flag, eint1_flag;   // Defined in main file
//============================================================//
//            ENABLE EINT0
//============================================================//
void Enable_EINT0(void)
{
    CFGPIN(PINSEL0, 1, FUNC4);      // Configure pin as EINT0

    SSETBIT(VICIntEnable, 14);      // Enable EINT0 in VIC

    VICVectCntl0 = 0x20 | 14;       // Enable slot 0 with IRQ number 14
    VICVectAddr0 = (unsigned)eint0_isr;  // Set ISR address

    SCLRBIT(EXTINT, 0);             // Clear pending interrupt

    SETBIT(EXTMODE, 0);             // Edge sensitive mode
    SETBIT(EXTPOLAR, 0);            // Falling edge trigger
}
//============================================================//
//            ENABLE EINT1
//============================================================//
void Enable_EINT1(void)
{
    CFGPIN(PINSEL0, 3, FUNC4);      // Configure pin as EINT1

    SSETBIT(VICIntEnable, 15);      // Enable EINT1 in VIC

    VICVectCntl1 = 0x20 | 15;       // Enable slot 1 with IRQ number 15
    VICVectAddr1 = (unsigned)eint1_isr;  // Set ISR address

    SCLRBIT(EXTINT, 1);             // Clear pending interrupt

    SETBIT(EXTMODE, 1);             // Edge sensitive mode
    SETBIT(EXTPOLAR, 1);            // Rising edge trigger
}
//============================================================//
//            EINT0 ISR
//============================================================//
void eint0_isr(void) __irq
{
    eint0_flag++;        // Increment left interrupt counter
    eint1_flag = 0;      // Reset right flag

    SSETBIT(EXTINT, 0);  // Clear EINT0 interrupt flag

    VICVectAddr = 0;     // Dummy write to clear VIC interrupt
}
//============================================================//
//            EINT1 ISR
//============================================================//
void eint1_isr(void) __irq
{
    eint1_flag++;        // Increment right interrupt counter
    eint0_flag = 0;      // Reset left flag

    SSETBIT(EXTINT, 1);  // Clear EINT1 interrupt flag

    VICVectAddr = 0;     // Dummy write to clear VIC interrupt
}
