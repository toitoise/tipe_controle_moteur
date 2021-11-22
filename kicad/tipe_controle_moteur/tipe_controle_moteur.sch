EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Comparator:LM393 U?
U 1 1 619C00C7
P 3150 1850
F 0 "U?" H 3150 2217 50  0000 C CNN
F 1 "LM393" H 3150 2126 50  0000 C CNN
F 2 "" H 3150 1850 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/lm393.pdf" H 3150 1850 50  0001 C CNN
	1    3150 1850
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 619C22B9
P 2550 1450
F 0 "R1" H 2620 1496 50  0000 L CNN
F 1 "R" H 2620 1405 50  0000 L CNN
F 2 "" V 2480 1450 50  0001 C CNN
F 3 "~" H 2550 1450 50  0001 C CNN
	1    2550 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 619C294A
P 2550 2500
F 0 "R2" H 2620 2546 50  0000 L CNN
F 1 "R" H 2620 2455 50  0000 L CNN
F 2 "" V 2480 2500 50  0001 C CNN
F 3 "~" H 2550 2500 50  0001 C CNN
	1    2550 2500
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 619C2F2B
P 3150 2250
F 0 "R3" V 3050 2250 50  0000 C CNN
F 1 "R" V 3250 2250 50  0000 C CNN
F 2 "" V 3080 2250 50  0001 C CNN
F 3 "~" H 3150 2250 50  0001 C CNN
	1    3150 2250
	0    1    1    0   
$EndComp
Wire Wire Line
	2550 1600 2550 1750
Wire Wire Line
	2550 2250 3000 2250
Connection ~ 2550 2250
Wire Wire Line
	2550 2250 2550 2350
Wire Wire Line
	3300 2250 3750 2250
Wire Wire Line
	3750 2250 3750 1850
Wire Wire Line
	3750 1850 3450 1850
Connection ~ 3750 1850
Wire Wire Line
	3750 1850 3750 1600
Wire Wire Line
	3750 1850 4000 1850
Wire Wire Line
	2550 1300 2550 1200
Wire Wire Line
	2550 1200 3150 1200
Wire Wire Line
	3750 1200 3750 1300
Wire Wire Line
	3150 1200 3150 1100
Connection ~ 3150 1200
Wire Wire Line
	3150 1200 3750 1200
$Comp
L power:+5V #PWR?
U 1 1 619C5EED
P 3150 1100
F 0 "#PWR?" H 3150 950 50  0001 C CNN
F 1 "+5V" H 3165 1273 50  0000 C CNN
F 2 "" H 3150 1100 50  0001 C CNN
F 3 "" H 3150 1100 50  0001 C CNN
	1    3150 1100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 619C6669
P 2550 2650
F 0 "#PWR?" H 2550 2400 50  0001 C CNN
F 1 "GND" H 2555 2477 50  0000 C CNN
F 2 "" H 2550 2650 50  0001 C CNN
F 3 "" H 2550 2650 50  0001 C CNN
	1    2550 2650
	1    0    0    -1  
$EndComp
Connection ~ 2550 1750
Wire Wire Line
	2550 1750 2550 2250
Wire Wire Line
	2850 1950 2150 1950
Text GLabel 2150 1950 0    50   Input ~ 0
Capteur_Hall
Text GLabel 4000 1850 2    50   Output ~ 0
Vers_Pin2_Arduino
$Comp
L Device:R R5_pullup
U 1 1 619C1A26
P 3750 1450
F 0 "R5_pullup" H 3820 1496 50  0000 L CNN
F 1 "10K" H 3820 1405 50  0000 L CNN
F 2 "" V 3680 1450 50  0001 C CNN
F 3 "~" H 3750 1450 50  0001 C CNN
	1    3750 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2550 1750 2850 1750
Text Notes 1150 950  0    50   ~ 0
Comparateur à hystéresis
Text Notes 1150 1050 0    50   ~ 0
Entrée capteur à effet Hall s49e
Text Notes 4000 2200 0    50   ~ 0
Sortie vers la pin \nd'interruption N°2\nde l'Arduino
$Comp
L Driver_Motor:Pololu_Breakout_DRV8825 A1
U 1 1 619CB8C7
P 7800 3000
F 0 "A1" H 7800 3689 50  0000 C CNN
F 1 "Pololu_Breakout_DRV8825" H 7800 3780 50  0000 C CNN
F 2 "Module:Pololu_Breakout-16_15.2x20.3mm" H 8000 2200 50  0001 L CNN
F 3 "https://www.pololu.com/product/2982" H 7900 2700 50  0001 C CNN
	1    7800 3000
	-1   0    0    1   
$EndComp
$Comp
L Motor:Stepper_Motor_bipolar M1
U 1 1 619CC403
P 6100 2400
F 0 "M1" H 6022 2075 50  0000 C CNN
F 1 "Stepper_Motor_bipolar" H 6022 2166 50  0000 C CNN
F 2 "" H 6110 2390 50  0001 C CNN
F 3 "http://www.infineon.com/dgdl/Application-Note-TLE8110EE_driving_UniPolarStepperMotor_V1.1.pdf?fileId=db3a30431be39b97011be5d0aa0a00b0" H 6110 2390 50  0001 C CNN
	1    6100 2400
	-1   0    0    1   
