
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
  if (degree == 0.00) {
    motorone.setSpeed(50);
    motortwo.setSpeed(50);
    motorthree.setSpeed(50);
    motorfour.setSpeed(50);
    motorone.run(BACKWARD);
    motortwo.run(BACKWARD);
    motorthree.run(FORWARD);
    motorfour.run(FORWARD);
    Serial.println("Left");
  }
  else if (degree == 180.00) {
    motorone.setSpeed(50);
    motortwo.setSpeed(50);
    motorthree.setSpeed(50);
    motorfour.setSpeed(50);
    motorone.run(FORWARD);
    motortwo.run(FORWARD);
    motorthree.run(BACKWARD);
    motorfour.run(BACKWARD);

    Serial.println("Right");
  }
  else {
    motorone.setSpeed(200);
    motortwo.setSpeed(200);
    motorthree.setSpeed(200);
    motorfour.setSpeed(200);
    motorone.run(FORWARD);
    motortwo.run(FORWARD);
    motorthree.run(FORWARD);
    motorfour.run(FORWARD);
    Serial.println("FORWARD");
    Serial.println("Forward");
  }
}

void changeSpeed(int degree) {
  int speedLeft = 200;
  int speedRight = 200;
  if (degree == -1.00) {
    stop();
    return;
  }
  if (degree > 90) { //if the person is to the left

    speedLeft -= floor(2 * (degree - 90)); //adjust motors on the left
    if (speedLeft < 0) {
      speedLeft = 0;
    }
  } else {//if the person is to the right or in front
    speedRight -= floor(2 * (90 - degree)); //adjust motors on the left
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
    west[j] = analogRead(3);
    south[j] = analogRead(2);
    east[j] = analogRead(5);
    north[j] = analogRead(4);
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
      Serial.println("stop");
    degree = -1;
    //clear array because of no direction
    aveDeg[2] = -1;
    aveDeg[1] = -1;
    aveDeg[0] = -1;
    stop();
    delay(250);
    return;
  }

  else if (sumNorth > sumSouth && sumNorth > 20) {//detected something more north than south
    if (sumNorth > 170 && sumEast < 20 && sumWest < 20) { //north much greater
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
    if(sumWest > 20){
    aveDeg[2] = -1;
    aveDeg[1] = -1;
    aveDeg[0] = -1;
    degree = 180;
    }
    else if(aveDeg[2] == 0){
    aveDeg[2] = -1;
    aveDeg[1] = -1;
    aveDeg[0] = -1;
    degree = 0;
    }
    else{
    aveDeg[2] = -1;
    aveDeg[1] = -1;
    aveDeg[0] = -1;
    degree = 180;
    }
  }
  Serial.println(degree);
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
  /*} else {
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
    changeSpeedSpecial(degree);
  }*/
  
  //reset sums
  sumWest = 0;
  sumEast = 0;
  sumNorth = 0;
  sumSouth = 0;
}
void loop() {
  sonar();
  Serial.println(distance);
  if (true){//distance > 15) {
    onwards();
  } else {
    //turn the sonar sensor and check around the robot
    stop();//object too close to the robot
  }
}
