 /* Twinkling Pendant - see http://www.technoblogy.com/show?3RMX

   David Johnson-Davies - www.technoblogy.com - 23rd February 2022
   
   This file is a port for attiny412 of the original
   attiny10 project http://www.technoblogy.com/show?3RMX
   
   connect leds to pins 3,4,5,7
   
   ATtiny412 @ 1MHz (internal oscillator)
   
   CC BY 4.0
   Licensed under a Creative Commons Attribution 4.0 International license: 
   http://creativecommons.org/licenses/by/4.0/
*/

#include <avr/sleep.h>
#include <avr/interrupt.h>

#define LEDS_TOTAL 12
#define GENERATOR_TOP 32767
#define GENERATOR_LOW 0

volatile uint8_t Lights = 0;  // All off

uint8_t ledMultiplexMap[][2] = {
  { 0b0, 0b0 },
  { 0b0110, 0b10 },
  { 0b0110, 0b100 },
  { 0b1010, 0b10 },
  { 0b1010, 0b1000 },
  { 0b1100, 0b100 },
  { 0b1100, 0b1000 },
  { 0b1000010, 0b0000010 },
  { 0b1000010, 0b1000000 },
  { 0b1000100, 0b1000000 },
  { 0b1000100, 0b0000100 },
  { 0b1001000, 0b0001000 },
  { 0b1001000, 0b1000000 },
//  { 0b11000000, 0b10000000 },
//  { 0b11000000, 0b01000000 },
};

void multiplex() {
  PORTA.OUT = 0;  // All bits low
  PORTA.DIR = 0;  // All pins inputs

  PORTA.DIR = ledMultiplexMap[Lights][0];  // Make outputs
  PORTA.OUT = ledMultiplexMap[Lights][1];  // Take bits high
}

void setup () {
  rtcPitSetup();
  sleepSetup();
  sleep();
}

void loop () {
}
  
// Interrupt Service Routine called every second
ISR(RTC_PIT_vect) {
  RTC.PITINTFLAGS = RTC_PI_bm;  // Clear interrupt flag

  generator();
  multiplex();

  sleep();
}

uint16_t n = GENERATOR_TOP;
uint8_t i = LEDS_TOTAL;
void generator() {
  int k = n % (1 + i);

  if (k == 0) {
    Lights = i; // LED on
  } else {
    Lights = 0; // LED's off
  }
  
  --i;
  if (i == 1) {
    i = LEDS_TOTAL;
    --n;
    if (n == GENERATOR_LOW) {
      n = GENERATOR_TOP;  
    }
  }      
}

void sleep() {
  sleep_cpu();    
}

void rtcPitSetup() {
  // All internal clock sources are enabled automatically when they are requested by a peripheral
  RTC.CLKSEL = RTC_CLKSEL_INT32K_gc;
    
  // RTC Clock Cycles 2048, enabled ie 16Hz interrupt
  RTC.PITCTRLA = RTC_PERIOD_CYC2048_gc | RTC_PITEN_bm; 
  RTC.PITINTCTRL = RTC_PI_bm;  // Periodic Interrupt: enabled
}

void sleepSetup() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  //PWR_DOWN, IDLE, STANDBY
  sleep_enable();
  
  // Turn on all the pullups for minimal power in sleep
  PORTA.PIN0CTRL = PORT_PULLUPEN_bm;  // UPDI
//  PORTA.PIN1CTRL = PORT_PULLUPEN_bm;
//  PORTA.PIN2CTRL = PORT_PULLUPEN_bm;
//  PORTA.PIN3CTRL = PORT_PULLUPEN_bm;
  PORTA.PIN6CTRL = PORT_PULLUPEN_bm;
//  PORTA.PIN7CTRL = PORT_PULLUPEN_bm;
}
