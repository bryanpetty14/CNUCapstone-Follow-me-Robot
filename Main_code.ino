#include <AFMotor.h>
#include <Servo.h>
//CREATING MOTOR OBJECTS//
AF_DCMotor motorone(1, MOTOR12_64KHZ);
AF_DCMotor motortwo(2, MOTOR12_64KHZ);
AF_DCMotor motorthree(3, MOTOR12_64KHZ);
AF_DCMotor motorfour(4, MOTOR12_64KHZ);
Servo servo;

//GLOBAL VARIABLES//
bool boolsN, boolsS, boolsW = false;
int speed = 0;
int minValue = 100;
long distance = 0;
const int trigPin = A1;
const int echoPin = A0;
double aveDeg[3] = { -1, -1, -1};

void setup() {
  speed = 0;
  motorone.setSpeed(speed);
  motortwo.setSpeed(speed);
  motorthree.setSpeed(speed);
  motorfour.setSpeed(speed);
  Serial.begin(9600);
}
void stop() {
  motorone.run(RELEASE);
  motortwo.run(RELEASE);
  motorthree.run(RELEASE);
  motorfour.run(RELEASE);
}
long microsecondsToInches(long microseconds) {
  return microseconds / 74 / 2;
}
long microsecondsToCentimeters(long microseconds) {
  return microseconds / 29 / 2;
}
void sonar() {
  int duration = 0;
  pinMode(trigPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH);
  distance = microsecondsToInches(duration);
  duration = 0;
}

void changeSpeed(int degree) {
  int speedLeft = 255;
  int speedRight = 255;
  if (degree>90) {//if the person is to the left

    /*if (boolsW == true) {
      motorone.setSpeed(speed);
      motortwo.setSpeed(speed);
    } else {
      motorthree.setSpeed(speed);
      motorfour.setSpeed(speed);
    }

    boolsW = false;
    boolsN = false;*/
    speedLeft -= floor(2.8*(degree-90));//adjust motors on the left
  } else {//if the person is to the right or in front

    /*if (boolsW == true) {
      motorone.setSpeed(speed);
      motortwo.setSpeed(speed);
    } else {
      motorthree.setSpeed(speed);
      motorfour.setSpeed(speed);
    }
    boolsW = false;
    boolsS = false;*/
    speedRight -= floor(2.8*(90-degree));//adjust motors on the left
  }
}

void onwards() {
  int west [180], south [180], east [180], north [180];
  int sum, sumWest, sumEast, sumNorth, sumSouth = 0;
  int minValue = 100;
  double degree, weightedAve = 0;
  for (int j; j < 180; j++) {
    west[j] = analogRead(2);
    south[j] = analogRead(3);
    east[j] = analogRead(4);
    north[j] = analogRead(5);
    delay(4);
    if (west[j] <= minValue) {
      sumWest++;
    }
    if (east[j] <= minValue) {
      sumEast++;
    }
    if (north[j] <= minValue) {
      sumNorth++;
    }
    if (south[j] <= minValue) {
      sumSouth++;
    }
  }
  degree = 90.0;

  if (sumWest == 0 && sumEast == 0 && sumNorth == 0 && sumSouth == 0) { //device off
    //reset degree values
    degree = -1;
    aveDeg[0] = -1;
    aveDeg[1] = -1;
    stop();
  } else if (sumNorth != 0) {//detected something north
    if (sumEast > sumWest) {//check if east is greater than west
      if (aveDeg[2] == 180) { //if just came from west
        aveDeg[2] = -1;//make the last value not count
      }
      degree = atan2(sumEast, sumNorth);//take the arctan of opposite over adjacent
      degree *= 180 / 3.1415; //convert to degrees
      degree = 90 - degree;//adjust to find the new angle
    }
    else { //east is not bigger, use west
      if (aveDeg[2] == 0) { //if just came from east
        aveDeg[2] = -1;//make the last value not count
      }
      degree = atan2(sumWest, sumNorth);//take arctan of opposite over adjacent on left side(if degree = 0 after,then north has value and east and west are 0)
      degree *= 180 / 3.1415; //convert (if 0, will still be 0)
      degree += 90;//adjust degree to new angle
    }
  } /*else if (sumSouth != 0) { //Detected something south
    if (sumEast > sumWest) {//check if east is greater than west
      degree = atan2(sumEast, sumNorth);//take the arctan of opposite over adjacent
      degree *= 180 / 3.1415; //convert to degrees
      degree = 180 - degree;//adjust to find the new angle
    }
    else { //east is not bigger, use west
      degree = atan2(sumWest, sumNorth);//take arctan of opposite over adjacent on left side(if degree = 0 after,then north has value and east and west are 0)
      degree *= 180 / 3.1415; //convert (if 0, will still be 0)
      degree += 180;//adjust degree to new angle
    }
  }*/ else if (sumEast > sumWest) { //east has greater value than west
    //if(sonar check)
    aveDeg[0] = -1;
    aveDeg[1] = -1;
    degree = 0;
  } else { //west has the greater value than east
    //if(sonar Check)
    aveDeg[0] = -1;
    aveDeg[1] = -1;
    degree = 180;
  }


  //shift values
  aveDeg[0] = aveDeg[1];
  aveDeg[1] = aveDeg[2];
  aveDeg[2] = degree;

  //declare variables for running average
  sum = 0;
  weightedAve = 0;

  //find the valid values and add them to the average;
  for (int j = 0; j < 3; j++) {
    if (aveDeg[j] != -1) {
      weightedAve += aveDeg[j];
      sum++;
    }
  }
  //weight the last value by x2
  if (aveDeg[2] != -1) {
    weightedAve += aveDeg[2];
    sum++;
  }
  //check to see if any values were valid and then average the values
  if (sum != 0) {
    weightedAve /= sum;
  } else {
    weightedAve = -1;
  }

  Serial.print("North : ");
  Serial.println(sumNorth);
  Serial.print("West : ");
  Serial.println(sumWest);
  Serial.print("East : ");
  Serial.println(sumEast); +
  Serial.print("South : ");
  Serial.println(sumSouth);
  Serial.print("Degree(weighted average : ");
  Serial.println(weightedAve);
  Serial.println("");

  sumWest = 0;
  sumEast = 0;
  sumNorth = 0;
  sumSouth = 0;
}
void loop() {
  sonar();
  if (distance > 10) {
    onwards();
  } else {
    stop();//object too close to the robot
  }
}
