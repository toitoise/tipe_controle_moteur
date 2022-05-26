//#include <Stepper.h>
// Include the AccelStepper Library
#include <AccelStepper.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "projet_definitions.h"

// Instancie le LCD 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Instancie le controle du DRV8825
AccelStepper myStepper(motorInterfaceType, step, dir);

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
  pinMode (encoderClk,    	INPUT_PULLUP);
  pinMode (encoderData,   	INPUT_PULLUP);
  pinMode (encoderSwitch, 	INPUT_PULLUP);
  pinMode (comparator_pin, 		  INPUT_PULLUP);

  // Outputs default value
  digitalWrite(reset_n, LOW);		// Assert DRV8825 reset_n
  digitalWrite(sleep_n, HIGH);
  digitalWrite(enable_n, HIGH);	// Disable at beginning
  digitalWrite(M0,   	LOW);
  digitalWrite(M1,   	LOW);
  digitalWrite(M2, 	  LOW);

  // De-Assert drv8825 reset_n
  digitalWrite(reset_n, HIGH);

  // set the maximum speed at begenning
  myStepper.setMaxSpeed(0);
  // Set initial acceleration factor
  myStepper.setAcceleration(DEFAULT_ACCEL);

  // initialisation de l'afficheur / Message d'accueil
  lcd.init();               // Attention réduit la frequence de l'I2C a 100KHz
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("Hello, Half-God!");
  lcd.setCursor(4, 1);
  lcd.print("Motor Control");
  delay(3000);
  lcd.clear();
  // Prepare LCD screen
  lcd.setCursor(0, 0);
  lcd.print("Select level 0 to 9");
  lcd.setCursor(0, 1);
  lcd.print(" Speed level : ");
  lcd.setCursor(0, 2);
  lcd.print(" New level   : ");
  lcd.setCursor(0, 3);
  lcd.print(" Periode(ms) : ");

  // Active l'interruption sur la pin 2 venant du comparateur LM393
  attachInterrupt(digitalPinToInterrupt(comparator_pin), interrupt_tachymeter, RISING );
  
  // Initialise la frequence de l'I2C apres l'init du LCD
  //Wire.setClock(400000);				// Augmente la frequence de l'I2C 400KHz au lieu de 100KHz
  Wire.setClock(850000);				// On test un peu plus vite
  Serial.println("Exit Setup");
}

//---------------------------------------------------------------------------------
// MAIN LOOP
//---------------------------------------------------------------------------------
void loop() {
  debug1();
 
  // Get Button state
  clk_state         = digitalRead(encoderClk);
  button_state      = digitalRead(encoderSwitch);
  button_millis_cur = millis();
  
  // Run Stepper to Target if not reach position
  if (myStepper.currentPosition()>=MAX_ROTATION) {
    stopMotor();
  } else {
    myStepper.run();
  }

  // check state and add debouncer (si nouveau click avant XX ms SKIP)
  if (button_state == LOW && (button_millis_cur - button_millis_save) > 300) {
    button_millis_save = millis();
    // Set NEW speed
    motor_speed_cur = motor_speed_new;	// updated on button'click

    lcd.setCursor(POS_SPEED_COL, POS_SPEED_LIG);
    lcd.print(motor_speed_cur, DEC );

    // Program new Stepper speed & Update microSteps factor
    myStepper.setMaxSpeed( speed_prog[motor_speed_cur][0] );
    digitalWrite(M0,   	  speed_prog[motor_speed_cur][3]);
    digitalWrite(M1,   	  speed_prog[motor_speed_cur][2]);
    digitalWrite(M2, 	    speed_prog[motor_speed_cur][1]);
    digitalWrite(enable_n, speed_prog[motor_speed_cur][4]);
    delay(10);
    myStepper.setCurrentPosition(0);
    myStepper.moveTo(MAX_ROTATION);     // Define target

  } else {

    // CLK Rising Edge du bouton rotatif
    // Sur le front montant (RISING) on regarde ce que vaut la Data
    // pour savoir si on tourne dans un sens ou dans l'autre.
    if ((clk_state_last == LOW) && (clk_state == HIGH)) {
      // Read rotary Data pin to know which direction increase/decrease new speed
      if (digitalRead(encoderData) == LOW) {
        if (motor_speed_new < (ligneTab - 1))
          motor_speed_new = motor_speed_new + 1;
      } else {
        if (motor_speed_new > 0)
          motor_speed_new = motor_speed_new - 1;
      }

      // Display new speed based on rotary button direction
      Serial.println (motor_speed_new );	//debug
      lcd.setCursor(NEW_POS_SPEED_COL, NEW_POS_SPEED_LIG);
      lcd.print(motor_speed_new, DEC);
      lcd.setCursor(NEW_POS_PER_COL, NEW_POS_PER_LIG);
      lcd.print("     ");                             // Efface les précedents caracteres
      lcd.setCursor(NEW_POS_PER_COL, NEW_POS_PER_LIG);
      lcd.print(speed_prog[motor_speed_new][5], DEC );
    }
    // Save CLK current state to avoid new trigger
    clk_state_last = clk_state;
  }

  // If time updates then interrupt occured (debug)
  if (currentmillis != previousmillis) {
    periode = (currentmillis - previousmillis) / RATIO_ENGRENAGE; 
    previousmillis = currentmillis;
    // Envoi de la periode sur le lien série car sur le LCD
    // ca provoque un petit hocquet sur la rotation
    Serial.println(periode);
  }
}

//---------------------------------------------------------------
// Interrupt function to update TIME and compute periode
//---------------------------------------------------------------
void interrupt_tachymeter() {
  // Update du chrono en milli secondes
  currentmillis = millis();
}

//---------------------------------------------------------------
// Stop Motor and disable to reduce consumption
//---------------------------------------------------------------
void stopMotor(){
    myStepper.stop();
    digitalWrite(enable_n, HIGH);  // Disable at beginning
}

//---------------------------------------------------------------
//
//---------------------------------------------------------------
void debug1() {
  Serial.println(myStepper.currentPosition());
}
