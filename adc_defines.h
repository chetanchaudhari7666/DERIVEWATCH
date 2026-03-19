//============================================================//
//                CLOCK + ADC DEFINES
//============================================================//

#ifndef __ADC_DEFINES_H__
#define __ADC_DEFINES_H__


//============================================================//
//                      CLOCK SETTINGS
//============================================================//

#define FOSC   12000000UL        // 12 MHz crystal
#define CCLK   (FOSC * 5)        // 60 MHz CPU clock

// Assuming VPBDIV = 1
#define PCLK   (CCLK/4)             // Peripheral clock = 60 MHz


//============================================================//
//                      ADC CLOCK SETTINGS
//============================================================//

#define ADCCLK     3000000UL     // Must be <= 4.5 MHz
#define CLKDIV     ((PCLK / ADCCLK) - 1)


//============================================================//
//                      ADCR REGISTER BITS
//============================================================//

#define CLKDIV_BITS     8
#define PDN_BIT         21
#define ADC_START_BIT   24


//============================================================//
//                      ADDR REGISTER BITS
//============================================================//

#define RESULT_BITS     6
#define DONE_BIT        31


#endif
