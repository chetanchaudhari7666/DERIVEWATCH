//==================== Includes ====================//
#include <LPC21xx.h>
#include "delay.h"
#include "lcd.h"
#include "defines.h"
#include "types.h"
#include "LCD_defines.h"


//============================================================//
//            LCD INITIALIZATION
//============================================================//
void LCD_Init(void)
{
    WRITEBYTE(IODIR0, LCD_DAT, 0xFF);   // Configure data pins as output
    WRITEBIT(IODIR0, LCD_RS, 1);        // Configure RS as output
    WRITEBIT(IODIR0, LCD_RW, 1);        // Configure RW as output
    WRITEBIT(IODIR0, LCD_EN, 1);        // Configure EN as output

    delayMS(16);                        // Power-on delay

    Write_CMD_LCD(0x30);
    delayMS(6);

    Write_CMD_LCD(0x30);
    delayMS(1);

    Write_CMD_LCD(0x30);
    delayMS(1);

    Write_CMD_LCD(0x38);  // 8-bit mode, 2 lines, 5x7 font

    Write_CMD_LCD(0x10);  // Display OFF
    Write_CMD_LCD(0x01);  // Clear display
    Write_CMD_LCD(0x06);  // Cursor increment mode
    Write_CMD_LCD(0x0F);  // Display ON, cursor blinking
}
//============================================================//
//            WRITE COMMAND TO LCD
//============================================================//
void Write_CMD_LCD(char cmd)
{
    WRITEBIT(IOCLR0, LCD_RS, 1);   // RS = 0 (Command)
    Write_LCD(cmd);
}
//============================================================//
//            WRITE DATA TO LCD
//============================================================//
void Write_DAT_LCD(char dat)
{
    WRITEBIT(IOSET0, LCD_RS, 1);   // RS = 1 (Data)
    Write_LCD(dat);
}
//============================================================//
//            LOW LEVEL LCD WRITE FUNCTION
//============================================================//
void Write_LCD(char ch)
{
    WRITEBIT(IOCLR0, LCD_RW, 1);      // RW = 0 (Write mode)
    WRITEBYTE(IOPIN0, LCD_DAT, ch);   // Place data on bus

    WRITEBIT(IOSET0, LCD_EN, 1);      // EN = 1
    delayUS(1);

    WRITEBIT(IOCLR0, LCD_EN, 1);      // EN = 0 (Latch data)
    delayMS(2);                       // Command execution delay
}
//============================================================//
//            WRITE STRING TO LCD
//============================================================//
void Write_str_LCD(char *p)
{
    while (*p)
        Write_DAT_LCD(*p++);          // Print character by character
}
//============================================================//
//            WRITE INTEGER TO LCD
//============================================================//
void Write_int_LCD(signed int n)
{
    char a[10], i = 0;

    if (n < 0)
    {
        Write_DAT_LCD('-');           // Print minus sign
        n = -n;
    }
    do
    {
        a[i++] = n % 10 + 48;         // Convert digit to ASCII
        n = n / 10;
    } while (n);
    for (; i > 0; i--)
        Write_DAT_LCD(a[i - 1]);      // Print digits in correct order
}
//============================================================//
//            WRITE FLOAT TO LCD
//============================================================//
// 1st arg (f) ? float number
// 2nd arg (i) ? digits after decimal
//============================================================//
void Write_float_LCD(float f, char i)
{
    unsigned long int n = f;

    Write_int_LCD(n);                 // Print integer part
    Write_DAT_LCD('.');               // Print decimal point

    for (; i > 0; i--)
    {
        f = f * 10;
        n = f;
        Write_DAT_LCD(n % 10 + 48);   // Print fractional digits
    }
}
//============================================================//
//            BUILD CUSTOM CGRAM CHARACTER
//============================================================//
void BuildCgram(u8 *p, u8 nBytes)
{
    u32 i;

    Write_CMD_LCD(GOTO_CGRAM_START);  // Point to CGRAM

    IOSET0 = 1 << LCD_RS;             // Select data register

    for (i = 0; i < nBytes; i++)
    {
        Write_LCD(p[i]);              // Write pattern bytes
    }

    Write_CMD_LCD(GOTO_LINE1_POS0);   // Return to DDRAM
}
