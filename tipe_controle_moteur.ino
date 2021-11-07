// Include the AccelStepper Library
#include <AccelStepper.h>
#include <Wire.h>
#include <Stepper.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

//-------------------------------------------------------
// Define drv8825 pin connections to Arduino UNO
//-------------------------------------------------------
#define dir			3
#define step		4
#define sleep_n		5		// not connected tied to VCC
#define reset_n		6
#define M2			7
#define M1			8
#define M0			9
#define enable_n	10

//-------------------------------------------------------
// Rotary encoder pins 
//-------------------------------------------------------
#define encoderClk		11		// CLK
#define encoderData		12		// Data
#define encoderSwitch	13		// Switch Button

// Various defines
#define motorInterfaceType	1		// Motor interface type (DRV8825 dir+steps)
#define MAX_ROTATION		400000	// Max steps for motor

//-------------------------------------------------------
// Comparator input pin with interrupt support
//-------------------------------------------------------
#define comparator 		2

//-------------------------------------------------------
// LCD 20x4 Declaration (i2c pins connexion)
//-------------------------------------------------------
#define POS_SPEED 15
#define POS_PER   15
LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystal_I2C lcd(0x3F,20,4);

// DRV8825 instance 
AccelStepper myStepper(motorInterfaceType, step, dir);

// computation for rotation periode with HALL sensor
volatile unsigned long periode 			= 0;
volatile unsigned long currentmillis 	= 0;
volatile unsigned long previousmillis 	= 0;

// Variables traitement bouton rotatif et click
int clk_state_last      = LOW;            // Idle
int clk_state           = LOW;            // Idle
int button_state        = HIGH;           // Not pressed
unsigned long button_millis_save = 0;
unsigned long button_millis_cur = 0;

// Variables traitement vitesse
int motor_speed_cur     = 0;     // current speed
int motor_speed_new     = 0;     // new speed

//-------------------------------------------------
// M0		M1		M2		Microstep resolution
// Low		Low		Low		Full step
// High		Low		Low		1/2 step
// Low		High	Low		1/4 step
// High		High	Low		1/8 step
// Low		Low		High	1/16 step
// High		Low		High	1/32 step
// Low		High	High	1/32 step
// High		High	High	1/32 step
//-------------------------------------------------
const int microsteps[] = {1,2,4,8,16,32,32,32};
#define MICROSTEPS_SIZE sizeof(microsteps)/sizeof(int)
unsigned int microsteps_ratio=0;

#define ligneTab 	10   		// 0 à 9
#define colonneTab 	6   		// 0 à 5
const int speed_prog[ligneTab][colonneTab] = {  
	      //MaxSpeed, 	M2, M1, M0 , Enable_n, target(ms) 
/*00*/	{	0,			0, 	0, 	0,   1,        0    } ,   // Stop
/*01*/	{	213,		1, 	0, 	1,   0,        10000} ,   // 10s	0.1 Hz    6  rpm     ~10033ms
/*02*/	{	285,		1, 	0, 	1,   0,        7500 } ,   // 7.5s	0.13Hz    15 rpm     ~ 7503ms
/*03*/	{	426 ,		1, 	0, 	1,   0,        5000 } ,   // 5.0s	0.2 Hz    30 rpm     ~ 5016ms
/*04*/	{	214,		0, 	1, 	1,   0,        2500 } ,   // 2.5s	0.4 Hz    45 rpm     ~ 2497ms
/*05*/	{	267,		0, 	1, 	0,   0,        1000 } ,   // 1.0s	1.0 Hz    60 rpm     ~ 1001ms
/*06*/	{	357,		0, 	1,  0,   0,        750  } ,   // 0.75s	1.33Hz    150 rpm    ~  751ms
/*07*/	{	267,		0,  0,  1,   0,        500  } ,   // 0.50s	2.0 Hz    120 rpm    ~  500ms
/*08*/	{	534,		0,  0,  1,   0,        250  } ,   // 0.25s  4.0 Hz    240 rpm    ~  250ms
/*09*/	{	535,		0,  0,  0,   0,        125  }     // 0.125s 8.0 Hz    480 rpm    ~  125ms

/*  Autres parametres de réglages possibles
{ 666,      0,  0,  0,   0,        100  }     // 0.10s  10  Hz    600 rpm    ~  101ms
 */
};

