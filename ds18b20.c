//==================== Includes ====================//
#include <LPC21xx.h>
#include "delay.h"

//==================== 1-Wire Definitions ====================//
#define D  (1 << 20)                 // 1-Wire Data Line (P0.20)
#define R  (IOPIN0 & (1 << 20))      // Read Data Line Macro


//============================================================//
//            RESET FUNCTION FOR DS18B20
//============================================================//
unsigned char ResetDS18b20(void)
{
    unsigned int presence;

    IODIR0 |= D;                     // Configure as output
    IOPIN0 |= D;                     // Set line high
    delayUS(1);                      // 1us delay

    IOPIN0 &= ~(D);                  // Pull line low
    delayUS(478);                    // Reset pulse (~480us)

    IOPIN0 |= D;                     // Release line
    delayUS(54);                     // Wait for presence

    presence = IOPIN0;               // Read presence response
    delayUS(423);                    // Wait remaining time slot

    if (presence & R)                // Check presence bit
        return 1;
    else
        return 0;
}


//============================================================//
//            READ A SINGLE BIT FROM DS18B20
//============================================================//
unsigned char ReadBit(void)
{
    unsigned int B;

    IOPIN0 &= ~(D);                  // Pull line low
    delayUS(1);                      // 1us delay

    IOPIN0 |= D;                     // Release line
    IODIR0 &= ~(D);                  // Configure as input
    delayUS(10);                     // Wait before sampling

    B = IOPIN0;                      // Read line

    IODIR0 |= D;                     // Configure back as output

    if (B & R)                       // Check bit value
        return 1;
    else
        return 0;
}


//============================================================//
//            WRITE A SINGLE BIT TO DS18B20
//============================================================//
void WriteBit(unsigned char Dbit)
{
    IOPIN0 &= ~(D);                  // Pull line low
    delayUS(1);                      // 1us delay

    if (Dbit)
        IOPIN0 |= D;                 // Write '1'

    delayUS(58);                     // Hold time (~60us)

    IOPIN0 |= D;                     // Release line
    delayUS(1);                      // Recovery time
}


//============================================================//
//            READ A BYTE FROM DS18B20
//============================================================//
unsigned char ReadByte(void)
{
    unsigned char i;
    unsigned char Din = 0;

    for (i = 0; i < 8; i++)
    {
        Din |= ReadBit() ? (0x01 << i) : Din;  // LSB first
        delayUS(45);                           // Bit interval delay
    }

    return Din;
}


//============================================================//
//            WRITE A BYTE TO DS18B20
//============================================================//
void WriteByte(unsigned char Dout)
{
    unsigned char i;

    for (i = 0; i < 8; i++)
    {
        WriteBit((Dout & 0x1));   // Send LSB first
        Dout = Dout >> 1;         // Shift right
        delayUS(1);               // Small delay
    }

    delayUS(98);                  // Recovery delay (~100us)
}


//============================================================//
//            READ TEMPERATURE FROM DS18B20
//============================================================//
int ReadTemp(void)
{
    unsigned char n, buff[2];
    int temp;

    ResetDS18b20();               // Reset device
    WriteByte(0xCC);              // Skip ROM
    WriteByte(0x44);              // Start temperature conversion

    while (ReadByte() == 0xFF);   // Wait until conversion complete

    ResetDS18b20();               // Reset again
    WriteByte(0xCC);              // Skip ROM
    WriteByte(0xBE);              // Read scratchpad

    for (n = 0; n < 2; n++)       // Read first 2 bytes only
    {
        buff[n] = ReadByte();
    }

    temp = buff[1];               // MSB
    temp = temp << 8;
    temp = temp | buff[0];        // Combine LSB

    return temp;                  // Return raw temperature
}
