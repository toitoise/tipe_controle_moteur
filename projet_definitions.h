#define RATIO_ENGRENAGE 3

//-------------------------------------------------------
// Define drv8825 pin connections to Arduino UNO
//-------------------------------------------------------
#define dir       3
#define step      4
#define sleep_n   5   // not connected tied to VCC
#define reset_n   6
#define M2        7
#define M1        8
#define M0        9
#define enable_n  10

//-------------------------------------------------------
// Rotary encoder pins
//-------------------------------------------------------
#define encoderClk    11    // CLK
#define encoderData   12    // Data
#define encoderSwitch 13    // Switch Button

// Various defines
#define motorInterfaceType  1       // Motor interface type (DRV8825 dir+steps)
#define MAX_ROTATION        400000  // Max steps for motor
#define MAX_ROTATION        1000  // Max steps for motor
#define DEFAULT_MAX_SPEED   200     // At beginning
#define DEFAULT_ACCEL       1000      // Steps to max speed

//-------------------------------------------------------
// Comparator input pin with interrupt support
//-------------------------------------------------------
#define comparator_pin    2

//-------------------------------------------------------
// LCD 20x4 Declaration (i2c pins connexion)
//-------------------------------------------------------
#define POS_SPEED_COL 15
#define POS_SPEED_LIG 1
#define NEW_POS_SPEED_COL 15
#define NEW_POS_SPEED_LIG 2
#define NEW_POS_PER_COL   15
#define NEW_POS_PER_LIG   3

//-------------------------------------------------------
// computation for rotation periode with HALL sensor
// préconisation de volatile si utilisé dans interruption
//-------------------------------------------------------
volatile unsigned long periode       = 0;
volatile unsigned long currentmillis  = 0;
volatile unsigned long previousmillis   = 0;

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
// M0   M1    M2    Microstep resolution
// Low    Low   Low   Full step
// High   Low   Low   1/2 step
// Low    High  Low   1/4 step
// High   High  Low   1/8 step
// Low    Low   High  1/16 step
// High   Low   High  1/32 step
// Low    High  High  1/32 step
// High   High  High  1/32 step
//-------------------------------------------------
const int microsteps[] = {1, 2, 4, 8, 16, 32, 32, 32};
#define MICROSTEPS_TAB_SIZE sizeof(microsteps)/sizeof(int)
unsigned int microsteps_ratio = 0;

#define ligneTab  10      // 0 à 9
#define colonneTab  6       // 0 à 5
const int speed_prog[ligneTab][colonneTab] = {
  //MaxSpeed,   M2, M1, M0 , Enable_n, target(ms)
  /*00*/  { 0,      0,  0,  0,   1,        0    } ,   // Stop
  /*01*/  { 213,    1,  0,  1,   0,        10000} ,   // 10s  0.1 Hz    6  rpm     ~10033ms
  /*02*/  { 285,    1,  0,  1,   0,        7500 } ,   // 7.5s 0.13Hz    15 rpm     ~ 7503ms
  /*03*/  { 426 ,   1,  0,  1,   0,        5000 } ,   // 5.0s 0.2 Hz    30 rpm     ~ 5016ms
  /*04*/  { 214,    0,  1,  1,   0,        2500 } ,   // 2.5s 0.4 Hz    45 rpm     ~ 2497ms
  /*05*/  { 267,    0,  1,  0,   0,        1000 } ,   // 1.0s 1.0 Hz    60 rpm     ~ 1001ms
  /*06*/  { 357,    0,  1,  0,   0,        750  } ,   // 0.75s  1.33Hz    150 rpm    ~  751ms
  /*07*/  { 267,    0,  0,  1,   0,        500  } ,   // 0.50s  2.0 Hz    120 rpm    ~  500ms
  /*08*/  { 534,    0,  0,  1,   0,        250  } ,   // 0.25s  4.0 Hz    240 rpm    ~  250ms
  /*09*/  { 535,    0,  0,  0,   0,        125  }     // 0.125s 8.0 Hz    480 rpm    ~  125ms

  /*  Autres parametres de réglages possibles
    { 666,      0,  0,  0,   0,        100  }     // 0.10s  10  Hz    600 rpm    ~  101ms
  */
};
