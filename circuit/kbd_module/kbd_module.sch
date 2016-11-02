EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:wht
LIBS:nrf24le1
LIBS:kbd_module-cache
EELAYER 25 0
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
L D D3
U 1 1 575580D3
P 4150 2450
F 0 "D3" H 4150 2550 50  0000 C CNN
F 1 "1n4148" H 4150 2350 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4150 2450 50  0001 C CNN
F 3 "" H 4150 2450 50  0000 C CNN
	1    4150 2450
	0    1    1    0   
$EndComp
$Comp
L D D6
U 1 1 57558130
P 4550 2450
F 0 "D6" H 4550 2550 50  0000 C CNN
F 1 "1n4148" H 4550 2350 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4550 2450 50  0001 C CNN
F 3 "" H 4550 2450 50  0000 C CNN
	1    4550 2450
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW5
U 1 1 575585C1
P 4150 1650
F 0 "SW5" H 4150 1600 40  0000 C CNN
F 1 "SW_SIMPLE" H 4150 1750 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 4150 1650 60  0001 C CNN
F 3 "" H 4150 1650 60  0000 C CNN
	1    4150 1650
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW6
U 1 1 57558BE3
P 4550 1650
F 0 "SW6" H 4550 1600 40  0000 C CNN
F 1 "SW_SIMPLE" H 4550 1750 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 4550 1650 60  0001 C CNN
F 3 "" H 4550 1650 60  0000 C CNN
	1    4550 1650
	0    -1   -1   0   
$EndComp
Text Label 3800 1400 0    60   ~ 0
FSCK
Text Label 3800 2700 0    60   ~ 0
FCSN
$Comp
L SW_SIMPLE SW3
U 1 1 5755A10E
P 4150 2950
F 0 "SW3" H 4150 2900 40  0000 C CNN
F 1 "SW_SIMPLE" H 4150 3050 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 4150 2950 60  0001 C CNN
F 3 "" H 4150 2950 60  0000 C CNN
	1    4150 2950
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW4
U 1 1 5755A20E
P 4550 2950
F 0 "SW4" H 4550 2900 40  0000 C CNN
F 1 "SW_SIMPLE" H 4550 3050 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 4550 2950 60  0001 C CNN
F 3 "" H 4550 2950 60  0000 C CNN
	1    4550 2950
	0    -1   -1   0   
$EndComp
$Comp
L D D4
U 1 1 5755A2C2
P 4150 3350
F 0 "D4" H 4150 3450 50  0000 C CNN
F 1 "1n4148" H 4150 3250 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4150 3350 50  0001 C CNN
F 3 "" H 4150 3350 50  0000 C CNN
	1    4150 3350
	0    1    1    0   
$EndComp
$Comp
L D D7
U 1 1 5755A30B
P 4550 3350
F 0 "D7" H 4550 3450 50  0000 C CNN
F 1 "1n4148" H 4550 3250 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4550 3350 50  0001 C CNN
F 3 "" H 4550 3350 50  0000 C CNN
	1    4550 3350
	0    -1   -1   0   
$EndComp
Text Label 3800 4100 0    60   ~ 0
FMISO
$Comp
L D D9
U 1 1 5755AC21
P 4950 2900
F 0 "D9" H 4950 3000 50  0000 C CNN
F 1 "1n4148" H 4950 2800 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4950 2900 50  0001 C CNN
F 3 "" H 4950 2900 50  0000 C CNN
	1    4950 2900
	0    1    1    0   
$EndComp
$Comp
L D D10
U 1 1 5755AC27
P 5350 2900
F 0 "D10" H 5350 3000 50  0000 C CNN
F 1 "1n4148" H 5350 2800 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 5350 2900 50  0001 C CNN
F 3 "" H 5350 2900 50  0000 C CNN
	1    5350 2900
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW7
U 1 1 5755AC2D
P 4950 2500
F 0 "SW7" H 4950 2450 40  0000 C CNN
F 1 "SW_SIMPLE" H 4950 2600 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 4950 2500 60  0001 C CNN
F 3 "" H 4950 2500 60  0000 C CNN
	1    4950 2500
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW8
U 1 1 5755AC33
P 5350 2500
F 0 "SW8" H 5350 2450 40  0000 C CNN
F 1 "SW_SIMPLE" H 5350 2600 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 5350 2500 60  0001 C CNN
F 3 "" H 5350 2500 60  0000 C CNN
	1    5350 2500
	0    -1   -1   0   
