
/*********************************************************************************************
*
*                                                 a simple formula car dashboard
*
* GccApplication1.c
*
* Created: 2/5/2020 11:55:41 PM						  ----------------------------
* Author : Aya Hamed
* A simple car dashboard reading values from sensors and displaying them on 16x2 character display.
*
* Sensors:
*   1. Speed : A hall effect sensor generating a variable frequency square wave with each 12 pulses representing
*   a complete revolution of the wheel. Connected to INT1 .
*
*   2. RPM : A hall effect sensor generating a variable frequency square wave with each 36 pulses representing
*   a complete revolution of the crankshaft. Connected to ICP1 .
*
*   3. Throttle Position : A rotary potentiometer. Connected to ADC0;
*
*   4. Steering Offset : A rotary potentiometer with the normal position of the steering (wheel straight ahead) at 
*   the middle of the travel. Connected to ADC1.
*
*   5. Engine Temperature : A linear sensor generating a voltage between 0v and 2.5v, with 0v representing -25 degrees C
*      and 2.5v representing +125 degrees C. Connected to ADC2.
*
* 
**********************************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "lcd.h"
#include "myutils.h"
#include <stdint.h>

#define WHEEL_RADIUS 40

uint16_t adc_read(uint8_t adcx); //read function for adc while adcx is the ADC channel pin
void adc_init();
void normal_timer_init();
void capture_timer_init();
void interrupt_init();

volatile int count; // a counter for number of TIMER0 overflow events
volatile uint16_t rpm_pulses ; //number of rising edges of the rpm input signal
volatile uint16_t speed_pulses ; //number of rising edges of the speed input signal
volatile uint16_t rpm ; //actual value of rpm
volatile uint8_t speed ; // actual value of speed

int main()
{
	
	uint16_t  throttle_position = 0;
	uint16_t  temperature_sensor =0;
	uint16_t  steering_sensor =0;
	int engine_temperature =0;
	int steering_offset=0;
		
	
	LCDInit(LS_BLINK); //initialize LCD in blinking cursor mode
	adc_init();
	capture_timer_init();
	normal_timer_init();
	interrupt_init();
	sei();	//enable global interrupts
	

	while (1)
	{
		//Write in LCD the constant characters 
		LCDWriteStringXY(7,1,"*C:" );
		LCDWriteStringXY(14,1,"*" );
		LCDWriteStringXY(2,1,"%:" )
		LCDWriteStringXY(4,0,"kph :" );
		
		//Display rpm and speed values computed in the timer overflow ISR
		LCDWriteIntXY(10,0,rpm,5);
		LCDWriteIntXY(0,0,speed,3);

		//The 0-5V range is divided into 2^10 = 1024 steps. Thus, a 0V input will give an ADC output of 0
		//5V input will give an ADC output of 1023 equivalent to 99% throttle position
		throttle_position = adc_read(PC0)*99.0 /1023.0 ;
		LCDWriteIntXY(0,1,throttle_position,2);
		

		//Max ADC value is 512 since the maximum input voltage is 2.5v . Temperature ranges from -25 to 125
		temperature_sensor = adc_read(PC2);
		engine_temperature = (temperature_sensor *150.0 /512.0) - 25.0;

		if ( engine_temperature < 0)
		{
			//if temp is a negative value , get the absolute and send it to LCD
			engine_temperature = - engine_temperature;
			LCDWriteIntXY(5,1,engine_temperature,2);
			LCDWriteStringXY(4,1,"-" );
		}
		else
		{
			LCDWriteIntXY(4,1,engine_temperature,3);
		}
		
		

		//Steering offset ranges from -180 to 180
		steering_sensor = adc_read(PC1);
		steering_offset =(( steering_sensor /1023.0)*360)-180;
		
		if (steering_offset <0)
		{
			steering_offset = -steering_offset;
			LCDWriteStringXY(10,1,"-" );
			LCDWriteIntXY(11,1,steering_offset,3);
		}
		else
		{
			LCDWriteStringXY(10,1,"+" );
			LCDWriteIntXY(11,1,steering_offset,3);
		}

	}
	return 0;
}

void adc_init()
{
	ADMUX = (1<<REFS0); // internal Vref
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0); //ADC Enable and prescaler of 128
}

uint16_t adc_read(uint8_t adcx)
{
	ADMUX	&=	0xf0;   // disable all channels
	ADMUX	|=	adcx; 	// read corresponding channel adcx
	ADCSRA |= (1<<ADSC); 	// start single conversion
	while ( (ADCSRA & (1<<ADSC)) ); //wait for the conversion to finish
	return ADC;
}



void capture_timer_init()
{
	TCCR1A = 0;	//Normal Mode
	TCCR1B =  (1<< ICNC1) | (1<< ICES1) |  (1<< CS10) ;	//capture on rising edge + no prescaler + filter input signal
	TIMSK1 =(1<< ICIE1) ;	//Timer/Counter1 Input Capture interrupt is enabled
}

ISR ( TIMER1_CAPT_vect )
{
	rpm_pulses ++ ;
}

void normal_timer_init()
{
	TCCR0A = 0; //Normal mode
	TCCR0B |= (1<<CS01) | (1<<CS00); //prescaler = 1024
	TIMSK0 |= ( 1<<  TOIE0 ); //enable overflow interrupt
}

ISR(TIMER0_OVF_vect)
{

	count++; 	// 1 count represents 0.01632 seconds

	// 20 counts represent 0.408 second
	if ( count == 20)
	{
		cli(); //disable global interrupts
		rpm = (rpm_pulses/36.0)*60.0*0.3264 ; //36 pulse represents 1 revolution of the crankshaft, 60 to convert from HZ to rpm
		rpm_pulses=0; count=0; //reset rpm_pulses and count

		// 12 pulses represents a complete revolution of the wheel, conversion from cm/sec to km/hr = 0.036
		speed = (speed_pulses/12.0)*(2.0*3.14*WHEEL_RADIUS)*0.036/0.3264;
		speed_pulses=0; // reset speed pulses
		sei(); //enable global interrupts
	}
}


void interrupt_init()
{
	EICRA |= (1<<ISC11) |  (1<<ISC10); // enable rising edge interrupt(INT1)
	EIMSK |= (1<<INT1); //enable external interrupt 1 (INT1)
}

ISR(INT1_vect)
{
	speed_pulses++;
}