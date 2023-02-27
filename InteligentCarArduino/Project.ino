#include <Servo.h>
#include <SoftwareSerial.h>
#include "pitches.h"
#include <Adafruit_NeoPixel.h>
#include <NewPing.h>
//Pinii motor 1
#define mpin00 5
#define mpin01 6
// Pinii motor 2
#define mpin10 3
#define mpin11 11

#define pinBuzzer 4

#define pinServo 9

#define pinNeopixel 10
#define numPixels 12

#define trigPin 2
#define echoPin 7

const unsigned long servoPeriod = 10;
unsigned long startServoMillis;
unsigned long currentServoMillis;

const unsigned long ledsPeriod = 15;
unsigned long startLedsMillis;
unsigned long currentLedsMillis;


unsigned long startCheckDistanceMillis;
unsigned long startTrigLowMillis;
unsigned long startTrigHighMillis;
unsigned const long distanceTrigLowPeriod = 2;
unsigned const long distanceTrigHighPeriod = 10;
unsigned const long checkDistancePeriod = 20;
unsigned long currentCheckDistanceMillis;
unsigned long currentTrigLowMillis;
unsigned long currentTrigHighMillis;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(numPixels, pinNeopixel, NEO_GRB + NEO_KHZ800);

NewPing sonar(trigPin, echoPin, 200);

SoftwareSerial Bluetooth(12, 13); // RX | TX 
Servo srv;

int hornNote = NOTE_C4;

int servoPosition = 110;

int distanceToObject = 30;

char flag;

char horn = 'v';

void setup() {
 //configurarea pinilor motor ca iesire, initial valoare 0
  digitalWrite(mpin00, 0);
  digitalWrite(mpin01, 0);
  digitalWrite(mpin10, 0);
  digitalWrite(mpin11, 0);
  pinMode (mpin00, OUTPUT);
  pinMode (mpin01, OUTPUT);
  pinMode (mpin10, OUTPUT);
  pinMode (mpin11, OUTPUT);

  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);

  Serial.begin(9600);
  Bluetooth.begin(9600);
  srv.attach(pinServo);

  strip.begin();
  strip.setBrightness(50);
  strip.show();

  startServoMillis = millis();

  startLedsMillis = millis();

  startCheckDistanceMillis = millis();
  // startTrigLowMillis = millis();
  // startTrigHighMillis = millis();
}

void loop() {
  currentServoMillis = millis();

  currentLedsMillis = millis();

  currentCheckDistanceMillis = millis();
  // currentTrigLowMillis = millis();
  // currentTrigHighMillis = millis();

  if (Bluetooth.available()){ 
   flag = Bluetooth.read();
  }

  playWithMotors(flag);

  if(flag == 'V'){
    horn = 'V';
  }
  if(flag == 'v'){
    horn = 'v';
  }

  playHorn(horn);
  if(currentCheckDistanceMillis - startCheckDistanceMillis >= checkDistancePeriod){
    distanceToObject = sonar.ping_cm();
    startCheckDistanceMillis = millis();
  }
  
} 


void StartMotor (int m1, int m2, int forward, int speed){
  if (speed==0){ //oprire
    digitalWrite(m1, 0);
    digitalWrite(m2, 0);
  }
  else{
    if (forward){
      digitalWrite(m2, 0);
      analogWrite(m1, speed); // folosire PWM
    }
    else{
      digitalWrite(m1, 0);
      analogWrite(m2, speed);
    }
  }
}

void delayStopped(){
  StartMotor (mpin00, mpin01, 0, 0);
  StartMotor (mpin10, mpin11, 0, 0);
  delay(5);
}


void moveServoDefault(){
   if(servoPosition > 92){
        servoPosition -= 2;
      }
    if(servoPosition < 98){
      servoPosition += 2;
    }
}

