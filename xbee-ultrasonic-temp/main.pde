#include <XBee.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Temperature data wire is plugged into pin 3 on the Arduino
#define ONE_WIRE_BUS 3

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

DeviceAddress insideThermometer = { 0x28, 0x67, 0x3A, 0xE1, 0x02, 0x00, 0x00, 0xE8 };



XBee xbee = XBee();


// allocate two bytes for to hold a 10-bit analog reading
uint8_t payload[] = { 'H', 'i', 0, 0 }; // Hi follwed by temp, distance (in)

// with Series 1 you can use either 16-bit or 64-bit addressing

// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0xAAAA, payload, sizeof(payload));

// 64-bit addressing: This is the SH + SL address of remote XBee
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x4008b490);
// unless you have MY on the receiving radio set to FFFF, this will be received as a RX16 packet
//Tx64Request tx = Tx64Request(addr64, payload, sizeof(payload));

TxStatusResponse txStatus = TxStatusResponse();

// Analog
int pinA5 = 28; // in

// Digital
//int myBuzzer = 10; // out
int myLed = 9; // out
int mySonar = 7;
long pulse, inches, cm;

void setup() {
  //pinMode(myBuzzer, OUTPUT);
  pinMode(myLed, OUTPUT);
  pinMode(pinA5, INPUT);
  pinMode(mySonar,INPUT);
  
  sensors.begin();
  // set the resolution to 10 bit (good enough?)
  sensors.setResolution(insideThermometer, 10);
  
  xbee.begin(9600); // 9600 is the baud rate
  
  delay(10000);
}

float getTempInF(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  if (tempC == -127.00) {
    Serial.print("Error getting temperature");
    return -1.0;
  } else {
    return DallasTemperature::toFahrenheit(tempC);
  }
}

float fmap(float x, float in_min, float in_max, float out_min, float out_max)
{
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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

void loop() {
   delay(10000);
   
   // Get distance
   pulse = pulseIn(mySonar, HIGH);
   //147uS per inch
   inches = pulse/147;
   //change inches to centimetres
   cm = inches * 2.54;
   
   
   // Get temp
   sensors.requestTemperatures();
   int tempF = (int)getTempInF(insideThermometer);
   
   int i_analogA5 = analogRead(A5);
      
   // float val = fmap(i_analogA5, 0, 1023, 0.0, 5.0);
   payload[2] = tempF;
   payload[3] = inches;
      
   flashLed(myLed, 3, 500);
   xbee.send(tx);
   flashLed(myLed, 1, 200);
      
   
     // Wait 5 sec for a response
  if (xbee.readPacket(5000)) 
  {      
     // should be a znet tx status            	
    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte
           if (txStatus.getStatus() == SUCCESS) {
            	// success.  time to celebrate
             	flashLed(myLed, 10, 50);
           } else {
            	// the remote XBee did not receive our packet. is it powered on?
             	flashLed(myLed, 3, 300);
           }
        }      
    } else {
      // local XBee did not provide a timely TX Status Response -- should not happen
      flashLed(myLed, 2, 200);
    }
   
    
    
}
