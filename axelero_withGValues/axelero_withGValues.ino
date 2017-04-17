
/*
 ADXL3xx
 
 Reads an Analog Devices ADXL3xx accelerometer and communicates the
 acceleration to the computer.  The pins used are designed to be easily
 compatible with the breakout boards from Sparkfun, available from:
 http://www.sparkfun.com/commerce/categories.php?c=80

 http://www.arduino.cc/en/Tutorial/ADXL3xx

 The circuit:
 analog 0: accelerometer self test
 analog 1: z-axis
 analog 2: y-axis
 analog 3: x-axis
 analog 4: ground
 analog 5: vcc
 
 created 2 Jul 2008
 by David A. Mellis
 modified 30 Aug 2011
 by Tom Igoe 
 
 This example code is in the public domain.

*/

// these constants describe the pins. They won't change:
const int groundpin = 18;             // analog input pin 4 -- ground
const int powerpin = 19;              // analog input pin 5 -- voltage
const int xpin = A3;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A1;                  // z-axis (only on 3-axis models)
double angle;
const float refrenceVoltage=5.0;
const float sensitivity=0.33; //this is in volts
const float zeroGVoltage=1.65; //volts

void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);
  
  // Provide ground and power by using the analog inputs as normal
  // digital pins.  This makes it possible to directly connect the
  // breakout board to the Arduino.  If you use the normal 5V and
  // GND pins on the Arduino, you can remove these lines.
  //pinMode(groundpin, OUTPUT);
//  pinMode(powerpin, OUTPUT);
//  digitalWrite(groundpin, LOW); 
//  digitalWrite(powerpin, HIGH);
}

void loop()
{
  
  //reading the values
  float x=analogRead(xpin);
  float y=analogRead(ypin);
  float z=analogRead(zpin);
  //this is the total voltage
  x=x*refrenceVoltage/1024.0; //this is in volts
  y=y*refrenceVoltage/1024.0;
  z=z*refrenceVoltage/1024.0;
  //substract 0g voltage
  x=(x-zeroGVoltage)/sensitivity;
  y=(y-zeroGVoltage)/sensitivity;
  z=(z-zeroGVoltage)/sensitivity;
  // print the sensor values:
  Serial.print(x);
  // print a tab between values:
  Serial.print("\t");
  Serial.print(y);
  // print a tab between values:
  Serial.print("\t");
  Serial.print(z);
  Serial.print("\t");
//  angle=atan(x/sqrt((y*y)+(z*z)));
  //angle=atan(analogRead(xpin)/analogRead(ypin));
  //angle=(angle*18000/314);
//  Serial.print(angle);
  Serial.println();
  // delay before next reading:
  delay(1000);
}
