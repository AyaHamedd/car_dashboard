
#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>

#include "lcd.h"


#define SET_E() (LCD_E_PORT|=(1<<LCD_E_POS))
#define SET_RS() (LCD_RS_PORT|=(1<<LCD_RS_POS))
#define SET_RW() (LCD_RW_PORT|=(1<<LCD_RW_POS))

#define CLEAR_E() (LCD_E_PORT&=(~(1<<LCD_E_POS)))
#define CLEAR_RS() (LCD_RS_PORT&=(~(1<<LCD_RS_POS)))
#define CLEAR_RW() (LCD_RW_PORT&=(~(1<<LCD_RW_POS)))



void LCDByte(uint8_t c,uint8_t isdata)
{

uint8_t highNibble,lowNibble;
uint8_t temp;

highNibble=c>>4;
lowNibble=(c & 0x0F);

if(isdata==0)
	CLEAR_RS();
else
	SET_RS();

_delay_us(0.500);

SET_E();
temp=(LCD_DATA_PORT & (~(0X0F<<LCD_DATA_POS)))|((highNibble<<LCD_DATA_POS));
LCD_DATA_PORT=temp;

_delay_us(1);
CLEAR_E();

_delay_us(1);
SET_E();

temp=(LCD_DATA_PORT & (~(0X0F<<LCD_DATA_POS)))|((lowNibble<<LCD_DATA_POS));
LCD_DATA_PORT=temp;

_delay_us(1);
CLEAR_E();

_delay_us(1);
LCDBusyLoop();
}

void LCDBusyLoop()
{
	uint8_t busy,status=0x00,temp;

	LCD_DATA_DDR&=(~(0x0f<<LCD_DATA_POS));
	SET_RW();		//Read mode
	CLEAR_RS();		//Read status
	_delay_us(0.5);	

	do
	{
		SET_E();
		_delay_us(0.5);

		status=(LCD_DATA_PIN>>LCD_DATA_POS);
		status=status<<4;
		_delay_us(0.5);

		CLEAR_E();
		_delay_us(1);

		SET_E();
		_delay_us(0.5);

		temp=(LCD_DATA_PIN>>LCD_DATA_POS);
		temp&=0x0F;

		status=status|temp;

		busy=status & 0b10000000;

		_delay_us(0.5);
		CLEAR_E();
		_delay_us(1);	//tEL
	}while(busy);

	CLEAR_RW();		//write mode
	LCD_DATA_DDR|=(0x0F<<LCD_DATA_POS);
}

void LCDInit()
{
	_delay_ms(30);
	
	LCD_DATA_DDR|=(0x0F<<LCD_DATA_POS);
	LCD_E_DDR|=(1<<LCD_E_POS);
	LCD_RS_DDR|=(1<<LCD_RS_POS);
	LCD_RW_DDR|=(1<<LCD_RW_POS);

	LCD_DATA_PORT&=(~(0x0F<<LCD_DATA_POS));
	CLEAR_E();
	CLEAR_RW();
	CLEAR_RS();
	_delay_us(0.3);

	SET_E();
	LCD_DATA_PORT|=((0b00000010)<<LCD_DATA_POS); //[B] To transfer 0b00100000 i was using LCD_DATA_PORT|=0b00100000
	_delay_us(1);
	CLEAR_E();
	_delay_us(1);
	
	LCDBusyLoop(); 

	LCDCmd(0x0D);
	LCDCmd(0b00101000);			//function set 4-bit,2 line 5x7 dot format

	LCDGotoXY(0,0);
}
void LCDWriteString(const char *msg)
{
 while(*msg!='\0')
 {
	LCDData(*msg);
	msg++;
 }
}

void LCDWriteInt(int val,unsigned int field_length)
{
	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) LCDData('-');
	for(i=j;i<5;i++)
	{
	LCDData(48+str[i]);
	}
}

void LCDGotoXY(uint8_t x,uint8_t y)
{
	switch(y)
	{
		case 0:
		LCDCmd(0x80+x);
			break;
		case 1:
		LCDCmd(0xc0 + x);
			break;
	}

}
