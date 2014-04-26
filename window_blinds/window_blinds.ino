#include <Servo.h> 

int DAY_TIME = 700; // this number or greater is considered bright daytime
int NIGHT_TIME = 200; // this number or less is considered dark/night

// Analog
int LDR_Pin = A0; //analog pin 0
boolean isOpen = false; // starts closed - assume blinds are closed becayse its bright/daytime

// Digital
int myLed = 9; // out
Servo myMotor;

void setup() {
  pinMode(myLed, OUTPUT);
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
    // We attach here so the motor isn't used when we don't need to use it
    myMotor.attach(12);
    myMotor.write(0);
    delay(5000);
    myMotor.detach();
    isOpen = false;
  }
}

void openBlinds()
{
  if( !isOpen ) 
  {    
    myMotor.attach(12);
    myMotor.write(280);
    delay(5000);
    myMotor.detach();
    isOpen = true;
  }
}

void loop() {
  int LDRReading = analogRead(LDR_Pin); 
  Serial.println(LDRReading);
  
  if( LDRReading > DAY_TIME )
  {
    flashLed(myLed, 7, 200);
    closeBlinds();
  }
  else if( LDRReading < NIGHT_TIME )
  {
    flashLed(myLed, 1, 800);
    openBlinds();
  }
  
}
