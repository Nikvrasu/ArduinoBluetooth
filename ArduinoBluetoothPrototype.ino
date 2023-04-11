#include <SoftwareSerial.h>
#include <String.h>

//Motoren
const int motor1pin1 = 2;
const int motor1pin2 = 3;

const int motor2pin1 = 4;
const int motor2pin2 = 7;

const int speed1 = 5;
const int speed2 = 6;

const int speedRechts = 150;
const int speedLinks = 160;

//Ultraschall1
const int triggerRechts = 8;
const int echoA = 9;
long entfernungRechts = 0;

//Ultraschall2
const int triggerLinks = 10;
const int echoB = 11;
long entfernungLinks = 0;

//Ultraschall3
const int triggerVorne = 0;
const int echoC = 1;
const long errorMargin = 3;
long entfernungVorne = 0;

// Bluetooth

int blueToothVal = 0;
char lastValue;

// Timer

int counter = 0;

SoftwareSerial mySerial(12, 13);


long measureDistance(int trigger, int echo) {
  int entfernung = 0;
  int dauer = 0;
  digitalWrite(trigger, HIGH);
  delay(10);
  digitalWrite(trigger, LOW);
  dauer = pulseIn(echo, HIGH);
  entfernung = (dauer / 2) * 0.03432;
  delay(20);
  return entfernung;
}

void setup() {
  mySerial.begin(9600);  //38400 für hc-05

  pinMode(motor1pin1, OUTPUT);
  pinMode(motor1pin2, OUTPUT);
  pinMode(motor2pin1, OUTPUT);
  pinMode(motor2pin2, OUTPUT);

  pinMode(speed1, OUTPUT);
  pinMode(speed2, OUTPUT);

  pinMode(triggerRechts, OUTPUT);
  pinMode(echoA, INPUT);
  pinMode(triggerLinks, OUTPUT);
  pinMode(echoB, INPUT);
  pinMode(triggerVorne, OUTPUT);
  pinMode(echoC, INPUT);

  digitalWrite(motor1pin1, LOW);  // LOW HIGH LOW HIGH ist beide vorwaerts
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void reverse() {
  analogWrite(speed1, 0);
  analogWrite(speed2, 0);
  int richtung = (entfernungRechts > entfernungLinks);

  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);

  analogWrite(speed1, 150);
  analogWrite(speed2, 150);

  delay(900);

  analogWrite(speed1, speedRechts);
  analogWrite(speed2, speedLinks);

  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);

  if (richtung) {

    analogWrite(speed1, 0);
    analogWrite(speed2, 150);
  } else {

    analogWrite(speed1, 150);
    analogWrite(speed2, 0);
  }

  delay(500);
}

void forward() {
  analogWrite(speed1, speedLinks * 0.75);
  analogWrite(speed2, speedRechts * 0.75);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void left() {
  analogWrite(speed1, speedLinks * 0.75);
  analogWrite(speed2, 0);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void right() {
  analogWrite(speed1, 0);
  analogWrite(speed2, speedRechts * 0.75);
  digitalWrite(motor1pin1, LOW);
  digitalWrite(motor1pin2, HIGH);
  digitalWrite(motor2pin1, LOW);
  digitalWrite(motor2pin2, HIGH);
}

void backwards() {
  analogWrite(speed1, speedLinks * 0.75);
  analogWrite(speed2, speedRechts * 0.75);
  digitalWrite(motor1pin1, HIGH);
  digitalWrite(motor1pin2, LOW);
  digitalWrite(motor2pin1, HIGH);
  digitalWrite(motor2pin2, LOW);
}

void stop() {
  analogWrite(speed1, 0);
  analogWrite(speed2, 0);
  delay(50);
}


void loop() {

  if (mySerial.available()) {
    blueToothVal = mySerial.read();
  }

  if (blueToothVal == 1) {  // 1 = selbstfahrend
    entfernungVorne = measureDistance(triggerVorne, echoC) - errorMargin;
    entfernungRechts = measureDistance(triggerRechts, echoA);
    entfernungLinks = measureDistance(triggerLinks, echoB);

    if (entfernungRechts <= 10) {
      mySerial.print("r");
    } else {
      mySerial.print("z");
    }
    if (entfernungLinks <= 10) {
      mySerial.print("l");
    } else {
      mySerial.print("y");
    }

    if (entfernungVorne <= 10) {
      mySerial.print("v");

      reverse();
    } else {
      mySerial.print("x");
    }
    analogWrite(speed1, speedLinks);
    analogWrite(speed2, speedRechts);
    delay(100);
  } else if (blueToothVal == 0){
    stop();
  } else if(blueToothVal == 5 || blueToothVal == 6 || blueToothVal == 7 || blueToothVal == 8 || blueToothVal == 9) {  // 5 - vorne, 6 - links, 7 - rechts, 8 - stop, 9 - hinten.
    switch (blueToothVal) {
      case 5:
        forward();
        break;
      case 6:
        left();
        break;
      case 7:
        right();
        break;
      case 8:
        stop();
        break;
      case 9:
        backwards();
        break;
    }
    entfernungVorne = measureDistance(triggerVorne, echoC) - errorMargin;
    entfernungRechts = measureDistance(triggerRechts, echoA);
    entfernungLinks = measureDistance(triggerLinks, echoB);
    if (counter == 2) {
      if (entfernungRechts <= 20) {
        mySerial.print("r");
      } else {
        mySerial.print("z");
      }
      if (entfernungLinks <= 20) {
        mySerial.print("l");
      } else {
        mySerial.print("y");
      }
      if (entfernungVorne <= 20) {
        mySerial.print("v");
      } if(entfernungVorne <= 13){
        mySerial.print("n");
        backwards();
        delay(600);
        mySerial.print("m");
        blueToothVal = 8;
      } else {
        mySerial.print("x");
      }
      counter = 0;
    }
    counter++;
  }
}
