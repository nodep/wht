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
LIBS:special
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
LIBS:pogopins-cache
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
L NRF24LE1_PROG U1
U 1 1 55BE2B56
P 4950 2150
F 0 "U1" H 4950 1800 60  0000 C CNN
F 1 "NRF24LE1_PROG" H 4950 2500 60  0000 C CNN
F 2 "wht:NRF_ICSP_PogoPinsHoles" H 4900 2050 60  0001 C CNN
F 3 "" H 4900 2050 60  0000 C CNN
	1    4950 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 2000 4100 2000
Wire Wire Line
	4100 2100 4450 2100
Wire Wire Line
	4450 2200 4100 2200
Wire Wire Line
	4100 2300 4450 2300
Wire Wire Line
	5450 2000 5800 2000
Wire Wire Line
	5450 2100 5800 2100
Wire Wire Line
	5450 2200 5800 2200
Wire Wire Line
	5450 2300 5800 2300
Text Label 4150 2000 0    60   ~ 0
VDD
Text Label 4150 2100 0    60   ~ 0
PROG
Text Label 4150 2200 0    60   ~ 0
RESET
Text Label 4150 2300 0    60   ~ 0
GND
Text Label 5500 2000 0    60   ~ 0
FSCK
Text Label 5500 2100 0    60   ~ 0
FMOSI
Text Label 5500 2200 0    60   ~ 0
FMISO
Text Label 5500 2300 0    60   ~ 0
FCSN
$Comp
L NRF24LE1_PROG U2
U 1 1 55BE3082
P 4950 3150
F 0 "U2" H 4950 2800 60  0000 C CNN
F 1 "NRF24LE1_PROG" H 4950 3500 60  0000 C CNN
F 2 "wht:NRF_ICSP_RibbonCable" H 4900 3050 60  0001 C CNN
F 3 "" H 4900 3050 60  0000 C CNN
	1    4950 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4450 3000 4100 3000
Wire Wire Line
	4100 3100 4450 3100
Wire Wire Line
	4450 3200 4100 3200
Wire Wire Line
	4100 3300 4450 3300
Wire Wire Line
	5450 3000 5800 3000
Wire Wire Line
	5450 3100 5800 3100
Wire Wire Line
	5450 3200 5800 3200
Wire Wire Line
	5450 3300 5800 3300
Text Label 4150 3000 0    60   ~ 0
VDD
Text Label 4150 3100 0    60   ~ 0
PROG
Text Label 4150 3200 0    60   ~ 0
RESET
Text Label 4150 3300 0    60   ~ 0
GND
Text Label 5500 3000 0    60   ~ 0
FSCK
Text Label 5500 3100 0    60   ~ 0
FMOSI
Text Label 5500 3200 0    60   ~ 0
FMISO
Text Label 5500 3300 0    60   ~ 0
FCSN
$EndSCHEMATC
