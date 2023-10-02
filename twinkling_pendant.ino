 /* Twinkling Pendant - see http://www.technoblogy.com/show?3RMX

   David Johnson-Davies - www.technoblogy.com - 23rd February 2022
   
   This file is a port for attiny412 of the original
   attiny10 project http://www.technoblogy.com/show?3RMX
   
   connect leds to pins 4,5,7
   
   ATtiny412 @ 1MHz (internal oscillator)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

#include <avr/sleep.h>
#include <avr/interrupt.h>

#define F_CPU             1000000UL

uint8_t  __attribute__ ((section (".noinit"))) Power;

// LEDs:                426 153
//const uint8_t Mask = 0b011101110;       // Valid light positions
volatile uint8_t Lights = 0;              // All off
volatile uint8_t Ticks;

// Delay in 1/200ths of a second
void _delay (uint8_t msec5) {
  Ticks = 0;
  while (Ticks < msec5);
}

// Multiplex LEDs
ISR(TCB0_INT_vect) {
  TCB0.INTFLAGS = TCB_CAPT_bm;                // Clear the interrupt flag

  static uint8_t Count;
  PORTA.OUT = 0;                              // All bits low
  PORTA.DIR = 0;                              // All pins inputs
  Count = (Count + 1) % 5 ;
  uint8_t bits = Lights>>(3*Count) & 0b1111;
  PORTA.DIR = (1<<Count | bits)<<1;           // Make outputs
  PORTA.OUT = bits<<1;                        // Take bits high

  Ticks++;
}

void setup () {
  sei();

  TCB0.CCMP = (unsigned int)(F_CPU/500 - 1);           // Divide clock to give 1999*Hz
  TCB0.CTRLA = TCB_CLKSEL_CLKDIV1_gc | TCB_ENABLE_bm ; // Enable timer, divide by 1
  TCB0.CTRLB = 0;                                      // Periodic Interrupt Mode
  TCB0.INTCTRL = TCB_CAPT_bm;                          // Enable interrupt
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  _delay(20);                              // 100 msec
  Power = 1 || ~Power & 1; // todo find why hardware reset is not reseting the microcontroller
}

void loop () {
  if (Power) {
    for (int n=0; n<=32767; n++) {
      for (int i=1; i<8; i++) {
        int k = n % (30 + i);
        if (k == 0 && i != 4) Lights = Lights | 1<<i; // LED on
        else Lights = Lights & ~(1<<i);               // LED off
      }
    _delay(20);                            // 100 msec
    }
  }
  PORTA.OUT = 0;
  sleep_enable();
  sleep_cpu();
}