$EndComp
Wire Wire Line
	6000 2700 6000 3100
Wire Wire Line
	6000 3100 7400 3100
Wire Wire Line
	6200 2700 6200 3000
Wire Wire Line
	6200 3000 7400 3000
Wire Wire Line
	6400 2300 6800 2300
Wire Wire Line
	6800 2300 6800 2700
Wire Wire Line
	6800 2700 7400 2700
Wire Wire Line
	6600 2800 7400 2800
Wire Wire Line
	6600 2500 6400 2500
Wire Wire Line
	6600 2500 6600 2800
$Comp
L power:GND #PWR?
U 1 1 619D2DDC
P 8000 2000
F 0 "#PWR?" H 8000 1750 50  0001 C CNN
F 1 "GND" H 8005 1827 50  0000 C CNN
F 2 "" H 8000 2000 50  0001 C CNN
F 3 "" H 8000 2000 50  0001 C CNN
	1    8000 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 2000 8000 1900
Wire Wire Line
	8000 1900 7800 1900
Wire Wire Line
	7800 1900 7800 2200
Wire Wire Line
	7800 1900 7700 1900
Wire Wire Line
	7700 1900 7700 2200
Connection ~ 7800 1900
$Comp
L power:GND #PWR?
U 1 1 619D4139
P 5600 1200
F 0 "#PWR?" H 5600 950 50  0001 C CNN
F 1 "GND" H 5500 1100 50  0000 C CNN
F 2 "" H 5600 1200 50  0001 C CNN
F 3 "" H 5600 1200 50  0001 C CNN
	1    5600 1200
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Female J1
U 1 1 619D5087
P 5100 950
F 0 "J1" H 5000 1050 50  0000 C CNN
F 1 "Alimentation Externe" H 4950 750 50  0000 C CNN
F 2 "" H 5100 950 50  0001 C CNN
F 3 "~" H 5100 950 50  0001 C CNN
	1    5100 950 
	-1   0    0    1   
$EndComp
Wire Wire Line
	5700 3600 7800 3600
Wire Wire Line
	5300 950  5600 950 
Text GLabel 6700 850  2    50   Input ~ 0
12volts
$Comp
L Device:CP C1
U 1 1 619DAE77
P 5800 1000
F 0 "C1" H 5900 1000 50  0000 L CNN
F 1 "100u" H 5850 900 50  0000 L CNN
F 2 "" H 5838 850 50  0001 C CNN
F 3 "~" H 5800 1000 50  0001 C CNN
	1    5800 1000
	1    0    0    -1  
$EndComp
Connection ~ 5800 850 
Wire Notes Line
	4750 600  6550 600 
Wire Wire Line
	5700 1650 5700 3600
Wire Wire Line
	5300 850  5800 850 
Wire Wire Line
	5600 950  5600 1150
Wire Wire Line
	5800 1150 5600 1150
Connection ~ 5600 1150
Wire Wire Line
	5600 1150 5600 1200
Text GLabel 9850 2200 0    50   Output ~ 0
PinXX_arduino
Text GLabel 9850 2350 0    50   Output ~ 0
PinXX_arduino
Text GLabel 9850 2500 0    50   Output ~ 0
PinXX_arduino
Text GLabel 9850 2650 0    50   Output ~ 0
PinXX_arduino
Text GLabel 9850 2800 0    50   Output ~ 0
PinXX_arduino
Text GLabel 9850 2950 0    50   Output ~ 0
PinXX_arduino
$Comp
L Display_Character:LCD_2004-I2C U?
U 1 1 619E652E
P 5500 5500
F 0 "U?" H 5930 5546 50  0000 L CNN
F 1 "LCD_2004-I2C" H 5930 5455 50  0000 L CNN
F 2 "Display:LCD1602-I2C" H 5500 4900 50  0001 C CNN
F 3 "http://www.lcd-module.de/pdf/doma/t123-i2c.pdf" H 5500 5000 50  0001 C CNN
	1    5500 5500
	1    0    0    -1  
$EndComp
Text GLabel 2100 6050 0    50   Output ~ 0
PinXX_arduino
Text GLabel 2100 5350 0    50   Output ~ 0
PinXX_arduino
$Comp
L Device:Rotary_Encoder SW?
U 1 1 619F516F
P 2900 5700
F 0 "SW?" H 2900 6067 50  0000 C CNN
F 1 "Rotary_Encoder" H 2900 5976 50  0000 C CNN
F 2 "" H 2750 5860 50  0001 C CNN
F 3 "~" H 2900 5960 50  0001 C CNN
	1    2900 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 5800 4000 5800
Wire Wire Line
	3200 5600 4000 5600
Wire Wire Line
	4400 5600 4400 5650
Wire Wire Line
	4400 5650 5000 5650
