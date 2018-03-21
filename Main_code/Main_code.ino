
#include <Wire.h>
#include <NewPing.h>
#include <AFMotor.h>
#include <Servo.h>

AF_DCMotor motorone(1, MOTOR12_64KHZ);
AF_DCMotor motortwo(2, MOTOR12_64KHZ);
AF_DCMotor motorthree(3, MOTOR12_64KHZ);
AF_DCMotor motorfour(4, MOTOR12_64KHZ);
Servo servo;

//GLOBAL VARIABLES//
int minValue = 200;
long distance = 0;
double aveDeg[3] = { -1, -1, -1};
const int trigPin = A0;
const int echoPin = A1;

void setup() {

  motorone.setSpeed(0);
  motortwo.setSpeed(0);
  motorthree.setSpeed(0);
  motorfour.setSpeed(0);

  Serial.begin(9600);

  servo.attach(10);
  servo.write(90);
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

void changeSpeedSpecial(int degree) {
  if (degree == 0.00) {

    motorone.setSpeed(85);
    motortwo.setSpeed(85);
    motorthree.setSpeed(85);
    motorfour.setSpeed(85);
    //turn servo east and poll
    servo.write(180);
    delay(3);
    sonar();

    if (distance < 15) {
      stop();
      delay(50);
      return;
    }

    motorone.run(RELEASE);
    motortwo.run(RELEASE);
    motorthree.run(FORWARD);
    motorfour.run(FORWARD);
    motorone.run(BACKWARD);
    motortwo.run(BACKWARD);
    Serial.println("Left");

  } else if (degree == 180.00) {
    
    motorone.setSpeed(85);
    motortwo.setSpeed(85);
    motorthree.setSpeed(85);
    motorfour.setSpeed(85);
    //turn servo west and poll
    servo.write(0);
    delay(3);
    sonar();
    
    if (distance < 15) {
      stop();
      delay(50);
      return;
    }
    
    motorthree.run(RELEASE);
    motorfour.run(RELEASE);
    motorone.run(FORWARD);
    motortwo.run(FORWARD);
    motorthree.run(BACKWARD);
    motorfour.run(BACKWARD);
    
  } else {
    motorone.setSpeed(200);
    motortwo.setSpeed(200);
    motorthree.setSpeed(200 + 15);
    motorfour.setSpeed(200 + 15);
    //turn servo forward and poll
    servo.write(90);
    delay(1);
    sonar();
    
    if (distance < 15) {
      stop();
      delay(50);
      return;
    }
    
    motorone.run(FORWARD);
    motortwo.run(FORWARD);
    motorthree.run(FORWARD);
    motorfour.run(FORWARD);

  }
}

void changeSpeed(int degree) {
  int speedLeft = 200;
  int speedRight = 200;
  
  if (degree == -1.00) {
    stop();
    delay(50);
    return;
  } else if (degree > 90) { //if the person is to the left
    
    //turn servo north west and poll
    servo.write(90);
    delay(1);
    sonar();
    
    if (distance < 15) {
      stop();
      delay(50);
      return;
    }
    
    speedLeft -= floor(13 * (degree - 90)); //adjust motors on the left
    
    if (speedLeft < 0) {
      speedLeft = 0;
    }
    
  } else {//if the person is to the right or in front
    //turn servo north east and poll
    servo.write(90);
    delay(1);
    sonar();
    if (distance < 15) {
      stop();
      delay(50);
      return;
    }
    speedRight -= floor(13 * (90 - degree)); //adjust motors on the left
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
    west[j] = analogRead(4);
    south[j] = analogRead(5);
    east[j] = analogRead(2);
    north[j] = analogRead(3);
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

  if ((sumWest <= 5 && sumEast <= 5 && sumNorth <= 5 && sumSouth <= 5 ) || (sumSouth != 0 && sumWest != 0 && sumEast != 0 && sumNorth != 0) ) { //device off
    //reset degree values
    degree = -1;
    //clear array because of no direction
    aveDeg[2] = -1;
    aveDeg[1] = -1;
    aveDeg[0] = -1;
    stop();
    delay(50);
    return;
  } else if (sumNorth > sumSouth && sumNorth > 20) {//detected something more north than south
    if (sumNorth > 100 && sumEast <= 1 && sumWest <= 1) { //north much greater
      //just go north
      degree = 90;
      //clear array because of direction change
      aveDeg[2] = -1;
      aveDeg[1] = -1;
      aveDeg[0] = -1;
    } else if (sumEast > sumWest) {//check if east is greater than west

      if (aveDeg[2] == 180) { //if just came from west
        aveDeg[2] = -1;//make the last value not count
      }

      degree = atan2(sumEast, sumNorth);//take the arctan of opposite over adjacent
      degree *= 180 / 3.1415; //convert to degrees
      degree = 90 - degree;//adjust to find the new angle
      
    } else { //east is not bigger, use west
      
      if (aveDeg[2] == 0) { //if just came from east
        aveDeg[2] = -1;//make the last value not count
      }
      
      degree = atan2(sumWest, sumNorth);//take arctan of opposite over adjacent on left side(if degree = 0 after,then north has value and east and west are 0)
      degree *= 180 / 3.1415; //convert (if 0, will still be 0)
      degree += 90;//adjust degree to new angle
    }
  }
  //detected something more south, check east vs west
  else if (sumEast > sumWest) {//east has greater value than west
    degree = 0;
  } else { //west has the greater value than east
    //clear array because of direction change
    if (sumWest > 20) {
      degree = 180;
    } else if (aveDeg[2] == 0) {
      degree = 0;
    } else {
      degree = 180;
    }
  }
  //if (degree != 90.00 && degree != 0.00 && degree != 180.00) {
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

  //weight the last value by x3
  if (aveDeg[2] != -1) {
    weightedAve += aveDeg[2] * 2;
    sum += 2;
  }

  //check to see if any values were valid and then average the values
  if (sum != 0) {
    weightedAve /= sum;
  }
  if (weightedAve != 90.00 && weightedAve != 0.00 && weightedAve != 180.00) {
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

    changeSpeed(weightedAve);
    motorone.run(FORWARD);
    motortwo.run(FORWARD);
    motorthree.run(FORWARD);
    motorfour.run(FORWARD);
  } else {
    Serial.print("North : ");
    Serial.println(sumNorth);
    Serial.print("West : ");
    Serial.println(sumWest);
    Serial.print("East : ");
    Serial.println(sumEast);
    Serial.print("South : ");
    Serial.println(sumSouth);
    Serial.print("Degree(weighted average) : ");
    Serial.println(degree);
    Serial.println("");
    changeSpeedSpecial(weightedAve);
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
    Serial.println(distance);
    onwards();
  } else {
    stop();
  }
}
