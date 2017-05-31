
//TODO
//Accelerometer
//1.Calculate distance
//2. calculate tilt
//NTC Thermister
//Application
//fine tune rotary encoder
//implement push button for resetting values
/*
  http://www.sparkfun.com/commerce/categories.php?c=80
  http://www.arduino.cc/en/Tutorial/ADXL3xx
*/

// Use the softwareserial library to create a new "soft" serial port
// for the display. This prevents display corruption when uploading code.
#include <SoftwareSerial.h>

// these constants describe the pins. They won't change:
const int xpin = A3;               // x-axis of the accelerometer
const int ypin = A2;               // y-axis
const int zpin = A1;               // z-axis (only on 3-axis models)
const int NTC_pin = A0;            //Pin to read thermister data
const int pin_axelero_warning = 2; //warning LED light for accelerometer
const int pin_axelero_failure = 3; //Failure LED for accelero meter
const int pin_A = 7;               // Pin A from rotary encoder
const int pin_B = 6;               // Pin B from rotary encoder
const int pin_reset_button = 8;
const int pin_led_reset = 13;
const int pin_ntc_Failure = 10;
const int pin_ntc_Warning = 11;

float angle;
const float refrenceVoltage = 5.0;              //volts
const float sensitivity = 0.33;                 //this is in volts
const float zeroGVoltage = 1.65;                //volts
const unsigned int minAccelerometerValue = 280; //if read ADC values below this then there is
//some issue with sensor.

//variables to hold g values
float x, y, z;
float maxX = 0;
float maxY = 0;
float maxZ = 0;
float xCC = 0;
float yCC = 0;
float zCC = 0;
float maxTotalAcceleration = 2.5;
float roll;
float pitch;
float tilt;

//variables for encoder
unsigned char encoder_A;
unsigned char encoder_B;
unsigned char encoder_A_prev = 0;

//Variables for NTC
const float vInThermistor = 5;
const int r2Thermistor = 10000;
long resistance[] = {12200, 14770, 17970, 22000, 27080, 33550, 41810, 52450, 66240, 84250};
int temperatures[] = {40, 35, 30, 25, 20, 15, 10, 5, 0, -5};
int minTemp = 200;
int maxTemp = -200;
int minAllowedTemp = -5;
int maxAllowedTemp = 30;
int tempDelta = 20;
int temperature;
int tempCalibrationConst = 0;

//Application specific variables
int resetButtonState = 0;
String displayOutput;
boolean displayToogle = true;
boolean accelerometer = false;
boolean accelero_state = true; //specifies if the accelerometer sensor functioning properly;
//unsigned long previousTime; //Time to calculate distance
unsigned long loopTimeRotaryEncoder;
unsigned long currentTimeRotaryEncoder;
boolean autoDataTrans = false;
boolean isDebug = false;

unsigned long loopTimeDisplayOutput;
unsigned long currentTimeDisplayOutput;
boolean humanRedableOP = true;
const int DISPLAY_UPDATE_INTERVAL = 1000;

float totalDistance = 0.0; //distance in meters
int displayScreen;

SoftwareSerial mySerial(9, 12); // pin 12 = TX, pin 9 = RX (unused)

//Protocol constanats
const char START = '$';
const char STOP = '*';
const char CHECKSUM = '#';
const char SEPERATOR = ';';
//Thermister
const int REQ_TEMP = 1;
const int RESP_TEMP = 2;
const int REQ_MAX_TEMP = 3;
const int RESP_MAX_TEMP = 4;
const int REQ_MIN_TEMP = 5;
const int RESP_MIN_TEMP = 6;
//Accelerometer
const int REQ_CURR_X = 10;
const int RESP_CURR_X = 11;
const int REQ_CURR_Y = 12;
const int RESP_CURR_Y = 13;
const int REQ_CURR_Z = 14;
const int RESP_CURR_Z = 15;
const int REQ_MAX_X = 16;
const int RESP_MAX_X = 17;
const int REQ_MAX_Y = 18;
const int RESP_MAX_Y = 19;
const int REQ_MAX_Z = 20;
const int RESP_MAX_Z = 21;
const int REQ_TILT = 22;
const int RESP_TILT = 23;
const int REQ_PITCH = 24;
const int RESP_PITCH = 25;
const int REQ_ROLL = 26;
const int RESP_ROLL = 27;

