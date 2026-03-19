//==================== PIN DEFINITIONS ====================//
#define RD1_PIN        0x00040000      // P0.25 as CAN1_RX
//============================================================//
//                 CAN BIT TIMING CONFIGURATION
//============================================================//

#define PCLK           60000000        // Peripheral Clock (Hz)
#define BIT_RATE       125000          // CAN Baud Rate (Hz)
#define QUANTA         16              // Time quanta per bit

#define BRP            (PCLK/(BIT_RATE*QUANTA))   // Baud Rate Prescaler

#define SAMPLE_POINT   (0.7 * QUANTA)             // 70% sample point

#define TSEG1          ((int)SAMPLE_POINT - 1)    // PropSeg + PhaseSeg1
#define TSEG2          (QUANTA - (1 + TSEG1))     // PhaseSeg2

#define SJW            ((TSEG2 >= 5) ? 4 : (TSEG2 - 1))  // Sync Jump Width

#define SAM            0   // 0 = 1 sample, 1 = 3 samples


//============================================================//
//               C1BTR REGISTER FINAL VALUE
//============================================================//
#define BTR_LVAL  ( SAM<<23 | \
                    (TSEG2-1)<<20 | \
                    (TSEG1-1)<<16 | \
                    (SJW-1)<<14  | \
                    (BRP-1) )
//============================================================//
//                C1CMR BIT SET DEFINES
//============================================================//
#define TR_BIT_SET      1<<0     // Transmission Request
#define RRB_BIT_SET     1<<2     // Release Receive Buffer
#define STB1_BIT_SET    1<<5     // Select TX Buffer 1


//============================================================//
//                C1GSR BIT CHECK DEFINES
//============================================================//
#define RBS_BIT_READ    1<<0     // Receive Buffer Status
#define TBS1_BIT_READ   1<<2     // TX Buffer 1 Status
#define TCS1_BIT_READ   1<<3     // TX Complete Status


//============================================================//
//                DUPLICATE DEFINES (UNCHANGED)
//============================================================//
#define TR_BIT_SET      1<<0
#define RRB_BIT_SET     1<<2
#define STB1_BIT_SET    1<<5

#define RBS_BIT_READ    1<<0
#define TBS1_BIT_READ   1<<2
#define TCS1_BIT_READ   1<<3

