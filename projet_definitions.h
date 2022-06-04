//-------------------------------------------------------
// EEPROM rotations  (unsigned long int)
//-------------------------------------------------------
#define EEPROM_ADDR_ROTATION_SAVED 0

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

//-------------------------------------------------------
// Comparator input pin with interrupt support
//-------------------------------------------------------
#define comparator_pin    2

//-------------------------------------------------------
// computation for rotation periode with HALL sensor
// préconisation de volatile si utilisé dans interruption
//-------------------------------------------------------
volatile unsigned long periode        = 0;
volatile unsigned long currentmillis  = 0;
volatile unsigned long previousmillis = 0;

// Variables traitement bouton rotatif et click
int clk_state_last      = HIGH;           // Idle
int clk_state           = HIGH;            // Idle
int button_state        = HIGH;           // Not pressed
unsigned long button_millis_save = 0;
unsigned long button_millis_cur = 0;

// Variables traitement vitesse
int motor_speed_cur     = 0;     // current speed
int motor_speed_new     = 0;     // new speed

//-------------------------------------------------
// M2    M1    M0    Microstep resolution
// Low   Low   Low   Full step
// Low   Low   High  1/2 step
// Low   High  Low   1/4 step
// Low   High  High  1/8 step
// High  Low   Low   1/16 step
// High  Low   High  1/32 step
// High  High  Low   1/32 step
// High  High  High  1/32 step
//-------------------------------------------------
const int microsteps[] = { 1, 2, 4, 8, 16, 32, 32, 32};
#define MICROSTEPS_TAB_SIZE sizeof(microsteps)/sizeof(int)

#define ligneTab    10      // 0 à 9
#define colonneTab  8       // 0 à 7
const int speed_prog[ligneTab][colonneTab] = {
  //      MaxSpeed, M2, M1, M0, Enable_n, target(ms), nb_steps_one_revolution, Accel_for_smoothness
  /*00*/  {      0,  0,  0,  0,        1,      0    ,                       0, 0       } ,   // Stop
  /*01*/  {    213,  1,  0,  1,        0,      10000,                    2150, 800     } ,   // 10s     0.1 Hz   obs~10033ms
  /*02*/  {    285,  1,  0,  1,        0,      7500 ,                    2150, 900     } ,   // 7.5s    0.13Hz   obs~ 7503ms
  /*03*/  {    428,  1,  0,  1,        0,      5000 ,                    2150, 1000    } ,   // 5.0s    0.2 Hz   obs~ 5016ms
  /*04*/  {    855,  1,  0,  1,        0,      2500 ,                    2150, 1200    } ,   // 2.5s    0.4 Hz   obs~ 2497ms
  /*05*/  {   1080,  1,  0,  0,        0,      1000 ,                    1060, 3000    } ,   // 1.0s    1.0 Hz   obs~ 1001ms
  /*06*/  {    717,  0,  1,  1,        0,      750  ,                    530 , 3000    } ,   // 0.75s   1.33Hz   obs~  751ms
  /*07*/  {   1080,  0,  1,  1,        0,      500  ,                    530 , 3000    } ,   // 0.50s   2.0 Hz   obs~  500ms
  /*08*/  {   1076,  0,  1,  0,        0,      250  ,                    266 , 3000    } ,   // 0.25s   4.0 Hz   obs~  250ms
  /*09*/  {    805,  0,  0,  1,        0,      167  ,                    134 , 3000    }     // 0.167s  6.0 Hz   obs~  167ms

// OLD OK 
//  /*04*/  { 214,    0,  1,  1,   0,        2500        , 530 } ,   // 2.5s    0.4 Hz    45 rpm     ~ 2497ms
//  /*05*/  { 267,    0,  1,  0,   0,        1000        , 266 } ,   // 1.0s    1.0 Hz    60 rpm     ~ 1001ms
//  /*06*/  { 357,    0,  1,  0,   0,        750         , 266 } ,   // 0.75s   1.33Hz    150 rpm    ~  751ms
//  /*07*/  { 267,    0,  0,  1,   0,        500         , 134 } ,   // 0.50s   2.0 Hz    120 rpm    ~  500ms
//  /*08*/  { 534,    0,  0,  1,   0,        250         , 134 } ,   // 0.25s   4.0 Hz    240 rpm    ~  250ms
};

//-------------------------------------------------------
// Various for MOTOR and ACCEL
//-------------------------------------------------------
#define MAX_ROTATIONS       100      // Max steps for motor
#define HIGHER_BOUNDARY_ROTATIONS 10000
#define motorInterfaceType  1       // Motor interface type (DRV8825 dir+steps)
#define DEFAULT_MAX_SPEED   100     // At beginning
#define DEFAULT_ACCEL       100    // Steps to max speed
#define STEPS_PER_REVOLUTION 200      // 360°/1.8°=200
#define RATIO_ENGRENAGE     3

long nb_of_turns_to_go          =0;
long computed_steps_for_rotation=0;

//-------------------------------------------------------
// LCD 20x4 Declaration (i2c pins connexion)
//-------------------------------------------------------
#define POS_CURPER_COL       15
#define POS_CURPER_LIG       1

#define POS_NEWPER_COL     15
#define POS_NEWPER_LIG     2

#define POS_NB_ROTATION_COL     15
#define POS_NB_ROTATION_LIG     3
