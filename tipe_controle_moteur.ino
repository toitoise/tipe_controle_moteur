#include <Wire.h>
// Include the AccelStepper Library
#include <AccelStepper.h>
#include "projet_definitions.h"

// Instancie le controle du DRV8825
AccelStepper myStepper(MOTOR_ITFC_TYPE, step, dir);

//---------------------------------------------------------------------------------
// SETUP
//---------------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);				// Definit le debit du bus Serie
  Serial.println("Enter Setup");
  // Outputs definitions
  pinMode(reset_n, 	  OUTPUT);
  pinMode(sleep_n,  	OUTPUT);
  pinMode(enable_n, 	OUTPUT);
  pinMode(M0,   		  OUTPUT);
  pinMode(M1,   		  OUTPUT);
  pinMode(M2, 		    OUTPUT);

  // Inputs definitions
  pinMode (comparator_pin, 		  INPUT_PULLUP);

  // Outputs default value
  digitalWrite(reset_n,   LOW);		// Un petit coup de RESET
  digitalWrite(sleep_n,   HIGH);
  digitalWrite(enable_n,  HIGH);	// Disable at beginning
  digitalWrite(M0,   	    LOW);
  digitalWrite(M1,   	    LOW);
  digitalWrite(M2, 	      LOW);
  digitalWrite(reset_n,   HIGH);  // On deactivate le RESET

 
  // Programme le moteur avec les nouvelles valeurs (Speed, Microsteps, Acceleration, nb tours)
  digitalWrite(reset_n,  HIGH);
  digitalWrite(enable_n,     speed_prog[MOTOR_SPEED][4]);
  digitalWrite(M0,           speed_prog[MOTOR_SPEED][3]);
  digitalWrite(M1,           speed_prog[MOTOR_SPEED][2]);
  digitalWrite(M2,           speed_prog[MOTOR_SPEED][1]);
  myStepper.setAcceleration( speed_prog[MOTOR_SPEED][7] );
  myStepper.setMaxSpeed    ( speed_prog[MOTOR_SPEED][0] );
  myStepper.setCurrentPosition(0);    // reset position to 0

  computed_steps_for_rotations = speed_prog[MOTOR_SPEED][6] * NOMBRE_ROTATIONS;
  myStepper.moveTo(computed_steps_for_rotations);      // Programme la nouvelle position (nb tours)
   
  // Active l'interruption sur la pin 2 venant du comparateur LM393
  attachInterrupt(digitalPinToInterrupt(comparator_pin), interrupt_tachymeter, RISING );
  
  Wire.setClock(850000);                // On boost un peu l'I2C en fréquence 850KHz
  Serial.println("Exit Setup");
}

//---------------------------------------------------------------------------------
// Boucle Principale
//---------------------------------------------------------------------------------
void loop() {

  // Run Stepper to Target if not reach position
  if (myStepper.currentPosition() >= computed_steps_for_rotations) {
    stopMotor();        // Si pas de nouvelle cible ou cible deja atteinte alors force stop 
  } else {
    myStepper.run();    // Appel regulier du run() obligatoire
  }

  
  // Si Interruption alors on passe ici OU forcage en mettant a jour previousmillis
  if (currentmillis != previousmillis) {
      periode = ( previousmillis - currentmillis) / RATIO_ENGRENAGE;
      currentmillis = previousmillis ;
    // Envoi de la periode sur le lien série
    Serial.print("Periode(ms)="); Serial.println(periode);
  }
}

//---------------------------------------------------------------
// Interrupt (on met juste le temps a jour)
//---------------------------------------------------------------
void interrupt_tachymeter() {
  // Update du chrono en milli secondes
  currentmillis = millis();
}

//---------------------------------------------------------------
// Stop Motor et disable pour reduire la consommation
//---------------------------------------------------------------
void stopMotor() {
  myStepper.stop();
  digitalWrite(enable_n, HIGH);  // Disable Driver (diminue la consommation)
}
