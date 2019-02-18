
#include <Wire.h>
#include <HX711.h>


double distance;
double mass;
double newtons;
double maxNewtons;
bool tensileTesting = true;
bool forceFlag = true;
bool compressionTesting = true;
int steps;
double distanceTravel;

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
    distanceTravel = Serial.parseFloat();
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
  if(((forceFlag == false) && (newtons < 0.1)) || (Serial.read() == 115)) {
    tensileTesting = false;
  } 
}

void tensileTest() {
  while (tensileTesting == true) {
    getForce();    
    checkState();
    moveMotor();
    countSteps();   
  }
}

//======== Compression Testing Functions ===============

void compressionTest() {
  digitalWrite(directionPin, HIGH);
  while(compressionTesting == true) {
    getForceC();
    checkStateC();
    moveMotorC();
    countSteps();
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

void checkStateC() {
  distance = steps / 25.0;
  if (Serial.read() == 115) {
    compressionTesting = false;
  } else if(distance >= distanceTravel) {
    compressionTesting = false;
  }
}
void moveMotorC() {
  digitalWrite(stepPin, HIGH);
  digitalWrite(stepPin, LOW);   
}
