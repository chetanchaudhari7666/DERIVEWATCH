//==================== Includes ====================//
#include <lpc21xx.h>
#include "types.h"
#include "can.h"
#include "can_defines.h"


//============================================================//
//            CAN1 INITIALIZATION
//============================================================//
void Init_CAN1(void)
{
    PINSEL1 |= RD1_PIN;          // Configure P0.25 as CAN1_RX (RD1)

    C1MOD = 1;                   // Enter Reset Mode

    AFMR = 2;                    // Accept all messages (bypass filter)

    C1BTR = BTR_LVAL;            // Set CAN baud rate

    C1MOD = 0;                   // Exit Reset Mode (Enable CAN1)
}


//============================================================//
//            CAN1 TRANSMIT FUNCTION
//============================================================//
void CAN1_Tx(struct CAN_Frame txFrame)
{
    // Wait until TX Buffer 1 is empty
    while ((C1GSR & TBS1_BIT_READ) == 0);

    C1TID1 = txFrame.ID;   // Load 11-bit CAN ID

    // Set RTR bit and DLC (Data Length Code)
    C1TFI1 = (txFrame.vbf.RTR << 30) | (txFrame.vbf.DLC << 16);

    // If Data Frame (not Remote Frame)
    if (txFrame.vbf.RTR != 1)
    {
        C1TDA1 = txFrame.Data1;  // Load first 4 data bytes
        C1TDB1 = txFrame.Data2;  // Load next 4 data bytes
    }

    // Select TX Buffer1 and Start Transmission
    C1CMR = STB1_BIT_SET | TR_BIT_SET;

    // Wait until transmission complete
    while (((C1GSR >> 3) & 1) == 0);
}


//============================================================//
//            CAN1 RECEIVE FUNCTION
//============================================================//
void CAN1_Rx(struct CAN_Frame *rxFrame)
{
    // Wait until Receive Buffer has data
    while ((C1GSR & RBS_BIT_READ) == 0);

    rxFrame->ID = C1RID;  // Read received 11-bit CAN ID

    // Extract RTR bit
    rxFrame->vbf.RTR = (C1RFS >> 30) & 1;

    // Extract DLC (Data Length Code)
    rxFrame->vbf.DLC = (C1RFS >> 16) & 0x0F;

    // If Data Frame
    if (rxFrame->vbf.RTR == 0)
    {
        rxFrame->Data1 = C1RDA;  // Read first 4 data bytes
        rxFrame->Data2 = C1RDB;  // Read next 4 data bytes
    }

    C1CMR = RRB_BIT_SET;  // Release Receive Buffer
}
