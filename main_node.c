//==================== Includes ====================//
#include <lpc21xx.h>
#include "types.h"
#include "can.h"
#include "can_defines.h"
#include "lcd.h"
#include "delay.h"
#include "ds18b20.h"
#include "Interrupts.h"
#include "defines.h"
#include "lcd_defines.h"

//==================== Global Variables ====================//
volatile u32 eint0_flag, eint1_flag;   // External interrupt flags
u8 previous_value = 255;               // Store previous fuel value
#define LED 4                          // LED starting bit position

//==================== Main Function ====================//
int main(void)
{
    //==================== Local Variables ====================//
    S32 temp = 15;                     // Temperature variable
    unsigned char tp, tpd;              // Integer and decimal temp parts
    struct CAN_Frame txFrame, rxFrame;  // CAN frames

    //==================== CAN Configuration ====================//
    txFrame.vbf.RTR = 0;                // Data frame
    txFrame.vbf.DLC = 1;                // Data length = 1 byte
    Init_CAN1();                        // Initialize CAN1

    //==================== Interrupt Configuration ====================//
    Enable_EINT0();                     // Enable left switch interrupt
    Enable_EINT1();                     // Enable right switch interrupt

    //==================== LCD Initialization ====================//
    LCD_Init();                         // Initialize LCD
    Write_CMD_LCD(0x80);                // Select first line
    Write_CMD_LCD(GOTO_LINE1_POS0 + 5);
    Write_str_LCD("CAR INFO");          // Display title
    Write_str_LCD("");
    delayMS(1000);                      // Delay 1 sec
    Write_CMD_LCD(0xC0);                // Move to second line

    //==================== LED Configuration ====================//
    IODIR0 |= (0xF << LED);             // Configure LED pins as output
    IOPIN0 |= (0xF << LED);             // Turn OFF LEDs initially

    eint1_flag = 0;                     // Reset interrupt flag
    eint0_flag = 0;                     // Reset interrupt flag

    //==================== Infinite Loop ====================//
    while (1)
    {
        //==================== Temperature Reading ====================//
        temp = ReadTemp();              // Read temperature from DS18B20

        if (temp == -1)                 // Sensor failure condition
        {
            Write_CMD_LCD(0xC0);
            Write_str_LCD("                ");  // Clear line
            Write_CMD_LCD(0xC0);
            Write_str_LCD("Sensor fails");      // Display error
        }
        else
        {
            //==================== Temperature Display ====================//
            tp  = temp >> 4;                     // Integer part
            tpd = (temp & 0x08) ? 0x35 : 0x30;   // Decimal part (.5 or .0)

            Write_CMD_LCD(0xC0);
            Write_CMD_LCD(0xC0);
            Write_str_LCD("Tem=");               // Print label
            Write_int_LCD(tp);                   // Print integer
            Write_DAT_LCD('.');                  // Print dot
            Write_DAT_LCD(tpd);                  // Print decimal digit

            Write_CMD_LCD(GOTO_LINE2_POS0 + 8);
            Write_DAT_LCD(0xDF);                 // Degree symbol
            Write_str_LCD("C");                  // Celsius symbol

            //==================== LEFT Indicator (EINT0) ====================//
            if ((eint0_flag == 1) && (eint1_flag == 0))
            {
                txFrame.ID = 4;
                txFrame.Data1 = 'L';             // Send 'L'
                Write_CMD_LCD(GOTO_LINE1_POS0 + 1);
                Write_str_LCD("L");              // Display L
                CAN1_Tx(txFrame);                // Transmit CAN
            }
            else if (eint0_flag > 1)
            {
                eint0_flag = 0;                  // Reset flag
                Write_CMD_LCD(GOTO_LINE1_POS0 + 1);
                Write_str_LCD(" ");              // Clear L
                txFrame.ID = 4;
                txFrame.Data1 = 0;               // Send OFF
                CAN1_Tx(txFrame);
            }

            //==================== RIGHT Indicator (EINT1) ====================//
            else if ((eint1_flag == 1) && (eint0_flag == 0))
            {
                txFrame.Data1 = 'R';
                txFrame.ID = 5;
                Write_CMD_LCD(GOTO_LINE1_POS0 + 17);
                Write_str_LCD("R");              // Display R
                Write_str_LCD("   ");
                CAN1_Tx(txFrame);                // Transmit CAN
            }
            else if (eint1_flag > 1)
            {
                eint1_flag = 0;                  // Reset flag
                Write_CMD_LCD(GOTO_LINE1_POS0 + 17);
                Write_str_LCD(" ");              // Clear R
                txFrame.Data1 = 0;               // Send OFF
                txFrame.ID = 5;
                CAN1_Tx(txFrame);
            }
        }

        //==================== CAN Receive (Fuel Data) ====================//
        if (C1GSR & RBS_BIT_READ)               // Check receive buffer
        {
            CAN1_Rx(&rxFrame);                  // Receive frame

            if (rxFrame.ID == 1)                // Fuel ID
            {
                if (previous_value != rxFrame.Data1)
                {
                    previous_value = rxFrame.Data1;  // Update previous

                    Write_CMD_LCD(0xC0 + 4);
                    Write_CMD_LCD(GOTO_LINE2_POS0 + 11);
                    Write_str_LCD("FUEL:");

                    Write_CMD_LCD(GOTO_LINE2_POS0 + 16);
                    Write_str_LCD("   ");            // Clear old value

                    Write_CMD_LCD(GOTO_LINE2_POS0 + 16);
                    Write_int_LCD(rxFrame.Data1);    // Print fuel %

                    Write_CMD_LCD(GOTO_LINE2_POS0 + 19);
                    Write_str_LCD("%");              // Print %
                }
            }
        }
    }
}