//Operational
const int REQ_PARA = 30;
const int RESP_PARA = 31;
const int SET_PARA = 32;

const int ALARM_TEMP_MAX = 40;
const int ALARM_TEMP_DELTA = 41;
const int ALARM_ACC = 42;
const int RESET_MIN_MAX = 43;

//Data
const int MAX_TEMPERATURE = 50;
const int MIN_TEMPERATURE = 51;
const int MAX_TEMP_TRESHOLD = 52;
const int DELTA_TEMP_TRESHOLD = 53;
const int MAX_ACC_X = 60;
const int MAX_ACC_Y = 61;
const int MAX_ACC_Z = 62;
const int CURR_ACC_X = 63;
const int CURR_ACC_Y = 64;
const int CURR_ACC_Z = 65;
const int TILT = 66;
const int MAX_ACC_TRESHOLD = 67;
const int CALIBRATION_X = 80;
const int CALIBRATION_Y = 81;
const int CALIBRATION_Z = 82;
const int CALIBRATION_TEMPERATURE = 83;
const int AUTOMATIC_DATA_TRANSMISSION = 90;

const int ACK = 100;
const int NACK = 101;

const int HUMAN_REDABLE_OP = 103;

void setup()
{
  mySerial.begin(9600); // set up serial port for 9600 baud
  // initialize the serial communications:
  Serial.begin(9600);
  maxX = maxY = maxZ = 0.0; //initializing max values to 0;
  //Pin configuration
  pinMode(pin_A, INPUT);
  //  pinMode(pin_B, INPUT);
  pinMode(pin_axelero_warning, OUTPUT);
  pinMode(pin_axelero_failure, OUTPUT);
  pinMode(pin_ntc_Warning, OUTPUT);
  pinMode(pin_ntc_Failure, OUTPUT);

  pinMode(pin_reset_button, INPUT);
  pinMode(pin_led_reset, OUTPUT);

  delay(500); // wait for display to boot up

  currentTimeRotaryEncoder = millis();
  loopTimeRotaryEncoder = currentTimeRotaryEncoder;

  currentTimeDisplayOutput = millis();
  loopTimeDisplayOutput = currentTimeDisplayOutput;
}

