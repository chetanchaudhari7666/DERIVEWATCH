//============================================================//
//                        ADC DRIVER
//============================================================//

#include <LPC21xx.h>

#include "types.h"
#include "pin_connect_block.h"
#include "pin_function_defines.h"
#include "adc_defines.h"
#include "delay.h"



//============================================================//
//                        INIT ADC
//============================================================//

void Init_ADC(void)
{
    // Configure ADCR:
    // - Power up ADC
    // - Set clock divider
    ADCR |= (1<<PDN_BIT) | (CLKDIV<<CLKDIV_BITS);

    // Configure P0.27 as AIN0 (Analog Input Channel 0)
    CfgPortPinFunc(0, 27, FUNC2);
}


//============================================================//
//                        READ ADC
//============================================================//

float Read_ADC(u32 chNo)
{
    float eAR;
    unsigned short int adcDVal;

    // Clear previous channel selection
    ADCR &= 0xFFFFFF00;

    // Select required channel
    // Start analog sampling & conversion
    ADCR |= (1<<chNo) | (1<<ADC_START_BIT);

    // Small acquisition delay
    delayUS(3);

    // Wait until conversion completes (DONE bit = 1)
    while(((ADDR >> DONE_BIT) & 1) == 0);

    // Stop conversion
    ADCR &= ~(1<<ADC_START_BIT);

    // Extract 10-bit result
    adcDVal = (ADDR >> RESULT_BITS) & 1023;

    // Convert to equivalent analog reading (assuming Vref = 3.3V)
    eAR = (adcDVal * (3.3 / 1023));

    return eAR;
}
