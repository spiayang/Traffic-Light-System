#include <IRremote.h>
#include <Servo.h>

/* Names: Sean Yang, Sophia Yang, David Shim
   Teacher: Mr. Wong
   Date: April 6, 2021
   Assignment: ISP Circuit
*/

// constants
const int LDRsensor = A0;
const int pushbutton = 2;
const int DIPone = A1;
const int DIPtwo = A2;
const int IRsensor = 3;
const int echoPin = 5;
const int trigPin = 6;
const int greenLed = 8;
const int yellowLed = 9;
const int redLed = 10;
const int whiteLed = 11;

// variables

// tarrif light variables
int defRed; // 3 seconds = 3000 milliseconds
int defGreen; // 1.5 seconds = 1500 milliseconds
int defYellow; // 1.5 seconds = 1500 milliseconds
bool resetSkip = false;
unsigned long redTimer;
unsigned long yellowTimer;
unsigned long greenTimer;
int trafficState = 1; // 1 = red, 2 = yellow, 3 = green

// ultrasonic & servo variables
Servo servo;
int minDist;
int maxDist;
int Udist;
unsigned long servoTimer;
int servoState = 1; //1 = ready, 2 = wait 2 seconds, 3 = wait 1.5 seconds

void setup() {
  pinMode(LDRsensor, INPUT);
  pinMode(pushButton, INPUT);
  pinMode(IRSensor, INPUT);
  pinMode(DIPone, INPUT);
  pinMode(DIPtwo, INPUT);
  servo.attach(4);
  servo.write(90);
  pinMode(echoPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(whiteLed, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Street Lights
  if (analogRead(LDRsensor) < 1014) {
    digitalWrite(whiteLed, HIGH);
  }
  else {
    digitalWrite(whiteLed, LOW);
  }

  // Traffic Lights
  // Reset light values
  if (resetSkip == false) {
    defRed = 3000; // 3 seconds = 3000 milliseconds
    defGreen = 1500; // 1.5 seconds = 1500 milliseconds
    defYellow = 1500; // 1.5 seconds = 1500 milliseconds
  }
  else {
    resetSkip = false;
  }

  // Red
  if (trafficState == 1) {
    if (digitalRead(redLed) == LOW) {
      digitalWrite(redLed, HIGH);
      redTimer = millis() + defRed;
    }
    if(digitalRead(pushButton) == HIGH) {
      redTimer = redTimer - ((redTimer - millis()) / 2);
      defYellow = 750;
      defGreen = 2250;
    }
    if ((digitalRead(redLed) == HIGH) && (millis() >= redTimer)) {
      digitalWrite(redLed, LOW);
      trafficState = 2;
    }
  }

  // Yellow
  if (trafficState == 2) {
    if (digitalRead(yellowLed) == LOW) {
      digitalWrite(yellowLed, HIGH);
      yellowTimer = millis() + defYellow;
    }
    if(digitalRead(pushButton) == HIGH) {
      yellowTimer = yellowTimer - ((yellowTimer - millis()) / 2);
      defGreen = 2250;
    }
    if ((digitalRead(yellowLed) == HIGH) && (millis() >= yellowTimer)) {
      digitalWrite(yellowLed, LOW);
      trafficState = 3;
    }
  }

  // Green
  if (trafficState == 3) {
    if (digitalRead(greenLed) == LOW) {
      digitalWrite(greenLed, HIGH);
      greenTimer = millis() + defGreen;
    }
    if(digitalRead(pushButton) == HIGH) {
      defRed = 1500;
      defYellow = 750;
      defGreen = 2250;
      resetSkip = true;
    }
    if ((digitalRead(greenLed) == HIGH) && (millis() >= greenTimer)) {
      digitalWrite(greenLed, LOW);
      trafficState = 1;
    }
  }

  // Servo and ultrasonic sensor (IR Sensor check digital.read(IRsensor) != 1)

  //clean trigpin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  //pulse 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(1);
  digitalWrite(trigPin, LOW);
  //read echo
  Udist = (pulseIn(echoPin, HIGH)) * 0.034613 / 2;

  //check DIP pin position
  if (digitalRead(DIPone) == LOW && digitalRead(DIPtwo) == HIGH) {
    minDist = 140;
    maxDist = 190;
  }
  else if (digitalRead(DIPone) == HIGH && digitalRead(DIPtwo) == LOW) {
    minDist = 130;
    maxDist = 180;
  }
  else if (digitalRead(DIPone) == HIGH && digitalRead(DIPtwo) == HIGH) {
    minDist = 120;
    maxDist = 170;
  }
  else {
    minDist = 150;
    maxDist = 200;
  }

  //check servo ready
  if(((Udist >= minDist && Udist <= maxDist) && servoState == 1) || digitalRead(IRsensor) != 1) {
    servoTimer = millis() + 2000;
    servoState = 2;
  }
  else if (servoState == 2 && (millis() >= servoTimer)) {
    servo.write(60);
    servoTimer = millis() + 1500;
    servoState = 3;
  }
  else if (servoState == 3 && (millis() >= servoTimer)) {
    servo.write(90);
    servoState = 1;
  }
  Serial.print(servoState);
  Serial.println(digitalRead(IRsensor));

  // Delay a little bit to improve simulation performance
  delay(10);
}