$EndComp
$Comp
L D D5
U 1 1 5755B276
P 4150 4750
F 0 "D5" H 4150 4850 50  0000 C CNN
F 1 "1n4148" H 4150 4650 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4150 4750 50  0001 C CNN
F 3 "" H 4150 4750 50  0000 C CNN
	1    4150 4750
	0    1    1    0   
$EndComp
$Comp
L D D8
U 1 1 5755B27C
P 4550 4750
F 0 "D8" H 4550 4850 50  0000 C CNN
F 1 "1n4148" H 4550 4650 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4550 4750 50  0001 C CNN
F 3 "" H 4550 4750 50  0000 C CNN
	1    4550 4750
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW11
U 1 1 5755B282
P 4150 4350
F 0 "SW11" H 4150 4300 40  0000 C CNN
F 1 "SW_SIMPLE" H 4150 4450 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 4150 4350 60  0001 C CNN
F 3 "" H 4150 4350 60  0000 C CNN
	1    4150 4350
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW12
U 1 1 5755B288
P 4550 4350
F 0 "SW12" H 4550 4300 40  0000 C CNN
F 1 "SW_SIMPLE" H 4550 4450 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 4550 4350 60  0001 C CNN
F 3 "" H 4550 4350 60  0000 C CNN
	1    4550 4350
	0    -1   -1   0   
$EndComp
Text Label 3800 5500 0    60   ~ 0
FMOSI
$Comp
L D D1
U 1 1 5755BE07
P 3100 4300
F 0 "D1" H 3100 4400 50  0000 C CNN
F 1 "1n4148" H 3100 4200 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 3100 4300 50  0001 C CNN
F 3 "" H 3100 4300 50  0000 C CNN
	1    3100 4300
	0    1    1    0   
$EndComp
$Comp
L D D2
U 1 1 5755BE0D
P 3500 4300
F 0 "D2" H 3500 4400 50  0000 C CNN
F 1 "1n4148" H 3500 4200 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 3500 4300 50  0001 C CNN
F 3 "" H 3500 4300 50  0000 C CNN
	1    3500 4300
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW1
U 1 1 5755BE13
P 3100 3900
F 0 "SW1" H 3100 3850 40  0000 C CNN
F 1 "SW_SIMPLE" H 3100 4000 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 3100 3900 60  0001 C CNN
F 3 "" H 3100 3900 60  0000 C CNN
	1    3100 3900
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW2
U 1 1 5755BE19
P 3500 3900
F 0 "SW2" H 3500 3850 40  0000 C CNN
F 1 "SW_SIMPLE" H 3500 4000 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 3500 3900 60  0001 C CNN
F 3 "" H 3500 3900 60  0000 C CNN
	1    3500 3900
	0    -1   -1   0   
$EndComp
$Comp
L D D11
U 1 1 5755C929
P 5750 3350
F 0 "D11" H 5750 3450 50  0000 C CNN
F 1 "1n4148" H 5750 3250 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 5750 3350 50  0001 C CNN
F 3 "" H 5750 3350 50  0000 C CNN
	1    5750 3350
	0    1    1    0   
$EndComp
$Comp
L D D12
U 1 1 5755C92F
P 6150 3350
F 0 "D12" H 6150 3450 50  0000 C CNN
F 1 "1n4148" H 6150 3250 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 6150 3350 50  0001 C CNN
F 3 "" H 6150 3350 50  0000 C CNN
	1    6150 3350
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW9
U 1 1 5755C935
P 5750 2950
F 0 "SW9" H 5750 2900 40  0000 C CNN
F 1 "SW_SIMPLE" H 5750 3050 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 5750 2950 60  0001 C CNN
F 3 "" H 5750 2950 60  0000 C CNN
	1    5750 2950
	0    -1   -1   0   
$EndComp
$Comp
L SW_SIMPLE SW10
U 1 1 5755C93B
P 6150 2950
F 0 "SW10" H 6150 2900 40  0000 C CNN
F 1 "SW_SIMPLE" H 6150 3050 40  0001 C CNN
F 2 "kbd_module:ExtSwitch" H 6150 2950 60  0001 C CNN
F 3 "" H 6150 2950 60  0000 C CNN
	1    6150 2950
	0    -1   -1   0   
