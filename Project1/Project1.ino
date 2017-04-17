
/*
 ADXL3xx
 
 Reads an Analog Devices ADXL3xx accelerometer and communicates the
 acceleration to the computer.  The pins used are designed to be easily
 compatible with the breakout boards from Sparkfun, available from:
 http://www.sparkfun.com/commerce/categories.php?c=80

 http://www.arduino.cc/en/Tutorial/ADXL3xx
*/

// these constants describe the pins. They won't change:
const int xpin = A3;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A1;                  // z-axis (only on 3-axis models)
double angle;
const float refrenceVoltage=5.0; //volts
const float sensitivity=0.33; //this is in volts
const float zeroGVoltage=1.65; //volts
//variables to hold g values
float x,y,z;
float maxX,maxY,maxZ;
const float maxTotalAcceleration=2.5;

void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);
  maxX=maxY=maxZ=0.0; //initializing max values to 0;
}

void loop()
{
  
  //reading the values
  x=analogRead(xpin);
  y=analogRead(ypin);
  z=analogRead(zpin);
  //this is the total voltage
  x=x*refrenceVoltage/1024.0; //this is in volts
  y=y*refrenceVoltage/1024.0;
  z=z*refrenceVoltage/1024.0;
  //substract 0g voltage
  x=(x-zeroGVoltage)/sensitivity;
  y=(y-zeroGVoltage)/sensitivity;
  z=(z-zeroGVoltage)/sensitivity;
  //calculate tilt angles
  float pitch =atan(x/sqrt((y*y)+(z*z)));
  float roll =atan(y/sqrt((x*x)+(z*z)));
  pitch=pitch*180/3.14; //dividing with pi;
  roll=roll*180/3.14;
  //calculating the max values
  if(abs(maxX)<abs(x)) maxX=x;
  if(abs(maxY)<abs(y)) maxY=y;
  if(abs(maxZ)<abs(z)) maxZ=z;

  // print the sensor values:
  Serial.print("Pitch: ");
  Serial.print(pitch);
  // print a tab between values:
  Serial.print("\t");
  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print("\t");
  //printing max values
  Serial.print(maxX);
  Serial.print("\t");
  Serial.print(maxY);
  Serial.print("\t");
  Serial.print(maxZ);
  Serial.print("\t");
  if(abs(x)+abs(y)+abs(z) > maxTotalAcceleration){
    Serial.print("Warning to much struggle ");
  }
  Serial.println();
  // delay before next reading:
  delay(1000);
}