$Comp
L power:GND #PWR?
U 1 1 619F846A
P 4000 5900
F 0 "#PWR?" H 4000 5650 50  0001 C CNN
F 1 "GND" H 4005 5727 50  0000 C CNN
F 2 "" H 4000 5900 50  0001 C CNN
F 3 "" H 4000 5900 50  0001 C CNN
	1    4000 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 5900 4000 5800
Connection ~ 4000 5800
Wire Wire Line
	4000 5800 5000 5800
$Comp
L power:+5V #PWR?
U 1 1 619F940C
P 4000 5500
F 0 "#PWR?" H 4000 5350 50  0001 C CNN
F 1 "+5V" H 4015 5673 50  0000 C CNN
F 2 "" H 4000 5500 50  0001 C CNN
F 3 "" H 4000 5500 50  0001 C CNN
	1    4000 5500
	1    0    0    -1  
$EndComp
Wire Wire Line
	4000 5500 4000 5600
Connection ~ 4000 5600
Wire Wire Line
	4000 5600 4400 5600
Text GLabel 2100 5700 0    50   Output ~ 0
PinXX_arduino
Wire Wire Line
	2100 5350 2350 5350
Wire Wire Line
	2350 5350 2350 5600
Wire Wire Line
	2350 5600 2600 5600
Wire Wire Line
	2600 5700 2100 5700
Wire Wire Line
	2100 6050 2350 6050
Wire Wire Line
	2350 6050 2350 5800
Wire Wire Line
	2350 5800 2600 5800
Text GLabel 2100 4650 0    50   Output ~ 0
Pin_SCL_arduino
Text GLabel 2100 5000 0    50   Output ~ 0
Pin_SDA_arduino
Wire Wire Line
	2100 4650 4550 4650
Wire Wire Line
	4550 4650 4550 5200
Wire Wire Line
	4550 5200 5000 5200
Wire Wire Line
	5000 5300 4350 5300
Wire Wire Line
	4350 5300 4350 5000
Wire Wire Line
	4350 5000 2100 5000
Wire Notes Line
	6850 4250 6850 6500
Wire Notes Line
	6850 6500 1250 6500
Wire Notes Line
	1250 6500 1250 4250
Text Notes 3550 4350 0    50   ~ 0
Panneau de controle Moteur
Wire Notes Line
	5100 1350 5100 4250
Wire Notes Line
	4750 1350 10200 1350
Wire Notes Line
	1250 4250 10200 4250
Text Notes 8550 1500 0    50   ~ 0
Driver Moteur Pas à Pas DRV8825
Text Notes 7450 7500 0    50   ~ 0
Chematique carte controle moteur et affichage
Text Notes 8200 7650 0    50   ~ 0
21/11/2021
Text Notes 10600 7650 0    50   ~ 0
1.0
Text Notes 7200 6900 0    197  ~ 0
Controle moteur
$Comp
L power:+5V #PWR?
U 1 1 61A0B011
P 8650 4950
F 0 "#PWR?" H 8650 4800 50  0001 C CNN
F 1 "+5V" H 8665 5123 50  0000 C CNN
F 2 "" H 8650 4950 50  0001 C CNN
F 3 "" H 8650 4950 50  0001 C CNN
	1    8650 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 61A0B728
P 8650 5650
F 0 "#PWR?" H 8650 5400 50  0001 C CNN
F 1 "GND" H 8655 5477 50  0000 C CNN
F 2 "" H 8650 5650 50  0001 C CNN
F 3 "" H 8650 5650 50  0001 C CNN
	1    8650 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8650 4950 8650 5100
Wire Wire Line
	8650 5100 9100 5100
Wire Wire Line
	8650 5650 8650 5450
Wire Wire Line
	8650 5450 9100 5450
Text GLabel 9100 5100 2    50   Input ~ 0
From_Arduino_board
Text GLabel 9100 5450 2    50   Input ~ 0
From_Arduino_board
Wire Notes Line
	6850 6200 10200 6200
Wire Notes Line
	10200 1350 10200 6200
Text Notes 8050 4450 0    50   ~ 0
Alimentation depuis la carte Arduino
Wire Notes Line
	4750 600  4750 1350
Wire Notes Line
	1100 2950 1100 850 
Wire Notes Line
	1100 850  4750 850 
Wire Notes Line
	1100 2950 5100 2950
Text Notes 1300 2200 0    50   ~ 0
Pin A0 du module Hall\n    (direct capteur)
Wire Wire Line
	5800 850  6250 850 
Wire Wire Line
	6550 1650 6550 850 
Wire Wire Line
	5700 1650 6550 1650
Connection ~ 6550 850 
Wire Wire Line
	6550 850  6700 850 
$Comp
L Device:C C2
U 1 1 61A19E26
P 6250 1000
F 0 "C2" H 6350 1000 50  0000 L CNN
F 1 "100n" H 6250 900 50  0000 L CNN
F 2 "" H 6288 850 50  0001 C CNN
F 3 "~" H 6250 1000 50  0001 C CNN
	1    6250 1000
	1    0    0    -1  
$EndComp
Connection ~ 6250 850 
Wire Wire Line
	6250 850  6550 850 
Wire Wire Line
	5800 1150 6250 1150
Connection ~ 5800 1150
$EndSCHEMATC
