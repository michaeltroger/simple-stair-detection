// these constants describe the pins. They won't change:
const int groundpin = 18;             // analog input pin 4 -- ground
const int powerpin = 19;              // analog input pin 5 -- voltage
const int xPin = A3;                  // x-axis of the accelerometer
const int yPin = A2;                  // y-axis
const int zPin = A1;                  // z-axis (only on 3-axis models)

//The minimum and maximum values that came from
//the accelerometer while standing still
//You very well may need to change these
int minVal = 265;
int maxVal = 402;


//to hold the caculated values
double x;
double y;
double z;


void setup(){
  Serial.begin(9600); 

   pinMode(groundpin, OUTPUT);
  pinMode(powerpin, OUTPUT);
  digitalWrite(groundpin, LOW);
  digitalWrite(powerpin, HIGH);
}


void loop(){
  

  //read the analog values from the accelerometer
  int xRead = analogRead(xPin);
  int yRead = analogRead(yPin);
  int zRead = analogRead(zPin);

  Serial.print(xRead);
  Serial.print("\t");
  Serial.print(yRead);
  Serial.print("\t");
  Serial.print(zRead);
  Serial.print("\t");
  delay(1000);
  //convert read values to degrees -90 to 90 - Needed for atan2
  int xAng = map(xRead, minVal, maxVal, -90, 90);
  int yAng = map(yRead, minVal, maxVal, -90, 90);
  int zAng = map(zRead, minVal, maxVal, -90, 90);

  //Caculate 360deg values like so: atan2(-yAng, -zAng)
  //atan2 outputs the value of -π to π (radians)
  //We are then converting the radians to degrees
  x = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
  y = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);
  z = RAD_TO_DEG * (atan2(-yAng, -xAng) + PI);

  //Output the caculations
  Serial.print("x: ");
  Serial.print(x);
  Serial.print(" | y: ");
  Serial.print(y);
  Serial.print(" | z: ");
  Serial.println(z);

  delay(1000);//just here to slow down the serial output - Easier to read
}
