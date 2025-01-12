#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <stdint.h>
#include <string.h>


#define MG996r_MIN 100
#define MG996r_MAX 490
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

//I really need to get better at C

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

double sholder = 0;
double elbow = 0;
double wrist = 0;

const byte numChars = 64;
char receivedChars[numChars];

boolean newData = false;

void setup() {
  Serial.begin(115200);
  //Serial.begin(9600);
  Serial.println("started");


  // put your setup code here, to run once:
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

  delay(10);
  Serial.println("listening");
}

void loop() {
  // put your main code here, to run repeatedly:
  recvWithStartEndMarkers();
  showNewData();
}

void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
 
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();

        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
                //Serial.println("message ended");
            }
        }

        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void showNewData() {
    if (newData == true) {
        //Serial.print("This just in ... ");
        //Serial.println(receivedChars);
        newData = false;

        char* things = strtok(receivedChars, ',');

        parse(receivedChars);
        //Serial.println(sholder);
        //Serial.println(elbow);
        //Serial.println(wrist);

        pwm.setPWM(1, 0, angle_to_pulse(sholder));
        pwm.setPWM(2, 0, angle_to_pulse(elbow));
        pwm.setPWM(3, 0, angle_to_pulse(wrist));
    }
}

void parse(char * data) {
   char * strtokIndx;


  strtokIndx = strtok(data,",");      // get the first part - the string
  strcpy(data, strtokIndx); // copy it to messageFromPC
  
  //strtokIndx = strtok(NULL, ","); // this continues where the previous call left off
  sholder = atof(strtokIndx);     // convert this part to an integer
  
  strtokIndx = strtok(NULL, ","); 
  elbow = atof(strtokIndx);     // convert this part to a float

  strtokIndx = strtok(NULL, ","); 
  wrist = atof(strtokIndx);     // convert this part to a float


}

uint16_t angle_to_pulse(double angle) {
  //slope derived from (x * (max-min)) / 100. Division is borked on this board for some reason
  uint32_t val = round(angle * 2.166)+100;
  if (val > 490) val = 490;
  
  return val;
}