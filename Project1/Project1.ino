
//TODO
//Accelerometer
//1.Calculate distance
//2.Display the values

//NTC Thermister
//1. Display failure/warning LED
//2. Calculate temprature
//3. Dosplay the results

//Application
//Design and implement the prtocol
/*
 http://www.sparkfun.com/commerce/categories.php?c=80
 http://www.arduino.cc/en/Tutorial/ADXL3xx
 */

// these constants describe the pins. They won't change:
const int xpin = A3;                  // x-axis of the accelerometer
const int ypin = A2;                  // y-axis
const int zpin = A1;                  // z-axis (only on 3-axis models)
const int NTC_pin=A0;                 //Pin to read thermister data
const int pin_axelero_warning = 2;    //warning LED light for accelerometer
const int pin_axelero_failure = 3;    //Failure LED for accelero meter
const int pin_A = 7;                 // Pin A from rotary encoder
const int pin_B = 6;                 // Pin B from rotary encoder
const int ntcFailurePin=10;
const int ntcWarningPin=11;


double angle;
const float refrenceVoltage=5.0;      //volts
const float sensitivity=0.33;         //this is in volts
const float zeroGVoltage=1.65;        //volts
const unsigned int minAccelerometerValue=280; //if read ADC values below this then there is some issue with sensor.

//variables to hold g values
float x,y,z;
float maxX,maxY,maxZ;
float maxTotalAcceleration=2.5;

//variables for encoder
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev=0;

//Variables for NTC
float ntcReading;
//Application specific variables
boolean accelerometer=true;
boolean accelero_state=true; //specifies if the accelerometer sensor functioning properly;
unsigned long previousTime; //Time to calculate distance
unsigned long loopTime;
unsigned long currentTime;
float totalDistance=0.0; //distance in meters


void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);
  maxX=maxY=maxZ=0.0; //initializing max values to 0;
  //Pin configuration
  pinMode(pin_A, INPUT);
  //  pinMode(pin_B, INPUT);
  pinMode(pin_axelero_warning, OUTPUT);
  pinMode(pin_axelero_failure, OUTPUT);
  pinMode(ntcFailurePin, OUTPUT);
  pinMode(ntcWarningPin, OUTPUT);  
  previousTime=loopTime=millis();
}

void loop()
{
  encoder_A = digitalRead(pin_A);    // Read encoder pins
  encoder_B = digitalRead(pin_B);
  if((!encoder_A) && (encoder_A_prev)){
    // A has gone from high to low 
    accelerometer=!accelerometer;  
  } 
  encoder_A_prev = encoder_A;     // Store value of A for next time      
  if(accelerometer){
    //checking if accelerometer is present
    //reading the values
    x=analogRead(xpin);
    y=analogRead(ypin);
    z=analogRead(zpin);
    if(x<minAccelerometerValue || y<minAccelerometerValue || z<minAccelerometerValue){
      accelero_state=false;
    }
    else{
      accelero_state=true;
    }
    if(!accelero_state){
      digitalWrite(pin_axelero_failure,HIGH);
    }
    else{
      digitalWrite(pin_axelero_failure,LOW);
    } 

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
    pitch=pitch*180/3.14159; //dividing with pi;
    roll=roll*180/3.14159;
    //calculating the max values
    if(abs(maxX)<abs(x)) maxX=x;
    if(abs(maxY)<abs(y)) maxY=y;
    if(abs(maxZ)<abs(z)) maxZ=z;
    currentTime=millis();
    totalDistance+=((currentTime-previousTime)*9.6*(abs(x)-0.01))/1000;
    previousTime=millis();
    // print the sensor values:
    if(loopTime+1000<millis()){
      Serial.print("Pitch: ");
      Serial.print(pitch);
      // print a tab between values:
      Serial.print("\tRoll: ");
      Serial.print(roll);
      Serial.print("\t");

      //printing max values
      Serial.print(x);
      Serial.print("\t");
      Serial.print(y);
      Serial.print("\t");
      Serial.print(z);
      Serial.print("\t");
      if(abs(x)+abs(y)+abs(z) > maxTotalAcceleration){
        Serial.print("Warning to much struggle ");
        digitalWrite(pin_axelero_warning,HIGH);
      }
      else{
        digitalWrite(pin_axelero_warning,LOW);
      }
      Serial.print("  Distance: ");
      Serial.print(totalDistance);
    }
  }
  else{
    //Thermister
    Serial.print("Thermister   ");
    ntcReading=analogRead(NTC_pin);
    if(ntcReading==0){
      digitalWrite(ntcFailurePin,HIGH);
    }else{
      digitalWrite(ntcFailurePin,LOW);
    } 
    Serial.println(ntcReading);
  }



  Serial.println();
  // delay before next reading:
  //delay();
}










