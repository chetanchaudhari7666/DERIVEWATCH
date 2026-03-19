//====================================================
//                MAIN_CANRX_TEST.C
//====================================================

#include <LPC21xx.h>
#include "can.h"
#include "defines.h"
#include "delay.h"
#include "types.h"
#include "can_defines.h"

#define LED 0

u32 left_indicator, right_indicator;


//====================================================
//              CHECK CAN RECEIVE
//====================================================

void check_can_receive(void)
{
    struct CAN_Frame rxFrame;              // Structure to store received CAN frame

    if (C1GSR & RBS_BIT_READ)              // Check if CAN receive buffer has new data
    {
        CAN1_Rx(&rxFrame);                 // Read received CAN frame into structure

        if (rxFrame.ID == 4)               // If message ID is 4 (Left indicator control)
        {
            right_indicator = 0;           // Turn OFF right indicator

            if (rxFrame.Data1 == 'L')      // If received data is 'L'
                left_indicator = 1;        // Turn ON left indicator
            else
                left_indicator = 0;        // Otherwise turn OFF left indicator
        }

        else if (rxFrame.ID == 5)          // If message ID is 5 (Right indicator control)
        {
            left_indicator = 0;            // Turn OFF left indicator

            if (rxFrame.Data1 == 'R')      // If received data is 'R'
                right_indicator = 1;       // Turn ON right indicator
            else
                right_indicator = 0;       // Otherwise turn OFF right indicator
        }
    }
}

//====================================================
//                      MAIN
//====================================================

int main()
{
    // LED running pattern values (active low pattern)
    u8 arr[8] = {0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe};
    S32 i;


    Init_CAN1(); // Initialize CAN1 peripheral    
    WRITEBYTE(IODIR0, LED, 0XFF);  // Configure LED port as output
    WRITEBYTE(IOPIN0, LED, 0XFF);  // Turn OFF all LEDs initially

    // Infinite loop
    while (1)
    {
        check_can_receive(); // Check if any CAN frame is received
			
        // If both indicators are OFF ? keep LEDs OFF
        if ((left_indicator == 0) && (right_indicator == 0))
        {
            WRITEBYTE(IOPIN0, LED, 0XFF);
            delayMS(100);
        }
        // If LEFT indicator is ON ? run LEDs left to right
        else if ((left_indicator == 1) && (right_indicator == 0))
        {
            for (i = 0; i < 8; i++)
            {
                WRITEBYTE(IOPIN0, LED, (arr[i]));  // Output current LED pattern
                check_can_receive();               // Continuously check CAN during animation
							
                if (left_indicator == 0)           // If LEFT signal turned OFF, stop animation
                    break;
                delayMS(50);
            }
        }
        // If both indicators are OFF (redundant check from original code)
        else if ((right_indicator == 0) && (left_indicator == 0))
        {
            WRITEBYTE(IOPIN0, LED, 0XFF);
            delayMS(100);
        }
        // If RIGHT indicator is ON ? run LEDs right to left
        else if ((left_indicator == 0) && (right_indicator == 1))
        {
            for (i = 7; i >= 0; i--)
            {
                WRITEBYTE(IOPIN0, LED, (arr[i]));    // Output current LED pattern
                check_can_receive();                 // Continuously check CAN during animation

                // If RIGHT signal turned OFF, stop animation
                if (right_indicator == 0)
                    break;
                delayMS(50);
            }
        }
    }
}
