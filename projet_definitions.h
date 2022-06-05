//-------------------------------------------------------
// Define drv8825 pin connections to Arduino UNO
//-------------------------------------------------------
#define dir       3
#define step      4
#define sleep_n   5   // On ne l'utilise pas, connexion au VCC
#define reset_n   6
#define M2        7
#define M1        8
#define M0        9
#define enable_n  10

//-------------------------------------------------------
// Interruption sur la pin 2 venant du comparateur
//-------------------------------------------------------
#define comparator_pin    2

//-------------------------------------------------------
// Calcu de la periode de rotation
// préconisation de volatile si utilisé dans interruption
//-------------------------------------------------------
volatile unsigned long periode        = 0;
volatile unsigned long currentmillis  = 0;
volatile unsigned long previousmillis = 0;

// Variables traitement vitesse
int motor_speed_cur     = 0;     // current speed
int motor_speed_new     = 0;     // new speed

//----Microstep resolution--------------------------
// M2    M1    M0    
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
  //      MaxSpeed theorique = Steps_par_tour x (freq motor/3) * microsteps
  //      nb_steps_one_revolution theorique = ( Steps_par_tour / 3 ) * microsteps
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
};

//-------------------------------------------------------
// Various for MOTOR and ACCEL
//-------------------------------------------------------
#define MOTOR_ITFC_TYPE        1        // Motor interface type (DRV8825 dir+steps)
#define RATIO_ENGRENAGE        3        // Rapport entre les 2 poulies

//-------------------------------------------------------
// A positionner par l'utilisateur
//-------------------------------------------------------
#define NOMBRE_ROTATIONS          500      // Nombre de tours a effectuer
#define MOTOR_SPEED               3        // Valeur legale corespond au tableau speed_prog : 0 à 9 


long computed_steps_for_rotations=0;
