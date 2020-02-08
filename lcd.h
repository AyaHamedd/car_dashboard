#include <avr/io.h>
#include <util/delay.h>

#define LCD_E_POS	PD0
#define LCD_RS_POS PD2
#define LCD_RW_POS PD1
#define LCD_DATA_POS 1
#define LCD_DATA_PORT 	PORTB
#define LCD_E_PORT 	PORTD
#define LCD_RS_PORT 	PORTD
#define LCD_RW_PORT 	PORTD
#define LCD_DATA_DDR 	DDRB
#define LCD_E_DDR 	DDRD
#define LCD_RS_DDR 	DDRD
#define LCD_RW_DDR 	DDRD
#define LCD_DATA_PIN	PINB


void LCDInit();
void LCDWriteString(const char *msg);
void LCDWriteInt(int val,unsigned int field_length);
void LCDGotoXY(uint8_t x,uint8_t y);
void LCDByte(uint8_t,uint8_t);
#define LCDCmd(c) (LCDByte(c,0))
#define LCDData(d) (LCDByte(d,1))

void LCDBusyLoop();

#define LCDWriteStringXY(x,y,msg) {\
 LCDGotoXY(x,y);\
 LCDWriteString(msg);\
}

#define LCDWriteIntXY(x,y,val,fl) {\
 LCDGotoXY(x,y);\
 LCDWriteInt(val,fl);\
}






