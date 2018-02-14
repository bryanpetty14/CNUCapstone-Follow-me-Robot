#include <AFMotor.h>
#include <Servo.h>
//CREATING MOTOR OBJECTS//
AF_DCMotor motorone(1, MOTOR12_64KHZ);
AF_DCMotor motortwo(2, MOTOR12_64KHZ);
AF_DCMotor motorthree(3, MOTOR12_64KHZ);
AF_DCMotor motorfour(4, MOTOR12_64KHZ);
Servo servo;
//GLOBAL VARIABLES//
bool bools = false;
int speed = 0;
int dir = 0;
int minValue = 100;
long distance = 0;
const int trigPin = A1;
const int echoPin = A0;
bool turL = false;
bool turR = false;
double degree[3];
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
void sonarturn() {
  if (turL == true) {
    servo.write(90);
    sonar();
    delay(100);
    servo.write(-90);
    turL = false;
  } else if (turR == true) {
    servo.write(-90);
    sonar();
    delay(100);
    servo.write(90);
    turR = false;
  }
}
void driveFwd() {
  if (speed < 150) {
    bools = true;
    changeSpeed();
  }
  motorone.run(FORWARD);
  motortwo.run(FORWARD);
  motorthree.run(FORWARD);
  motorfour.run(FORWARD);
}
void reverse() {
  if (speed < 150) {
    bools = true;
    changeSpeed();
  }
  motorone.run(BACKWARD);
  motortwo.run(BACKWARD);
  motorthree.run(BACKWARD);
  motorfour.run(BACKWARD);
}
void turnRight() {
  if (speed > 150) {
    bools = false;
    changeSpeed();
  }
  motorone.run(BACKWARD);
  motortwo.run(BACKWARD);
  motorthree.run(FORWARD);
  motorfour.run(FORWARD);
}
void turnLeft() {
  if (speed > 150) {
    bools = false;
    changeSpeed();
  }
  motorone.run(FORWARD);
  motortwo.run(FORWARD);
  motorthree.run(BACKWARD);
  motorfour.run(BACKWARD);
}
void changeSpeed() {
  if (bools == false) {
    speed = 120;
    motorone.setSpeed(speed);
    motortwo.setSpeed(speed);
    motorthree.setSpeed(speed + 20);
    motorfour.setSpeed(speed);
  } else if (bools == true) {
    speed = 200;
    motorone.setSpeed(speed);
    motortwo.setSpeed(speed);
    motorthree.setSpeed(speed + 20);
    motorfour.setSpeed(speed);
  }
}
void onwards() {
  int west [180];
  int south [180];
  int east [180];
  int north [180];
  int sum1 = 0;
  int sum2 = 0;
  int sum3 = 0;
  int sum4 = 0;
  int minValue = 100;
  for (int j; j < 180; j++) {
    west[j] = analogRead(2);
    south[j] = analogRead(3);
    east[j] = analogRead(4);
    north[j] = analogRead(5);
    delay(5);
    if (west[j] <= minValue) {
      sum1++;
    }
    if (east[j] <= minValue) {
      sum2++;
    }
    if (north[j] <= minValue) {
      sum3++;
    }
    if (south[j] <= minValue) {
      sum4++;
    }
  }
  double degree = 90.0;
  if (sum3 != 0) {
    if (sum2 > sum1) {
      degree = atan2(sum2, sum3);
      degree *= 180/3.1415;
    }
    else {
      degree = atan2(sum1, sum3);
      degree *= 180/3.1415;
    }
  }

  Serial.print("North : ");
  Serial.println(sum3);
  Serial.print("West : ");
  Serial.println(sum1);
  Serial.print("East : ");
  Serial.println(sum2); +
  Serial.print("South : ");
  Serial.println(sum4);
  Serial.print("Degree : ");
  Serial.println(degree);
  Serial.println("");
  sum1 = 0;
  sum2 = 0;
  sum3 = 0;
  sum4 = 0;
}
void loop() {
  sonar();
  if (distance > 10) {
    onwards();
  } else {
    stop();//object too close to the robot
  }
}