void loop()
{
  resetButtonState = digitalRead(pin_reset_button);
  if (resetButtonState == HIGH)
  {
    digitalWrite(pin_led_reset, HIGH);
    maxX = 0;
    maxY = 0;
    maxZ = 0;
    minTemp = 200;
    maxTemp = -200;
  }
  else
  {
    digitalWrite(pin_led_reset, LOW);
  }
  //reading the values
  x = analogRead(xpin);
  y = analogRead(ypin);
  z = analogRead(zpin);
  if (x < minAccelerometerValue || y < minAccelerometerValue || z < minAccelerometerValue)
  {
    accelero_state = false;
  }
  else
  {
    accelero_state = true;
  }
  if (!accelero_state)
  {
    digitalWrite(pin_axelero_failure, HIGH);
  }
  else
  {
    digitalWrite(pin_axelero_failure, LOW);
  }

  //this is the total voltage
  x = x * refrenceVoltage / 1024.0; //this is in volts
  y = y * refrenceVoltage / 1024.0;
  z = z * refrenceVoltage / 1024.0;
  //substract 0g voltage
  x = (x - zeroGVoltage) / sensitivity;
  y = (y - zeroGVoltage) / sensitivity;
  z = (z - zeroGVoltage) / sensitivity;
  //saving calubrated value
  x = x + xCC;
  y = y + yCC;
  z = z + zCC;

  //calculate tilt angles
  pitch = atan(x / sqrt((y * y) + (z * z)));
  roll = atan(y / sqrt((x * x) + (z * z)));
  pitch = pitch * 180 / 3.14159; //dividing with pi;
  roll = roll * 180 / 3.14159;
  //calculating the max values

  float tempX = x;
  if (tempX < 0)
    tempX *= -1;
  if (tempX > maxX)
    maxX = tempX;

  float tempY = y;
  if (tempY < 0)
    tempY *= -1;
  if (tempY > maxY)
    maxY = tempY;

  float tempZ = z;
  if (tempZ < 0)
    tempZ *= -1;
  if (tempZ > maxZ)
    maxZ = tempZ;

  if (abs(x) + abs(y) + abs(z) > maxTotalAcceleration)
  {
    digitalWrite(pin_axelero_warning, HIGH);
  }
  else
  {
    digitalWrite(pin_axelero_warning, LOW);
  }

  //Thermister
  float ntcReading = analogRead(NTC_pin);
  float vOut = ntcReading / 1023.0 * vInThermistor;
  float r1 = (r2Thermistor * vInThermistor - r2Thermistor * vOut) / vOut;
  temperature = multiMap(r1, resistance, temperatures, 10);
  temperature = temperature + tempCalibrationConst;

  if (temperature < minTemp)
    minTemp = temperature;
  if (temperature > maxTemp)
    maxTemp = temperature;

  if (ntcReading == 0)
  {
    digitalWrite(pin_ntc_Failure, HIGH);
  }
  else
  {
    digitalWrite(pin_ntc_Failure, LOW);
  }
  if (temperature < minAllowedTemp || temperature > maxAllowedTemp || (maxTemp - minTemp) > tempDelta)
  {
    digitalWrite(pin_ntc_Warning, HIGH);
  }
  else
  {
    digitalWrite(pin_ntc_Warning, LOW);
  }
  //TODO finetune rotary encoder
  currentTimeRotaryEncoder = millis();
  if (currentTimeRotaryEncoder >= loopTimeRotaryEncoder + 5)
  {
    encoder_A = digitalRead(pin_A); // Read encoder pins
    encoder_B = digitalRead(pin_B);
    // Serial.print(" Read encoder pin ");
    if ((!encoder_A) && (encoder_A_prev))
    {
      // Serial.print(" Encoder pin changed ");
      // A has gone from high to low
      if (encoder_B)
      {
        // B is high so clockwise
        displayScreen = (displayScreen + 1) % 4;
      }
      else
      {
        // B is low so counter-clockwise
        if (displayScreen == 0)
        {
          displayScreen = 3;
        }
        else
        {
          displayScreen = (displayScreen - 1) % 4;
        }
      }
    }
    encoder_A_prev = encoder_A; // Store value of A for next time
    loopTimeRotaryEncoder = currentTimeRotaryEncoder;
  }
  // Serial.print("displayScreen: ");
  // Serial.println(displayScreen);
  switch (displayScreen)
  {
  case 0:
    displayOutput = "Acc:";
    displayOutput += "X:";
    displayOutput += x;
    displayOutput += " Y:";
    displayOutput += y;
    displayOutput += " Z:";
    displayOutput += z;
    break;
  case 1:
    displayOutput = "Max:";
    displayOutput += "X:";
    displayOutput += maxX;
    displayOutput += " Y:";
    displayOutput += maxY;
    displayOutput += " Z:";
    displayOutput += maxZ;
    break;
  case 2:
    displayOutput = "Pitch: ";
    displayOutput += pitch;
    displayOutput += "  Roll: ";
    displayOutput += roll;
    break;
  case 3:
    displayOutput = "Temp:";
    displayOutput += temperature;
    displayOutput += "  Min:";
    displayOutput += minTemp;
    displayOutput += "  Max:";
    displayOutput += maxTemp;
    break;
  }
  //loopTime=millis();
  currentTimeDisplayOutput = millis();
  if (currentTimeDisplayOutput > loopTimeDisplayOutput + DISPLAY_UPDATE_INTERVAL)
  {
    display(displayOutput);
    loopTimeDisplayOutput = currentTimeDisplayOutput;
  }
  readCommand();
  autoDataTransfer();

  //}
}

// note: the _in array should have increasing values
// source: http://playground.arduino.cc/Main/MultiMap
int multiMap(int val, long *_in, int *_out, uint8_t size)
{
  // take care the value is within range
  // val = constrain(val, _in[0], _in[size-1]);
  if (val <= _in[0])
    return _out[0];
  if (val >= _in[size - 1])
    return _out[size - 1];

  // search right interval
  uint8_t pos = 1; // _in[0] allready tested
  while (val > _in[pos])
    pos++;

  // this will handle all exact "points" in the _in array
  if (val == _in[pos])
    return _out[pos];

  // interpolate in the right segment for the rest
  return (val - _in[pos - 1]) * (_out[pos] - _out[pos - 1]) /
             (_in[pos] - _in[pos - 1]) +
         _out[pos - 1];
}

