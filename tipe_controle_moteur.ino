// Include the AccelStepper Library
#include <AccelStepper.h>

//-------------------------------------------------------
// Define drv8825 pin connections to Arduino UNO
//-------------------------------------------------------
#define dir         	3
#define step        	4
#define sleep_n       	5
#define reset_n       	6
#define m2        		7
#define m1        		8
#define m0		       	9
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
LiquidCrystal_I2C lcd(0x27, 20, 4); // 2 lines/16 chars

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

// Variables traitement vitesse
int motor_speed_cur     = 0;     // current speed
int motor_speed_new     = 0;     // new speed
#define MAX_ROTATION	200000

#define ligneTab 	10   		// 0 à 9
#define colonneTab 	4   		// 0 à 3
int speed_prog[ligneTab][colonneTab] = {  
	//maxSpeed, m2, m1, m0
	{	100,	1, 	1, 	1} ,    // Stop
	{	100,	1, 	1, 	1} ,   	// 10s		0.1 Hz
	{	100,	0, 	1, 	0} ,   	// 7.5s		0.13Hz
	{	100,	0, 	1, 	0} ,   	// 5.0s		0.2 Hz
	{	100,	0, 	1, 	0} ,    // 2.5s		0.4 Hz
	{	100,	0, 	1, 	0} ,   	// 1.0s		1.0 Hz
	{	100,	0, 	1, 	0} ,   	// 0.75s	1.33Hz
	{	100,	0, 	1, 	0} ,   	// 0.50s	2.0 Hz
	{	100,	0, 	0, 	0} ,   	// 0.25s	4.0 Hz
	{	100,	0, 	0, 	0}    	// 0.10s	10	Hz
};


//---------------------------------------------------------------------------------
// SETUP
//---------------------------------------------------------------------------------
void setup() {
	Serial.begin(115200);
	Serial.println("Enter Setup");
	
	// Outputs definitions
	pinMode(m0,   		OUTPUT);
	pinMode(m1,   		OUTPUT);
	pinMode(m2, 		OUTPUT);
	pinMode(reset_n, 	OUTPUT);
	pinMode(sleep_n, 	OUTPUT);
	pinMode(enable_n, 	OUTPUT);
	
	// Inputs definitions
	pinMode (encoderClk,    	INPUT);
	pinMode (encoderData,   	INPUT);
	pinMode (encoderSwitch, 	INPUT_PULLUP);
	pinMode (comparator, 		INPUT_PULLUP);
	
	// Outputs default value
	digitalWrite(m0,   	LOW);
	digitalWrite(m1,   	LOW);
	digitalWrite(m2, 		LOW);
	digitalWrite(reset_n, LOW);
	digitalWrite(sleep_n, LOW);
	digitalWrite(enable_n,LOW);
	
	// set the maximum speed, acceleration factor,
	// initial speed and the target position
	myStepper.setMaxSpeed(200);		//1sec primaire    / 1HZ  ==>secondaire 3HZ
	//myStepper.setMaxSpeed(400);		//0.5sec primaire  / 2HZ  ==>secondaire 6HZ
	//myStepper.setMaxSpeed(600);	//0.33sec primaire / 3HZ  ==>secondaire 9HZ
	
	myStepper.setAcceleration(100);
	//myStepper.setSpeed(100);
	myStepper.moveTo(200000);
	Serial.println("Exit Setup");
	
	// Active l'interruption sur la pin 2
	attachInterrupt(digitalPinToInterrupt(comparator), interrupt_tachymeter, RISING );
}

//---------------------------------------------------------------------------------
// MAIN LOOP
//---------------------------------------------------------------------------------
void loop() {
	clk_state    = digitalRead(encoderClk);
	button_state = digitalRead(encoderSwitch);

	if (button_state == LOW) {
		// Set NEW speed
		motor_speed_cur = motor_speed_new;	// update click
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("Cur Speed: ");
		lcd.print(motor_speed_cur);
		
		// Program new Stepper speed
		//myStepper.setSpeed(motor_speed_cur/10);
		//myStepper.step(MAX_REVOLUTIONS);
		 myStepper.setMaxSpeed( speed_prog[motor_speed_cur][0] );
		 myStepper.moveTo(MAX_ROTATION);
		 myStepper.run();
	} else {
		// CLK Rising Edge
		if ((clk_state_last == LOW) && (clk_state == HIGH)) {
			// Read rotary Data pin to know which direction
			if (digitalRead(encoderData) == LOW) {
				if (motor_speed_new < ligneTab)
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

 	
	// Change direction once the motor reaches target position
	//Serial.println(myStepper.distanceToGo());
	if (myStepper.distanceToGo() == 0) {
		myStepper.moveTo(myStepper.currentPosition());
	}
	
	// Move the motor one step
	//myStepper.runSpeed();
	myStepper.run();
	
	
	
	// If time update then interrupt occurs (debug)
	if (currentmillis!=previousmillis){
		periode=(currentmillis-previousmillis)/3; // rapport engrenage de 3
		Serial.print("Periode(ms):");
		Serial.println(periode);
		previousmillis = currentmillis;
	}
}


void interrupt_tachymeter(){
  // Update du chrono en milli secondes
  currentmillis = millis();
}
