#ifndef __CAN_H__
#define __CAN_H__

//==================== Includes ====================//
#include "types.h"
//============================================================//
//                    CAN FRAME STRUCTURE
//============================================================//
struct CAN_Frame
{
    u32 ID;                      // 11-bit CAN Identifier

    struct BitField
    {
        u8 RTR : 1;              // Remote Transmission Request bit
        u8 DLC : 4;              // Data Length Code (0–8 bytes)
    } vbf;                       // Variable bit field

    u32 Data1, Data2;            // Data bytes (max 8 bytes total)
};
//============================================================//
//                    FUNCTION DECLARATIONS
//============================================================//
void Init_CAN1(void);                   // Initialize CAN1
void CAN1_Tx(struct CAN_Frame);         // Transmit CAN Frame
void CAN1_Rx(struct CAN_Frame *);       // Receive CAN Frame

#endif  // __CAN_H__