$EndComp
$Comp
L NRF24LE1_PROG U1
U 1 1 57556B79
P 2350 2100
F 0 "U1" H 2350 1750 60  0000 C CNN
F 1 "NRF24LE1_PROG" H 2350 2450 60  0000 C CNN
F 2 "kbd_module:NRF_ICSP_RibbonCable" H 2300 2000 60  0001 C CNN
F 3 "" H 2300 2000 60  0000 C CNN
	1    2350 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 1500 4150 1400
Wire Wire Line
	3750 1400 6150 1400
Wire Wire Line
	4550 1400 4550 1500
Connection ~ 4150 1400
Wire Wire Line
	4150 2600 4150 2800
Wire Wire Line
	3100 2700 4550 2700
Wire Wire Line
	4550 2600 4550 2800
Connection ~ 4150 2700
Connection ~ 4550 2700
Wire Wire Line
	4150 3100 4150 3200
Wire Wire Line
	4550 3100 4550 3200
Wire Wire Line
	4150 3900 4150 4200
Wire Wire Line
	3750 4100 5350 4100
Wire Wire Line
	4550 3900 4550 4200
Connection ~ 4150 4100
Wire Wire Line
	4950 2650 4950 2750
Wire Wire Line
	5350 2650 5350 2750
Wire Wire Line
	4950 1400 4950 2350
Connection ~ 4550 1400
Wire Wire Line
	5350 1400 5350 2350
Connection ~ 4950 1400
Connection ~ 4550 4100
Connection ~ 4950 4100
Wire Wire Line
	4150 4500 4150 4600
Wire Wire Line
	4550 4500 4550 4600
Wire Wire Line
	3100 5500 6150 5500
Connection ~ 4150 5500
Wire Wire Line
	3100 4050 3100 4150
Wire Wire Line
	3500 4050 3500 4150
Wire Wire Line
	3500 2700 3500 3750
Wire Wire Line
	3100 2700 3100 3750
Connection ~ 3500 2700
Connection ~ 3500 5500
Wire Wire Line
	5750 3100 5750 3200
Wire Wire Line
	6150 3100 6150 3200
Wire Wire Line
	5750 1400 5750 2800
Connection ~ 5350 1400
Wire Wire Line
	6150 1400 6150 2800
Connection ~ 5750 1400
Connection ~ 4550 5500
Connection ~ 5750 5500
NoConn ~ 1850 1950
NoConn ~ 1850 2050
NoConn ~ 1850 2150
NoConn ~ 1850 2250
Wire Wire Line
	2850 1950 3150 1950
Wire Wire Line
	2850 2050 3150 2050
Wire Wire Line
	2850 2150 3150 2150
Wire Wire Line
	2850 2250 3150 2250
Text Label 2850 1950 0    60   ~ 0
FSCK
Text Label 2850 2250 0    60   ~ 0
FCSN
Text Label 2850 2150 0    60   ~ 0
FMISO
Text Label 2850 2050 0    60   ~ 0
FMOSI
$Comp
L D D15
U 1 1 5819C29A
P 4150 2050
F 0 "D15" H 4150 2150 50  0000 C CNN
F 1 "1n4148" H 4150 1950 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4150 2050 50  0001 C CNN
F 3 "" H 4150 2050 50  0000 C CNN
	1    4150 2050
	0    1    1    0   
$EndComp
$Comp
L D D18
U 1 1 5819C308
P 4550 2050
F 0 "D18" H 4550 2150 50  0000 C CNN
F 1 "1n4148" H 4550 1950 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4550 2050 50  0001 C CNN
F 3 "" H 4550 2050 50  0000 C CNN
	1    4550 2050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4150 1800 4150 1900
Wire Wire Line
	4150 2200 4150 2300
Wire Wire Line
	4550 1800 4550 1900
Wire Wire Line
	4550 2200 4550 2300