void display(String value)
{
  char buffer[value.length() + 1] = "";
  value.toCharArray(buffer, sizeof buffer);

  mySerial.write(254); // move cursor to beginning of first line
  mySerial.write(128);

  mySerial.write("                "); // clear display
  mySerial.write("                ");

  mySerial.write(254); // move cursor to beginning of first line
  mySerial.write(128);

  mySerial.write(buffer);
}
void readCommand()
{
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil(STOP);
    // Serial.print("'" + command + "'");
    String commandTrimmed = command.substring(command.indexOf(START) + 1, command.indexOf(CHECKSUM));
    debug("ct0: " + commandTrimmed);
    int req = commandTrimmed.toInt();
    String response = String(START);
    if (command.indexOf(SEPERATOR) == -1)
    {
      switch (req)
      {
      case REQ_TEMP:
        sendResponse(RESP_TEMP, temperature, "RESP_TEMP");
        break;
      case REQ_MAX_TEMP:
        sendResponse(RESP_MAX_TEMP, maxTemp, "RESP_MAX_TEMP");
        break;
      case REQ_MIN_TEMP:
        sendResponse(RESP_MIN_TEMP, minTemp, "RESP_MIN_TEMP");
        break;
      case REQ_CURR_X:
        sendResponse(RESP_CURR_X, x, "RESP_CURR_X");
        break;
      case REQ_CURR_Y:
        sendResponse(RESP_CURR_Y, y, "RESP_CURR_Y");
        break;
      case REQ_CURR_Z:
        sendResponse(RESP_CURR_Z, z, "RESP_CURR_Z");
        break;
      case REQ_MAX_X:
        sendResponse(RESP_MAX_X, maxX, "RESP_MAX_X");
        break;
      case REQ_MAX_Y:
        sendResponse(RESP_MAX_Y, maxY, "RESP_MAX_Y");
        break;
      case REQ_MAX_Z:
        sendResponse(RESP_MAX_Z, maxZ, "RESP_MAX_Z");
        break;
      case REQ_TILT:
        sendResponse(RESP_TILT, tilt, "RESP_TILT");
        break;
      case REQ_PITCH:
        sendResponse(RESP_PITCH, pitch, "RESP_PITCH");
        break;
      case REQ_ROLL:
        sendResponse(RESP_ROLL, roll, "RESP_ROLL");
        break;
      case RESET_MIN_MAX:
        minTemp = 100;
        maxTemp = -100;
        maxX = maxY = maxZ = 0;
        //sednig ack
        sendResponse(ACK, "RESET_MIN_MAX");
        break;
      default:
        debug("nack in 1 if");
        //sending negative ack
        sendResponse(NACK, "response if");
      }
    }
    else
    {
      int reqType = commandTrimmed.substring(0, commandTrimmed.indexOf(SEPERATOR)).toInt();
      //debug("ct1 " + commandTrimmed);
      commandTrimmed = commandTrimmed.substring(commandTrimmed.indexOf(SEPERATOR) + 1);
      //debug("ct2: " + commandTrimmed);
      int data, appConst;
      if (reqType == REQ_PARA)
      {
        debug("if reqpara");
        appConst = commandTrimmed.toInt();
      }
      else if (reqType == SET_PARA)
      {
        debug("if setpara" + commandTrimmed);
        appConst = commandTrimmed.substring(0, commandTrimmed.indexOf(SEPERATOR)).toInt();
        data = commandTrimmed.substring(commandTrimmed.indexOf(SEPERATOR) + 1).toInt();
      }

      //debug(String("Data: reqType: "+String(reqType))
      debug(String("data Req type:") + reqType + " App const: " + appConst + " Data: " + data);

      if (reqType == REQ_PARA)
      {
        debug("Requesting param");
        switch (appConst)
        {
        case MAX_TEMP_TRESHOLD:
          sendResponse(RESP_PARA, MAX_TEMP_TRESHOLD, maxAllowedTemp, "MAX_TEMP_TRESHOLD");
          break;
        case DELTA_TEMP_TRESHOLD:
          sendResponse(RESP_PARA, DELTA_TEMP_TRESHOLD, tempDelta, "DELTA_TEMP_TRESHOLD");
          break;
        case MAX_ACC_TRESHOLD:
          sendResponse(RESP_PARA, MAX_ACC_TRESHOLD, maxTotalAcceleration, "MAX_ACC_TRESHOLD");
          break;
        default:
          debug("NACK in req para");
          //sending negative ack
          sendResponse(NACK, "");
        }
      }
      if (reqType == SET_PARA)
      {
        switch (appConst)
        {
        case MAX_TEMP_TRESHOLD:
          maxAllowedTemp = data;
          sendResponse(ACK, "MAX_TEMP_TRESHOLD");
          break;
        case DELTA_TEMP_TRESHOLD:
          tempDelta = data;
          sendResponse(ACK, "DELTA_TEMP_TRESHOLD");
          break;
        case MAX_ACC_TRESHOLD:
          maxTotalAcceleration = data;
          sendResponse(ACK, "MAX_ACC_TRESHOLD");
          break;
        case CALIBRATION_TEMPERATURE:
          tempCalibrationConst = data - temperature;
          sendResponse(ACK, "CALIBRATION_TEMPERATURE");
          break;
        case HUMAN_REDABLE_OP:
          if (data == 1)
            humanRedableOP = true;
          else
            humanRedableOP = false;
          sendResponse(ACK, "HUMAN_REDABLE_OP");
          break;
        case AUTOMATIC_DATA_TRANSMISSION:
          if (data == 1)
            autoDataTrans = true;
          else
            autoDataTrans = false;
          sendResponse(ACK, "AUTOMATIC_DATA_TRANSMISSION");
          break;
        case CALIBRATION_X:
          xCC = data - x;
          sendResponse(ACK, "CALIBRATION_X");
          break;
        case CALIBRATION_Y:
          yCC = data - y;
          sendResponse(ACK, "CALIBRATION_Y");
          break;
        case CALIBRATION_Z:
          zCC = data - z;
          sendResponse(ACK, "CALIBRATION_Z");
          break;
        default:
          debug("NACK in set para");
          //sending negative ack
          sendResponse(NACK, "SET_PARA");
        }
      }
    }
  }
}

