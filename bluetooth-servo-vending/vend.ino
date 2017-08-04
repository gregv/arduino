#include <Servo.h> 

const int buttonPin = 2;     // the number of the pushbutton pin
const int led = 7;           // the PWM pin the LED is attached to
const int potPin = A0;    // select the input pin for the potentiometer
const int potLed = 5;

int buttonState = 0;   // variable for reading the pushbutton status

bool doit = false;
Servo myMotor;

#include <SoftwareSerial.h>

#define rxPin 9
#define txPin 10
SoftwareSerial mySerial(rxPin, txPin); // RX | TX


// the setup routine runs once when you press reset:
void setup() {    
  Serial.begin(9600);  
  mySerial.begin(9600);
  
  pinMode(potLed, OUTPUT);
  pinMode(led, OUTPUT);
  
  pinMode(buttonPin, INPUT);
  pinMode(potPin, INPUT);  
}

void dispense(int servoOnForThisManyMillis){  
  myMotor.attach(12);
  myMotor.write(280);
  delay(servoOnForThisManyMillis);
  myMotor.detach();
}


// the loop routine runs over and over again forever:
int servoOnForMs=800;
char blueToothVal;

void loop() {  
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);  

  if (mySerial.available() > 0)
  {
      blueToothVal =  mySerial.read();      
  }
    

  if( blueToothVal == 'h' )
  {   
    doit = true;
    blueToothVal = ' ';
  }
  
  int potval = analogRead(potPin);    // read the value from the sensor
  int brightness = 50;  
  if(potval<=700)
  {
    brightness=50;    
    servoOnForMs=200;    
  }
  if(potval>700 && potval<=715)
  {
    brightness=100;    
    servoOnForMs=400;    
  }
  if(potval>715)
  {
    brightness=200;    
    servoOnForMs=2000;    
  }
  analogWrite(potLed, brightness);
  delay(30);  

 
  if(buttonState == HIGH)
  {    
    doit = !doit;
    delay(500);    
  }

  if(doit)
  {
    Serial.println("Running servo for " + (String)servoOnForMs + " ms");
    digitalWrite(led, HIGH);        
    dispense(servoOnForMs);       
    digitalWrite(led, LOW);
    doit = false;
  }
  else
  {
    digitalWrite(led, LOW);
  }
}