void playWithMotors(char flag){

  switch(flag){
    case 'L':{
      if(currentServoMillis - startServoMillis >= servoPeriod){
        startServoMillis = currentServoMillis;
        if(servoPosition < 180) {
          servoPosition += 2;
        }
        srv.write(servoPosition);
      }
      showLedsLeft();
      break;
    }
    case 'R':{
      if(currentServoMillis - startServoMillis >= servoPeriod){
        startServoMillis = currentServoMillis;
        if(servoPosition > 0) {
          servoPosition -= 2;
        }
        srv.write(servoPosition);
      }
      showLedsRight();
      break;
    }
    case 'I':{ //ForwardRight
      StartMotor (mpin00, mpin01, 0, 128); 
      StartMotor (mpin10, mpin11, 0, 128);    
      showLedsRight();
      break;
    }
    case 'G':{  //ForwardLeft
      StartMotor (mpin00, mpin01, 1, 128);
      StartMotor (mpin10, mpin11, 1, 128);
      showLedsLeft();
      break;
    }
    case 'F':{  //Forward
      if(distanceToObject < 20 && distanceToObject != 0){
        showLedsObjectDetected();
        if(servoPosition < 135 && servoPosition > 45){
          break;
        }
      }
      StartMotor (mpin00, mpin01, 0, 128);
      StartMotor (mpin10, mpin11, 1, 128);
      showLedsForward();
      break;
    }
    case 'B':{ //Backward
      StartMotor (mpin00, mpin01, 1, 128);
      StartMotor (mpin10, mpin11, 0, 128);
      showLedsBackward();
      break;
    }
    case 'H':{ //BackwardLeft
      StartMotor (mpin00, mpin01, 0, 128);
      StartMotor (mpin10, mpin11, 0, 128);
      showLedsLeft();
      break;
    }
    case 'J':{  //BackwardRight
      StartMotor (mpin00, mpin01, 1, 128); 
      StartMotor (mpin10, mpin11, 1, 128);
      showLedsRight();
      break;
    }
    default :{
      delayStopped();
       if(currentServoMillis - startServoMillis >= servoPeriod){
        startServoMillis = currentServoMillis;
        moveServoDefault();
        srv.write(servoPosition);
      }

      if(distanceToObject < 20 && distanceToObject != 0){
        showLedsObjectDetected();
      }
      else{
          showLedsDefault();
      }
      break;
    }
  }
}


void showLedsRight(){
  if(currentLedsMillis - startLedsMillis >= ledsPeriod){
    startLedsMillis = currentLedsMillis;
    strip.clear();
    strip.setPixelColor(8, strip.Color(0, 255, 0));
    strip.setPixelColor(9, strip.Color(0, 255, 0));
    strip.setPixelColor(10, strip.Color(0, 255, 0));
    strip.show();
  }
}

void showLedsLeft(){
  if(currentLedsMillis - startLedsMillis >= ledsPeriod){
    startLedsMillis = currentLedsMillis;
    strip.clear();
    strip.setPixelColor(2, strip.Color(0, 255, 0));
    strip.setPixelColor(3, strip.Color(0, 255, 0));
    strip.setPixelColor(4, strip.Color(0, 255, 0));
    strip.show();
  }
}

void showLedsForward(){
  if(currentLedsMillis - startLedsMillis >= ledsPeriod){
    startLedsMillis = currentLedsMillis;
    strip.clear();
    strip.setPixelColor(5, strip.Color(0, 255, 0));
    strip.setPixelColor(6, strip.Color(0, 255, 0));
    strip.setPixelColor(7, strip.Color(0, 255, 0));
    strip.show();
  }
}

void showLedsBackward(){
  if(currentLedsMillis - startLedsMillis >= ledsPeriod){
    startLedsMillis = currentLedsMillis;
    strip.clear();
    strip.setPixelColor(0, strip.Color(0, 255, 0));
    strip.setPixelColor(1, strip.Color(0, 255, 0));
    strip.setPixelColor(11, strip.Color(0, 255, 0));
    strip.show();
  }
}

void showLedsDefault(){
  if(currentLedsMillis - startLedsMillis >= ledsPeriod){
    strip.clear();
    startLedsMillis = currentLedsMillis;
    for(int i = 0; i < numPixels; i++){
      strip.setPixelColor(i, strip.Color(0, 255, 0));
      strip.show();
    }
  }
}

void showLedsObjectDetected(){
  if(currentLedsMillis - startLedsMillis >= ledsPeriod){
    startLedsMillis = currentLedsMillis;
    strip.clear();
    strip.clear();
    for(int i = 0; i < numPixels; i++){
      strip.setPixelColor(i, strip.Color(255, 0, 0));
      strip.show();
    }
  }
}

void playHorn(char horn){
  if(horn == 'V'){
    int noteDuration = 1000/4;
    tone(pinBuzzer, hornNote, 100);
  }
}

