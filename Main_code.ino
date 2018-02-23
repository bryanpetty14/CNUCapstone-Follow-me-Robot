
#include <AFMotor.h>
#include <Servo.h>
//CREATING MOTOR OBJECTS//
AF_DCMotor motorone(1, MOTOR12_64KHZ);
AF_DCMotor motortwo(2, MOTOR12_64KHZ);
AF_DCMotor motorthree(3, MOTOR12_64KHZ);
AF_DCMotor motorfour(4, MOTOR12_64KHZ);
Servo servo;

//GLOBAL VARIABLES//
int minValue = 100;
long distance = 0;
const int trigPin = A1;
const int echoPin = A0;
double aveDeg[3] = { -1, -1, -1};

void setup() {
  motorone.setSpeed(0);
  motortwo.setSpeed(0);
  motorthree.setSpeed(0);
  motorfour.setSpeed(0);
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

void changeSpeedSpecial(int degree) {
  if (degree == 0) {
    motorone.setSpeed(150);
    motortwo.setSpeed(150);
    motorthree.setSpeed(100);
    motorfour.setSpeed(100);
    motorone.run(FORWARD);
    motortwo.run(FORWARD);
    motorthree.run(BACKWARD);
    motorfour.run(BACKWARD);
  }
  else if (degree == 180) {
    motorone.setSpeed(100);
    motortwo.setSpeed(100);
    motorthree.setSpeed(150);
    motorfour.setSpeed(150);
    motorone.run(BACKWARD);
    motortwo.run(BACKWARD);
    motorthree.run(FORWARD);
    motorfour.run(FORWARD);
  }
  else {
    motorone.setSpeed(255);
    motortwo.setSpeed(255);
    motorthree.setSpeed(255);
    motorfour.setSpeed(255);
    motorone.run(FORWARD);
    motortwo.run(FORWARD);
    motorthree.run(FORWARD);
    motorfour.run(FORWARD);
  }
}

void changeSpeed(int degree) {
  int speedLeft = 255;
  int speedRight = 255;
  if (degree == -1) {
    stop();
    return;
  }
  if (degree > 90) { //if the person is to the left

    speedLeft -= floor(5 * (degree - 90)); //adjust motors on the left
    if (speedLeft < 0) {
      speedLeft = 0;
    }
  } else {//if the person is to the right or in front

    speedRight -= floor(5 * (90 - degree)); //adjust motors on the left
    if (speedRight < 0) {
      speedRight = 0;
    }
  }
  motorone.setSpeed(speedRight);
  motortwo.setSpeed(speedRight);
  motorthree.setSpeed(speedLeft);
  motorfour.setSpeed(speedLeft);
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
    //delay(1);
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
  degree = 90.0;//90 because ¯\_(ツ)_/¯

  if ((sumWest == 0 && sumEast == 0 && sumNorth == 0 && sumSouth == 0 ) || (sumWest != 0 && sumEast != 0 && sumNorth != 0 && sumSouth != 0) ) { //device off
    //reset degree values
    degree = -1;
    //clear array because of no direction
    aveDeg[2] = -1;
    aveDeg[1] = -1;
    aveDeg[0] = -1;
    stop();
    delay(500);
    return;
  }

  else if (sumNorth > sumSouth && sumNorth > 30) {//detected something more north than south
    if (sumNorth > 170 && sumEast < 30 && sumWest < 30) { //north much greater
      //just go north
      degree = 90;
      //clear array because of direction change
      aveDeg[2] = -1;
      aveDeg[1] = -1;
      aveDeg[0] = -1;
    }
    else if (sumEast > sumWest) {//check if east is greater than west

      /*else if (aveDeg[2] == 180) { //if just came from west
        aveDeg[2] = -1;//make the last value not count
        }*/

      degree = atan2(sumEast, sumNorth);//take the arctan of opposite over adjacent
      degree *= 180 / 3.1415; //convert to degrees
      degree = 90 - degree;//adjust to find the new angle
    }
    else { //east is not bigger, use west
      /*if (aveDeg[2] == 0) { //if just came from east
        aveDeg[2] = -1;//make the last value not count
        }*/
      degree = atan2(sumWest, sumNorth);//take arctan of opposite over adjacent on left side(if degree = 0 after,then north has value and east and west are 0)
      degree *= 180 / 3.1415; //convert (if 0, will still be 0)
      degree += 90;//adjust degree to new angle
    }
  }

  //detected something more south, check east vs west
  else if (sumEast > sumWest) {//east has greater value than west
    //if(sonar check)
    //clear array because of direction change
    aveDeg[2] = -1;
    aveDeg[1] = -1;
    aveDeg[0] = -1;
    degree = 0;
  } else { //west has the greater value than east
    //if(sonar Check)
    //clear array because of direction change
    aveDeg[2] = -1;
    aveDeg[1] = -1;
    aveDeg[0] = -1;
    degree = 180;
  }

degree = 180;
  if (degree != 90 || degree != 0 || degree != 180) {
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
      weightedAve += aveDeg[2] * 2;
      sum += 2;
    }

    //check to see if any values were valid and then average the values
    if (sum != 0) {
      weightedAve /= sum;
    } else {
      weightedAve = -1;
    }
    /*
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
    */

    changeSpeed(weightedAve);
    motorone.run(FORWARD);
    motortwo.run(FORWARD);
    motorthree.run(FORWARD);
    motorfour.run(FORWARD);
  } else {
    changeSpeedSpecial(degree);
  }
  
  //reset sums
  sumWest = 0;
  sumEast = 0;
  sumNorth = 0;
  sumSouth = 0;
}
void loop() {
  sonar();
  if (distance > 15) {
    onwards();
  } else {
    //turn the sonar sensor and check around the robot
    stop();//object too close to the robot
  }
}
