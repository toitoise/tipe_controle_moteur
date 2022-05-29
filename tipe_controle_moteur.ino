//#include <Stepper.h>
//#include <math.h>

#include <EEPROMex.h>
#include <Wire.h>
// Include the AccelStepper Library
#include <AccelStepper.h>
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
  digitalWrite(reset_n, HIGH);  // deactivate reset
  //digitalWrite(enable_n, LOW);  // enable
  myStepper.setAcceleration( DEFAULT_ACCEL );
  myStepper.setMaxSpeed    ( DEFAULT_MAX_SPEED );
  myStepper.setSpeed       ( DEFAULT_MAX_SPEED / 5 );

  //====================================================
  // INIT LCD
  //====================================================
  lcd.init();               // Attention réduit la frequence de l'I2C a 100KHz
  lcd.backlight();
  //----------------------------------------------------
  // Ecran 1 : Message d'accueil
  //----------------------------------------------------
  lcd.setCursor(2, 1);
  lcd.print("Hello, Half-God!");
  lcd.setCursor(4, 2);
  lcd.print("Motor Control");
  delay(3000);
  lcd.clear();
  //----------------------------------------------------
  // Ecran 2 : Selection NB Tours
  //----------------------------------------------------
  lcd.setCursor(0, 0);
  lcd.print("Adjust number of ");
  lcd.setCursor(0, 1);
  lcd.print("turns to go : ");
  lcd.setCursor(0, 3);
  lcd.print(">");
  lcd.setCursor(19, 3);
  lcd.print("<");
  unsigned long max_turns_from_selector = MAX_ROTATIONS;
  
  // Read/Write EEPROM
  unsigned long data_eeprom = EEPROM.readLong(EEPROM_ADDR_ROTATION_SAVED);
  if ( data_eeprom > HIGHER_BOUNDARY_ROTATIONS) {
    Serial.println("Reprogramm EEPROM");
    EEPROM.writeLong(EEPROM_ADDR_ROTATION_SAVED, MAX_ROTATIONS);
  }
  max_turns_from_selector = EEPROM.readLong(EEPROM_ADDR_ROTATION_SAVED);

  lcd.setCursor(9, 3);
  lcd.print(max_turns_from_selector);
  bool selected = false;
  while (! selected) {
    delay(10);
    button_state      = digitalRead(encoderSwitch);
    if (button_state == LOW) { // Click button
      selected = true;
      nb_of_turns_to_go = max_turns_from_selector;
      // Save to EEPROM
      EEPROM.writeLong(EEPROM_ADDR_ROTATION_SAVED, nb_of_turns_to_go);
      Serial.println("VALUES EEPROM");      Serial.println(EEPROM.readLong(EEPROM_ADDR_ROTATION_SAVED));
    } else {
      clk_state         = digitalRead(encoderClk);
      if ((clk_state_last == LOW) && (clk_state == HIGH)) {
        // Check turn-left or right
        max_turns_from_selector = strategy_for_evolution(max_turns_from_selector, digitalRead(encoderData));
        lcd.setCursor(9, 3);
        lcd.print("       ");
        lcd.setCursor(9, 3);
        lcd.print(max_turns_from_selector);
      }
      clk_state_last = clk_state;
    }
  }
  lcd.clear();

  //----------------------------------------------------
  // Ecran 3 : Selection vitesse
  //----------------------------------------------------
  lcd.setCursor(0, 0);
  lcd.print("== Select Periode ==");
  lcd.setCursor(0, POS_CURPER_LIG);
  lcd.print("CurPeriode(ms): ");
  lcd.setCursor(0, POS_NEWPER_LIG);
  lcd.print("NewPeriode(ms): ");
  lcd.setCursor(0, POS_NB_ROTATION_LIG);
  lcd.print("NB Rotations  :");lcd.print(nb_of_turns_to_go);


  // Active l'interruption sur la pin 2 venant du comparateur LM393
  attachInterrupt(digitalPinToInterrupt(comparator_pin), interrupt_tachymeter, RISING );

  //nb_of_turns_to_go = MAX_ROTATIONS;       // Assign default NB tours

  // Initialise la frequence de l'I2C apres l'init du LCD
  //Wire.setClock(400000);				// Augmente la frequence de l'I2C 400KHz au lieu de 100KHz
  Wire.setClock(850000);				// On test un peu plus vite
  stopMotor();
  Serial.println("Exit Setup");
}