//---------------------------------------------------------------------------------
// SETUP
//---------------------------------------------------------------------------------
void setup() {
	Serial.begin(115200);				// Definit le debit du bus rs232/Serie
	Serial.println("Enter Setup");
	Wire.setClock(400000);				// Augmente la frequence de l'I2C 400KHz au lieu de 100KHz
	// Outputs definitions
	pinMode(reset_n, 	OUTPUT);
	pinMode(sleep_n, 	OUTPUT);
	pinMode(enable_n, 	OUTPUT);
	pinMode(M0,   		OUTPUT);
	pinMode(M1,   		OUTPUT);
	pinMode(M2, 		OUTPUT);
	
	// Inputs definitions
	pinMode (encoderClk,    	INPUT);
	pinMode (encoderData,   	INPUT);
	pinMode (encoderSwitch, 	INPUT_PULLUP);
	pinMode (comparator, 		INPUT_PULLUP);
	
	// Outputs default value
	digitalWrite(reset_n, LOW);		// Assert DRV8825 reset_n
	digitalWrite(sleep_n, HIGH);	
	digitalWrite(enable_n,HIGH);	// Disable
	digitalWrite(M0,   	LOW);
	digitalWrite(M1,   	LOW);
	digitalWrite(M2, 	LOW);

	// De-Assert drv8825 reset_n
	digitalWrite(reset_n, HIGH);
	
	// set the maximum speed, acceleration factor,
	// initial speed and the target position
	//myStepper.setMaxSpeed(200);	
	myStepper.setAcceleration(100);

	// Active l'interruption sur la pin 2 venant du comparateur LM393
	attachInterrupt(digitalPinToInterrupt(comparator), interrupt_tachymeter, RISING );

	// initialisation de l'afficheur / Message d'accueil
	lcd.init(); 
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
	// Run Stepper to Target
	myStepper.run();                    

	// check state and add debouncer
	if (button_state == LOW && (button_millis_cur - button_millis_save)>500) {
	button_millis_save=millis();
	// Set NEW speed
	motor_speed_cur = motor_speed_new;	// updated on button'click

    lcd.setCursor(POS_SPEED, 1);
	lcd.print(motor_speed_cur,DEC );
    lcd.setCursor(POS_PER, 3);
    lcd.print(speed_prog[motor_speed_cur][5],DEC );
    // Program new Stepper speed
	myStepper.setMaxSpeed( speed_prog[motor_speed_cur][0] );
    myStepper.moveTo(MAX_ROTATION);     // Define target

    // Update microSteps factor
	digitalWrite(M0,   	  speed_prog[motor_speed_cur][3]);
	digitalWrite(M1,   	  speed_prog[motor_speed_cur][2]);
	digitalWrite(M2, 	  speed_prog[motor_speed_cur][1]);
	digitalWrite(enable_n,speed_prog[motor_speed_cur][4]);

//	Serial.println("MaxSpeed, M2, M1, M0 , Enable_n");
//	Serial.println(speed_prog[motor_speed_cur][0]);
//  Serial.println(microsteps_ratio);
//  Serial.println(rpm_speed);
//  Serial.println(myStepper.maxSpeed( ));

	} else {
		
	// CLK Rising Edge du bouton rotatif
    // Sur le front montant (RISING) on regarde ce que vaut la Data
    // pour savoir si on tourne dans un sens ou dans l'autre.
		if ((clk_state_last == LOW) && (clk_state == HIGH)) {
			// Read rotary Data pin to know which direction increase/decrease new speed
			if (digitalRead(encoderData) == LOW) {
				if (motor_speed_new < (ligneTab-1))
					motor_speed_new = motor_speed_new + 1;
			} else {
				if (motor_speed_new > 0)
					motor_speed_new = motor_speed_new - 1;
			}
	
			// Display new speed based on rotary button direction
			Serial.println (motor_speed_new );	//debug
			lcd.setCursor(POS_SPEED, 2);
			lcd.print(motor_speed_new);
			lcd.setCursor(POS_PER, 3);
			lcd.setCursor(POS_PER, 3);
			lcd.print("     ");
			lcd.setCursor(POS_PER, 3);
			lcd.print(speed_prog[motor_speed_new][5],DEC );
		}	
		// Save CLK current state to avoid new trigger
		clk_state_last = clk_state;
	}
 
  
	// If time update then interrupt occurs (debug)
	if (currentmillis!=previousmillis){
		periode=(currentmillis-previousmillis)/3; // rapport engrenage de 3
		previousmillis = currentmillis;
    	// Envoi de la periode sur le lien série car sur le LCD 
		// ca provoque un petit hocquet sur la rotation
		//lcd.setCursor(POS_PER, 3);
		//lcd.print(periode);
		Serial.println(periode);
	}
}

//---------------------------------------------------------------
// Interrupt function to update TIME and compute periode
//---------------------------------------------------------------
void interrupt_tachymeter(){
  // Update du chrono en milli secondes
  currentmillis = millis();
}
