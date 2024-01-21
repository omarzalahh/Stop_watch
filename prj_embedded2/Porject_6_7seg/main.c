/*
 *  Project embedded 2.c
 *  Name: Omar Salah
 *  Created on: Sep 16, 2021
 *
 */
/*
 * Hash includes
 */
#include<avr/io.h>
#include<util/delay.h>
#include<avr/interrupt.h>
/*
 * Global  values of Minutes,seconds,hours and intialized by 0
 */
unsigned char sec=0,min=0,hour=0;
/*
 * Functions
 */
void Timer1_Init()
 {
	 /* Configure the timer mode
	 	 * 1. Non PWM mode FOC1A = 1
	 	 * 2. CTC Mode WGM10 = 0 & WGM11 = 0 & WGM12 = 1 & WGM13 = 0
	 	 * 3. clock = CPU clock/1024 CS10 = 1 CS11 = 0 CS12 = 1
	 	 */
	 TCCR1A|=(1<<FOC1A);
	 TCCR1B|=(1<<CS12)|(1<<CS10)|(1<<WGM12);
	 /*
	  * Initial value of timer1
	  */
	 TCNT1 = 0;
	 /*
	  * TOP value
	  */
	 OCR1A=976;//After calculate the time by the rule
	SREG |= (1<<7);

	 /*
	  * Enable interrupt
	  */
	 TIMSK |= (1<<OCIE1A);

 }
 ISR (TIMER1_COMPA_vect)
 {
	  sec++;
	 	if (sec == 60)
	 	{
	 		sec = 0;
	 		min ++;
	 	}
	 	if (min == 60)
	 	{
	 		min = 0;
	 		sec=0;
	 		hour ++;
	 	}
	 	if (hour == 24)
	 	{
	 		hour = 0 ;
	 		min = 0;
	 		sec=0;
	 	}
 }
void INT0_Init_Reset()
{
	DDRD &=~(1<<PD2);
	PORTD|=(1<<PD2);
	/*
	 * internal pull up
    */
		/* Configure interrupt 0
		 * 1. Enable interrupt 0 from GICR
		 * 2. Enable Falling edge
		 * 3. Enable Global Interrupt
		 * 4.Falling edge
		 */
GICR=(1<<INT0);
MCUCR=(1<<ISC01);
SREG |=(1<<7);
}
ISR(INT0_vect)
{
	sec=0,min=0,hour=0;
}
void INT1_Init_Pause(void)
{
	DDRD &= ~(1<<PD3);
	/* Configure interrupt 1
	 * 1. Enable interrupt 1 from GICR
	 * 2. Enable Global Interrupt
	 */
	MCUCR|=(1<<ISC10)|(1<<ISC11);	/*Raising edge*/
	GICR |=(1<<INT1);   /*interrupt 1 enable*/
	SREG|=(1<<7);
}
ISR(INT1_vect)
{
	/* this ISR is to stop the clock by switch timer1 CTC mode interrupt OFF */
	TCCR1B &= ~(1<<CS10)&~(1<<CS11)&~(1<<CS12);
}
void INT2_Init_Resume(void)
{
	DDRB &=~(1<<PB2);
	PORTB |= (1<<PB2);   /* internal pull up*/

	/* Configure interrupt 2
	 * 1. Enable interrupt 2 from GICR
	 * 2. Enable Falling edge mode >> ISC2 = 0
	 * 3. Enable Global Interrupt*/
	MCUCR&=~(1<<ISC2);	// falling edge
	GICR |=(1<<INT2);
	SREG|=(1<<7);
}
ISR(INT2_vect)
{
	 TCCR1B|=(1<<CS12)|(1<<CS10)|(1<<WGM12);
}
 /*
 Main
  */
 int main()
 {
	 DDRC|=0X0F;
	 DDRA|=0X3F;
	 PORTC&=~0X0F;
	 PORTA&=~0X3F;
	 INT0_Init_Reset();
	 INT1_Init_Pause();
	 INT2_Init_Resume();
	 Timer1_Init();
	 while(1)
	 {

		 PORTA = (1<<0);
		 PORTC = (sec % 10);
		 _delay_us(1);
		 PORTA = (1<<1);
		 PORTC = sec/10;
		 _delay_us(1);
		 PORTA = (1<<2);
		 PORTC = min % 10;
		 _delay_us(1);
		 PORTA = (1<<3);
		 PORTC = min / 10;
		 _delay_us(1);
		 PORTA = (1<<4);
		 PORTC = hour % 10;
		 _delay_us(1);
		 PORTA = (1<<5);
		 PORTC = hour / 10;
		 _delay_us(1);

	 }
 }
