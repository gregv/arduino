#include <JeeLib.h> // Low power functions library

ISR(WDT_vect) { Sleepy::watchdogEvent(); } // Setup the watchdog


// Arduino Uno (135mA) 0.135A servo on, 35mA (0.035A) sleeping
// Arduino Mini Pro: 0.135A servo on, 0.005A (5mA) sleeping, 0.035A awake
// At 2.8Ah for the batteries, running at 5mA, this should last 560 hours (23.3 days)
// http://referencedesigner.com/cal/cal_54.php

int DAY_TIME = 700; // this number or greater is considered bright daytime
int NIGHT_TIME = 210; // this number or less is considered dark/night

// Analog
int LDR_Pin = A0; //analog pin 0 - Photo Resistor
boolean isOpen = false; // starts closed - assume blinds are closed becayse its bright/daytime

// Digital
int myLed = LED_BUILTIN; // out
int servoPin = 12;

void setup() {
  pinMode(myLed, OUTPUT);
  pinMode(servoPin, OUTPUT);
  
  Serial.begin(9600);
  delay(5000);  
}


void flashLed(int pin, int times, int wait) {
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}

// Blinds Open/Close
// write(180) and 5 seconds = 3.75 rotations
void closeBlinds()
{
  if( isOpen )
  {
    Serial.println("Closing Blinds");
    // We attach here so the motor isn't used when we don't need to use it
    // http://www.bajdi.com/continuous-rotation-servos-and-arduino/
    // 90 = Stop, Lower than 90 is counter-clockwise, larger than 90 is clockwise

    for (int i = 0; i<=750; i++) 
    { 
      digitalWrite(servoPin, LOW); 
      delay(1.25);             
      digitalWrite(servoPin, HIGH); 
      delay(18.75);            
    } 

    isOpen = false;
  }
}

void openBlinds()
{
  if( !isOpen ) 
  {    
    Serial.println("Opening Blinds");
    for (int i = 0; i<=500; i++) 
    { 
      digitalWrite(servoPin, HIGH); 
      delay(1.25); 
      digitalWrite(servoPin, LOW); 
      delay(18.75); 
    } 
    isOpen = true;
  }
}

void loop() {
  int LDRReading = analogRead(LDR_Pin); 
  Serial.println(LDRReading);
  

  if( LDRReading > DAY_TIME )
  {        
    closeBlinds();        
  }
  else if( LDRReading < NIGHT_TIME )
  {      
    openBlinds(); 
  }

  flashLed(myLed, 1, 300);
  Sleepy::loseSomeTime(15000);
}
