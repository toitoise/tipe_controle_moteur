//=====================================================================
//
//
//
//=====================================================================



// Include the AccelStepper Library
#include <AccelStepper.h>

// Define drv8825 pin connections
const int dirPin        = 3;
const int stepPin       = 4;

// computation for periode
const int hallSensorPin = 2;
volatile unsigned long periode = 0;
volatile unsigned long currentmillis = 0;
volatile unsigned long previousmillis = 0;

// Define motor interface type
#define motorInterfaceType 1

// Creates an instance
AccelStepper myStepper(motorInterfaceType, stepPin, dirPin);

void setup() {
  Serial.begin(115200);
  Serial.println("Enter Setup");
  // set the maximum speed, acceleration factor,
  // initial speed and the target position
  myStepper.setMaxSpeed(200);//1sec primaire    / 1HZ  ==>secondaire 3HZ
  myStepper.setMaxSpeed(400);//0.5sec primaire  / 2HZ  ==>secondaire 6HZ
  //myStepper.setMaxSpeed(600);//0.33sec primaire / 3HZ  ==>secondaire 9HZ

  myStepper.setAcceleration(100);
  //myStepper.setSpeed(100);
  myStepper.moveTo(200000);
  Serial.println("Exit Setup");

    /* Met la broche en entrée */  
  pinMode(hallSensorPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(hallSensorPin), interrupt_tachy, RISING );
}

void loop() {
  // Change direction once the motor reaches target position
  //Serial.println(myStepper.distanceToGo());
  if (myStepper.distanceToGo() == 0) 
    myStepper.moveTo(myStepper.currentPosition());

  // Move the motor one step
  //myStepper.runSpeed();
  myStepper.run();
  //delay(1000);

 

  // If time update then interrupt occurs
  if (currentmillis!=previousmillis){
    periode=(currentmillis-previousmillis)/3;
    Serial.print("Periode(ms):");
    Serial.println(periode);
    previousmillis = currentmillis;
  }

}


void interrupt_tachy(){
  currentmillis = millis();
}
