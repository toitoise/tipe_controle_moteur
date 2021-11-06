// Include the AccelStepper Library
#include <AccelStepper.h>
#include <Wire.h>
#include <Stepper.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>

//-------------------------------------------------------
// Define drv8825 pin connections to Arduino UNO
//-------------------------------------------------------
#define dir         	3
#define step        	4
#define sleep_n       	5		// not connected tied to VCC
#define reset_n       	6
#define M2        		7
#define M1        		8
#define M0		       	9
#define enable_n       	10

//-------------------------------------------------------
// Rotary encoder pins 
//-------------------------------------------------------
#define encoderClk    	4  // CLK
#define encoderData   	5  // Data
#define encoderSwitch 	6  // Switch Button

//-------------------------------------------------------
// Comparator input pin with interrupt support
//-------------------------------------------------------
#define comparator 		2

//-------------------------------------------------------
// LCD 20x4 Declaration (i2c pins connexion)
//-------------------------------------------------------
#define POS_SPEED 11
LiquidCrystal_I2C lcd(0x27, 20, 4);
//LiquidCrystal_I2C lcd(0x3F,20,4);

// DRV8825 instance 
#define motorInterfaceType 1
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
#define MAX_ROTATION	200000

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
#define colonneTab 	5   		// 0 à 4
const int speed_prog[ligneTab][colonneTab] = {  
	      //MaxSpeed, M2, M1, M0 , Enable_n
/*00*/	{	100,	    0, 	0, 	0,   1} ,    // Stop
/*01*/	{	64 ,	    1, 	1, 	1,   0} ,   	// 10s		0.1 Hz    600rpm
/*02*/	{	85 ,	    1, 	1, 	1,   0} ,   	// 7.5s		0.13Hz    450rpm
/*03*/	{	64 ,	    1, 	0, 	0,   0} ,   	// 5.0s		0.2 Hz    300rpm
/*04*/	{	128,	    1, 	0, 	0,   0} ,    // 2.5s		0.4 Hz    150rpm
/*05*/	{	160,	    0, 	1, 	1,   0} ,   	// 1.0s		1.0 Hz    60 rpm
/*06*/	{	213,	    0, 	1, 	1,   0} ,   	// 0.75s	1.33Hz    45 rpm
/*07*/	{ 160,	    0, 	1, 	0,   0} ,   	// 0.50s	2.0 Hz    30 rpm
/*08*/	{	160,	    0, 	0, 	1,   0} ,   	// 0.25s	4.0 Hz    15 rpm
/*09*/	{	200,	    0, 	0, 	0,   0}     	// 0.10s	10	Hz     6 rpm
};

//---------------------------------------------------------------------------------
// SETUP
//---------------------------------------------------------------------------------
void setup() {
	Serial.begin(115200);
	Serial.println("Enter Setup");
	
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
	myStepper.setMaxSpeed(200);		//1sec primaire    / 1HZ  ==>secondaire 3HZ
	//myStepper.setMaxSpeed(400);	//0.5sec primaire  / 2HZ  ==>secondaire 6HZ
	//myStepper.setMaxSpeed(600);	//0.33sec primaire / 3HZ  ==>secondaire 9HZ
	
	myStepper.setAcceleration(100);
	//myStepper.setSpeed(100);
	myStepper.moveTo(200000);
	
	// Active l'interruption sur la pin 2
	attachInterrupt(digitalPinToInterrupt(comparator), interrupt_tachymeter, RISING );
	// initialisation de l'afficheur / Message d'accueil
	lcd.init(); 
	lcd.backlight();
	lcd.setCursor(2, 0);
	lcd.print("Hello, Half-God!");
	lcd.setCursor(4, 1);
	lcd.print("Motor Control");
	delay(5000);
	lcd.clear();
 
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
  
  // check state and add debouncer
	if (button_state == LOW && (button_millis_cur - button_millis_save)>500) {
   button_millis_save=millis();
		// Set NEW speed
		motor_speed_cur = motor_speed_new;	// update click
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Cur Speed: ");
		lcd.print(motor_speed_cur);
		
		// Program new Stepper speed
		myStepper.setMaxSpeed( speed_prog[motor_speed_cur][0] );
		// Update microSteps factor
		digitalWrite(M0,   	  speed_prog[motor_speed_cur][3]);
		digitalWrite(M1,   	  speed_prog[motor_speed_cur][2]);
		digitalWrite(M2, 	  speed_prog[motor_speed_cur][1]);
		digitalWrite(enable_n,speed_prog[motor_speed_cur][4]);

   microsteps_ratio=microsteps[ speed_prog[motor_speed_cur][3] | (speed_prog[motor_speed_cur][2]<<1) | (speed_prog[motor_speed_cur][1]<<2) ];

  double rpm_speed= (speed_prog[motor_speed_cur][0] * 3 )/microsteps_ratio;

	Serial.println("MaxSpeed, M2, M1, M0 , Enable_n");
	Serial.println(speed_prog[motor_speed_cur][0]);
  Serial.println(microsteps_ratio);
  Serial.println(rpm_speed);

		myStepper.moveTo(MAX_ROTATION);			// Define target
		myStepper.run();						// run to Target
		
	} else {
		
		// CLK Rising Edge
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
			//lcd.clear();
			lcd.setCursor(0, 1);
			lcd.print("New Speed:      ");
			lcd.setCursor(POS_SPEED, 1);
			lcd.print(motor_speed_new);
		}
		// Save CLK current state to avoid new trigger
		clk_state_last = clk_state;
	}
 	
	// If time update then interrupt occurs (debug)
	if (currentmillis!=previousmillis){
		periode=(currentmillis-previousmillis)/3; // rapport engrenage de 3
		Serial.print("Periode(ms):");
		Serial.println(periode);
		previousmillis = currentmillis;
	}
}

// Interrupt function to update TIME and compute periode
void interrupt_tachymeter(){
  // Update du chrono en milli secondes
  currentmillis = millis();
}