$Comp
L D D16
U 1 1 5819D53F
P 4150 3750
F 0 "D16" H 4150 3850 50  0000 C CNN
F 1 "1n4148" H 4150 3650 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4150 3750 50  0001 C CNN
F 3 "" H 4150 3750 50  0000 C CNN
	1    4150 3750
	0    1    1    0   
$EndComp
$Comp
L D D19
U 1 1 5819D5AF
P 4550 3750
F 0 "D19" H 4550 3850 50  0000 C CNN
F 1 "1n4148" H 4550 3650 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4550 3750 50  0001 C CNN
F 3 "" H 4550 3750 50  0000 C CNN
	1    4550 3750
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4150 3500 4150 3600
Wire Wire Line
	4550 3500 4550 3600
$Comp
L D D17
U 1 1 5819E2EC
P 4150 5150
F 0 "D17" H 4150 5250 50  0000 C CNN
F 1 "1n4148" H 4150 5050 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4150 5150 50  0001 C CNN
F 3 "" H 4150 5150 50  0000 C CNN
	1    4150 5150
	0    1    1    0   
$EndComp
$Comp
L D D20
U 1 1 5819E36A
P 4550 5150
F 0 "D20" H 4550 5250 50  0000 C CNN
F 1 "1n4148" H 4550 5050 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4550 5150 50  0001 C CNN
F 3 "" H 4550 5150 50  0000 C CNN
	1    4550 5150
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4150 4900 4150 5000
Wire Wire Line
	4550 4900 4550 5000
Wire Wire Line
	4150 5300 4150 5500
Wire Wire Line
	4550 5300 4550 5500
$Comp
L D D14
U 1 1 5819F0CE
P 3500 4750
F 0 "D14" H 3500 4850 50  0000 C CNN
F 1 "1n4148" H 3500 4650 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 3500 4750 50  0001 C CNN
F 3 "" H 3500 4750 50  0000 C CNN
	1    3500 4750
	0    -1   -1   0   
$EndComp
$Comp
L D D13
U 1 1 5819F14C
P 3100 4750
F 0 "D13" H 3100 4850 50  0000 C CNN
F 1 "1n4148" H 3100 4650 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 3100 4750 50  0001 C CNN
F 3 "" H 3100 4750 50  0000 C CNN
	1    3100 4750
	0    1    1    0   
$EndComp
Wire Wire Line
	3100 4450 3100 4600
Wire Wire Line
	3500 4450 3500 4600
Wire Wire Line
	3500 4900 3500 5500
Wire Wire Line
	3100 4900 3100 5500
$Comp
L D D21
U 1 1 5819FAA8
P 4950 3350
F 0 "D21" H 4950 3450 50  0000 C CNN
F 1 "1n4148" H 4950 3250 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 4950 3350 50  0001 C CNN
F 3 "" H 4950 3350 50  0000 C CNN
	1    4950 3350
	0    1    1    0   
$EndComp
$Comp
L D D22
U 1 1 5819FB32
P 5350 3350
F 0 "D22" H 5350 3450 50  0000 C CNN
F 1 "1n4148" H 5350 3250 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 5350 3350 50  0001 C CNN
F 3 "" H 5350 3350 50  0000 C CNN
	1    5350 3350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4950 3050 4950 3200
Wire Wire Line
	4950 3500 4950 4100
Wire Wire Line
	5350 4100 5350 3500
Wire Wire Line
	5350 3200 5350 3050
$Comp
L D D23
U 1 1 581A002A
P 5750 3800
F 0 "D23" H 5750 3900 50  0000 C CNN
F 1 "1n4148" H 5750 3700 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 5750 3800 50  0001 C CNN
F 3 "" H 5750 3800 50  0000 C CNN
	1    5750 3800
	0    1    1    0   
$EndComp
$Comp
L D D24
U 1 1 581A00BA
P 6150 3800
F 0 "D24" H 6150 3900 50  0000 C CNN
F 1 "1n4148" H 6150 3700 50  0000 C CNN
F 2 "kbd_module:SOD27_RM5_5" H 6150 3800 50  0001 C CNN
F 3 "" H 6150 3800 50  0000 C CNN
	1    6150 3800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5750 3500 5750 3650
Wire Wire Line
	5750 3950 5750 5500
Wire Wire Line
	6150 5500 6150 3950
Wire Wire Line
	6150 3650 6150 3500
$EndSCHEMATC
