
#include <Wire.h>
#include <HX711.h>

double distance;
double mass;
double newtons;
double maxNewtons;
bool testing = true;
bool forceFlag = true;
int steps;

//================ Load Cell =====================
HX711 scale(A1, A0);
long offset = 0;
double Scale = 363.75;
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

  int flag = 0;
  while(flag == 0) { 
    flag = Serial.available();
  }  // Waits for input from Python script.
  
  digitalWrite(nemaSleep, LOW); // Wakes stepper up.
  
  init_scale();
 
  Serial.println("Distance, Newtons");
 // While loop for testing the sample.
  while (testing == true) {
   
    getForce();
    
    if (Serial.read() == 115) {
      //Serial.println("Done");
      forceFlag = false;
      newtons = 0;
    }
    
    checkState();
    moveMotor();
    countSteps();

    
  }
  //printData();
  digitalWrite(nemaSleep, HIGH);
}

 
void loop() {
}

//============ Functions ==================

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
    testing = false;
  }
}
