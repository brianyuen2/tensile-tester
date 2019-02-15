
#include <Wire.h>
#include <HX711.h>
#include <Stepper.h>

double distance;
double mass;
double newtons;
double maxNewtons;
bool tensileTesting = true;
bool forceFlag = true;
bool compressionTesting = true;
int steps;

//================ Load Cell =====================
HX711 scale(A1, A0);
long offset = 0;
double Scale = 375.14;
long raw; 

//============== Stepper Motor ===============

#define directionPin 8
#define stepPin 9
#define nemaSleep 7

void setup() {
  // Init pins and serial
  Serial.begin(9600);
  scale.set_gain(128);
  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(nemaSleep, OUTPUT);
  
  digitalWrite(nemaSleep, HIGH); // Puts stepper to sleep so you can adjust height while attaching sample.

  int choice = -1;
  while(choice == -1){
    choice = Serial.read();
  }  // Gets choice from Python script.
  
  digitalWrite(nemaSleep, LOW); // Wakes stepper up.
  
  init_scale();
 
  Serial.println("Distance, Newtons");
  if(choice == 49) {
    tensileTest();
  } else if(choice == 50) {
    double distanceTravel = Serial.parseFloat();
    Serial.println(distanceTravel);
    compressionTest();
  }
  //printData();
  digitalWrite(nemaSleep, HIGH);
}

 
void loop() {
}

//============ Tensile Testing Functions ==================

void init_scale(){
  for(int i = 0; i < 5; i++) {
    raw = scale.read_average(20);
    Serial.println(raw);
  }
  raw = scale.read_average(50);
  offset = raw;
}

void getForce() {
   raw = scale.read_average(1);
   mass = (raw - offset)/Scale;
   newtons = mass*9.81/1000;
   if(newtons > maxNewtons) {
    maxNewtons = newtons;
   }
   Serial.print(steps / 25.0); Serial.print(", ");
   Serial.println(newtons); 
}

void moveMotor() {
  digitalWrite(stepPin, HIGH);
  digitalWrite(stepPin, LOW); 
  
}

void printData() {
  distance = steps / 25.0; 
  Serial.print("Steps "); Serial.println(steps);
  Serial.print("Distance "); Serial.println(distance);
  Serial.print("Max Newtons "); Serial.println(maxNewtons);
} 

void countSteps() {
  if(newtons > 0.02) { 
    steps += 1;
  }
}

void checkState() {
if(newtons > 0.1) {
    forceFlag = false;
  }
  if((forceFlag == false) && (newtons < 0.1)) {
    tensileTesting = false;
  }
}

void tensileTest() {
  while (tensileTesting == true) {
    getForce();    
    checkStop();
    checkState();
    moveMotor();
    countSteps();   
  }
}
void checkStop() {
  if (Serial.read() == 115) {
      forceFlag = false;
      newtons = 0;
  } 
}
//======== Compression Testing Functions ===============

void compressionTest() {
  while(compressionTesting == true) {
    getForceC();
    checkStopC();
    moveMotorC();
    if(newtons > 1) {
      compressionTesting == false;
    }
  }
}

void getForceC() {
   raw = scale.read_average(1);
   mass = (raw - offset)/Scale;
   newtons = -mass*9.81/1000;
   if(newtons > maxNewtons) {
    maxNewtons = newtons;
   }
   Serial.print(steps / 25.0); Serial.print(", ");
   Serial.println(newtons); 
}

void checkStopC() {
  if (Serial.read() == 115) {
      compressionTesting = false;
  } 
}
void moveMotorC() {
  digitalWrite(directionPin, HIGH);
  digitalWrite(stepPin, HIGH);
  digitalWrite(stepPin, LOW);   
}
