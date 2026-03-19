//============================================================//
//                    MAIN APPLICATION
//        ADC ? Fuel % Calculation ? LCD ? CAN Tx
//============================================================//

#include <LPC21xx.h>

#include "delay.h"
#include "types.h"
#include "can.h"
#include "can_defines.h"
#include "adc.h"
#include "adc_defines.h"
#include "lcd.h"


//============================================================//
//                    ADC CALIBRATION VALUES
//============================================================//

#define ADC_EMPY   0.451f      // Empty tank voltage
#define ADC_FULL   3.390f      // Full tank voltage


//============================================================//
//                    GLOBAL VARIABLES
//============================================================//

f32 AR;                    // Analog Reading (Voltage)
u32 fuel_val = 0;          // Fuel Percentage

struct CAN_Frame txFrame;  // CAN Transmission Frame


//============================================================//
//                         MAIN
//============================================================//

int main(void)
{
    //------------- Peripheral Initialization -------------//
    Init_ADC();
    Init_CAN1();
    LCD_Init();


    //------------- Infinite Loop -------------//
    while(1)
    {
        //------------------------------------------------//
        //                READ ADC
        //------------------------------------------------//
        AR = Read_ADC(0);


        //------------------------------------------------//
        //        CALCULATE FUEL PERCENTAGE
        //------------------------------------------------//
        fuel_val = ((AR - 0.306) / (2.270 - 0.306)) * 100;


        //------------------------------------------------//
        //                DISPLAY ON LCD
        //------------------------------------------------//
        Write_CMD_LCD(0x80);        // Line 1
        Write_float_LCD(AR, 3);

        Write_CMD_LCD(0xC0);        // Line 2
        Write_int_LCD(fuel_val);
        Write_str_LCD("   ");       // Clear leftover digits


        //------------------------------------------------//
        //                SEND VIA CAN
        //------------------------------------------------//
        txFrame.ID = 1;
        txFrame.vbf.RTR = 0;
        txFrame.vbf.DLC = 4;

        txFrame.Data1 = fuel_val;

        CAN1_Tx(txFrame);
    }
}