void sendResponse(int param, String name)
{
  if (humanRedableOP)
  {
    String op = name;
    op += " " + String(param);
    Serial.println(op);
  }
  else
  {
    String response = String(START);
    response += param;
    response += CHECKSUM;
    response += STOP;
    Serial.println(response);
  }
}
void sendResponse(int param, float data, String name)
{
  if (humanRedableOP)
  {
    String op = name;
    op += " " + String(data);
    Serial.println(op);
  }
  else
  {
    String response = String(START);
    response += param;
    response += SEPERATOR;
    response += data;
    response += CHECKSUM;
    response += STOP;
    Serial.println(response);
  }
}
void sendResponse(int response_param, int param, float data, String name)
{
  if (humanRedableOP)
  {
    String op = name;
    op += " " + String(data);
    Serial.println(op);
  }
  else
  {
    String response = String(START);
    response += response_param;
    response += SEPERATOR;
    response += param;
    response += SEPERATOR;
    response += data;
    response += CHECKSUM;
    response += STOP;
    Serial.println(response);
  }
}
void debug(String msg)
{
  if (isDebug)
  {
    Serial.println(msg);
  }
}
void autoDataTransfer()
{
  if (autoDataTrans)
  {
    sendResponse(RESP_TEMP, temperature, "RESP_TEMP");
    sendResponse(RESP_MIN_TEMP, minTemp, "RESP_MIN_TEMP");
    sendResponse(RESP_MAX_TEMP, maxTemp, "RESP_MAX_TEMP");
    sendResponse(RESP_CURR_X, x, "RESP_CURR_X");
    sendResponse(RESP_CURR_Y, y, "RESP_CURR_Y");
    sendResponse(RESP_CURR_Z, z, "RESP_CURR_Z");
    sendResponse(RESP_MAX_X, maxX, "RESP_MAX_X");
    sendResponse(RESP_MAX_Y, maxY, "RESP_MAX_Y");
    sendResponse(RESP_MAX_Z, maxZ, "RESP_MAX_Z");
    //sendResponse(RESP_TILT, tilt, "RESP_TILT");
    sendResponse(RESP_ROLL, roll, "RESP_ROLL");
    sendResponse(RESP_PITCH, pitch, "RESP_PITCH");
  }
}