//---------------------------------------------------------------------------------
// MAIN LOOP
//---------------------------------------------------------------------------------
void loop() {
  // Get Button state
  clk_state         = digitalRead(encoderClk);
  button_state      = digitalRead(encoderSwitch);
  button_millis_cur = millis();

  // Run Stepper to Target if not reach position
  if (myStepper.currentPosition() >= computed_steps_for_rotation) {
    stopMotor();
  } else {
    myStepper.run();
  }

  // check state and add debouncer (si nouveau click avant XX ms SKIP)
  if (button_state == LOW && (button_millis_cur - button_millis_save) > 300) {
    button_millis_save = millis();
    // Set NEW speed
    motor_speed_cur = motor_speed_new;	// updated on button'click
    // Display on LCD new selection
    lcd.setCursor(POS_CURPER_COL, POS_CURPER_LIG);
    lcd.print("     ");   // Clear
    lcd.setCursor(POS_CURPER_COL, POS_CURPER_LIG);
    lcd.print(speed_prog[motor_speed_new][5], DEC);
    // Program new Stepper speed & Update microSteps factor
    digitalWrite(reset_n,  HIGH);
    digitalWrite(enable_n, speed_prog[motor_speed_cur][4]);
    digitalWrite(M0,   	   speed_prog[motor_speed_cur][3]);
    digitalWrite(M1,   	   speed_prog[motor_speed_cur][2]);
    digitalWrite(M2, 	     speed_prog[motor_speed_cur][1]);
    myStepper.setAcceleration( speed_prog[motor_speed_cur][7] );
    myStepper.setMaxSpeed    ( speed_prog[motor_speed_cur][0] );
    myStepper.setCurrentPosition(0);    // reset position to 0
Serial.print("Turnstogo:");Serial.println(nb_of_turns_to_go);

    
    computed_steps_for_rotation = speed_prog[motor_speed_cur][6] * nb_of_turns_to_go;
    myStepper.moveTo(computed_steps_for_rotation);     // Define new Position Target
    previousmillis = millis();
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
      //Serial.println (motor_speed_new );	//debug
      //lcd.setCursor(NEW_POS_CURPER_COL, NEW_POS_CURPER_LIG);
      //lcd.print(motor_speed_new, DEC);
      lcd.setCursor(POS_NEWPER_COL, POS_NEWPER_LIG);
      lcd.print("     ");                             // Efface les précedents caracteres
      lcd.setCursor(POS_NEWPER_COL, POS_NEWPER_LIG);
      lcd.print(speed_prog[motor_speed_new][5], DEC );
    }
    // Save CLK current state to avoid new trigger
    clk_state_last = clk_state;
  }

  // If time updates then interrupt occured (debug)
  if (currentmillis != previousmillis) {
    if (currentmillis > previousmillis) {
      periode = (currentmillis - previousmillis) / RATIO_ENGRENAGE;
      previousmillis = currentmillis;
    } else {
      periode = ( previousmillis - currentmillis) / RATIO_ENGRENAGE;
      currentmillis = previousmillis ;
    }
    // Envoi de la periode sur le lien série car sur le LCD
    // ca provoque un petit hocquet sur la rotation
    Serial.print("Periode(ms)="); Serial.println(periode);
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
void stopMotor() {
  myStepper.stop();
  digitalWrite(enable_n, HIGH);  // Disable
  //digitalWrite(reset_n, LOW);  // reset
}


int compute_steps_for_one_rotation(int current_prog_speed ) {
  return speed_prog[current_prog_speed][6];
}


unsigned long strategy_for_evolution(unsigned long nb_turns, bool direction ) {
  /*
    Strategy:
    0    ->    10 = +1
    10   ->   100 = +10
    100  ->  1000 = +100
    1000 -> 10000 = +1000
    MAX 10000;
  */
  if (direction == LOW) { // turn right
    if  (nb_turns >= 0 && nb_turns < 10 && nb_turns < HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns += 1;
    } else if (nb_turns >= 10 && nb_turns < 100 && nb_turns < HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns += 10;
    } else if (nb_turns >= 100 && nb_turns < 1000 && nb_turns < HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns += 100;
    } else if (nb_turns >= 1000 && nb_turns < 10000 && nb_turns < HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns += 1000;
    } else if (nb_turns >= 10000 && nb_turns < 100000 && nb_turns < HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns += 10000;
    } else {
      nb_turns = HIGHER_BOUNDARY_ROTATIONS;
    }
  } else {
    if  (nb_turns > 0 && nb_turns <= 10 && nb_turns <= HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns -= 1;
    } else if (nb_turns > 10 && nb_turns <= 100 && nb_turns <= HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns -= 10;
    } else if (nb_turns > 100 && nb_turns <= 1000 && nb_turns <= HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns -= 100;
    } else if (nb_turns > 1000 && nb_turns <= 10000 && nb_turns <= HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns -= 1000;
    } else if (nb_turns > 10000 && nb_turns <= 100000 && nb_turns <= HIGHER_BOUNDARY_ROTATIONS) {
      nb_turns -= 10000;
    } else {
      nb_turns = 0;
    }
  }
  return nb_turns;
}



//---------------------------------------------------------------
//
//---------------------------------------------------------------
void debug1() {
  Serial.println(myStepper.currentPosition());
}
