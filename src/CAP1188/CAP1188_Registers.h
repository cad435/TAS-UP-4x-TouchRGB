#pragma once

// CAP1188 Register Addresses
#define CAP1188_MAIN_CONTROL          0x00
#define CAP1188_MAIN_INTERRUPT        0x01
#define CAP1188_GENERAL_STATUS        0x02
#define CAP1188_SENSOR_INPUT_STATUS   0x03
#define CAP1188_LED_STATUS            0x04
#define CAP1188_NOISE_FLAG_STATUS     0x0A

// Sensor Input Delta Count Registers
#define CAP1188_SENSOR_INPUT_1_DELTA  0x10
#define CAP1188_SENSOR_INPUT_2_DELTA  0x11
#define CAP1188_SENSOR_INPUT_3_DELTA  0x12
#define CAP1188_SENSOR_INPUT_4_DELTA  0x13
#define CAP1188_SENSOR_INPUT_5_DELTA  0x14
#define CAP1188_SENSOR_INPUT_6_DELTA  0x15
#define CAP1188_SENSOR_INPUT_7_DELTA  0x16
#define CAP1188_SENSOR_INPUT_8_DELTA  0x17

// Sensitivity Control Register
#define CAP1188_SENSITIVITY_CONTROL   0x1F

// Configuration Registers
#define CAP1188_CONFIGURATION         0x20
#define CAP1188_SENSOR_INPUT_ENABLE   0x21
#define CAP1188_SENSOR_INPUT_CONFIG   0x22
#define CAP1188_SENSOR_INPUT_CONFIG_2 0x23
#define CAP1188_AVG_SAMPLING_CONFIG   0x24
#define CAP1188_CALIBRATION_ACTIVATE  0x26
#define CAP1188_INTERRUPT_ENABLE      0x27
#define CAP1188_REPEAT_RATE_ENABLE    0x28
#define CAP1188_MULTIPLE_TOUCH_CONFIG 0x2A
#define CAP1188_RECALIBRATION_CONFIG  0x2F

// Sensor Input Threshold Registers
#define CAP1188_SENSOR_INPUT_1_THRESH 0x30
#define CAP1188_SENSOR_INPUT_2_THRESH 0x31
#define CAP1188_SENSOR_INPUT_3_THRESH 0x32
#define CAP1188_SENSOR_INPUT_4_THRESH 0x33
#define CAP1188_SENSOR_INPUT_5_THRESH 0x34
#define CAP1188_SENSOR_INPUT_6_THRESH 0x35
#define CAP1188_SENSOR_INPUT_7_THRESH 0x36
#define CAP1188_SENSOR_INPUT_8_THRESH 0x37

// Standby Configuration Registers
#define CAP1188_STANDBY_CHANNEL       0x40
#define CAP1188_STANDBY_CONFIG        0x41
#define CAP1188_STANDBY_SENSITIVITY   0x42
#define CAP1188_STANDBY_THRESHOLD     0x43
#define CAP1188_CONFIGURATION_2       0x44

// Base Count Registers
#define CAP1188_SENSOR_INPUT_1_BASE   0x50
#define CAP1188_SENSOR_INPUT_2_BASE   0x51
#define CAP1188_SENSOR_INPUT_3_BASE   0x52
#define CAP1188_SENSOR_INPUT_4_BASE   0x53
#define CAP1188_SENSOR_INPUT_5_BASE   0x54
#define CAP1188_SENSOR_INPUT_6_BASE   0x55
#define CAP1188_SENSOR_INPUT_7_BASE   0x56
#define CAP1188_SENSOR_INPUT_8_BASE   0x57

// LED Controls
#define CAP1188_LED_OUTPUT_TYPE       0x71
#define CAP1188_SENSOR_INPUT_LED_LINK 0x72
#define CAP1188_LED_POLARITY          0x73
#define CAP1188_LED_OUTPUT_CONTROL    0x74

// LED Behavior Registers
#define CAP1188_LED_BEHAVIOR_1        0x81
#define CAP1188_LED_BEHAVIOR_2        0x82
#define CAP1188_LED_PULSE_1_PERIOD    0x84
#define CAP1188_LED_PULSE_2_PERIOD    0x85
#define CAP1188_LED_BREATHE_PERIOD    0x86
#define CAP1188_LED_CONFIG            0x88

// Identification Registers
#define CAP1188_PRODUCT_ID            0xFD
#define CAP1188_MANUFACTURER_ID       0xFE
#define CAP1188_REVISION              0xFF


/**
 * 
 * CAP1188 Bit Positions
 * 
 */
// Main Control Register (0x00)
#define CAP1188_MAIN_CONTROL_STBY        5
#define CAP1188_MAIN_CONTROL_DSLEEP      4
#define CAP1188_MAIN_CONTROL_INT         0
#define CAP1188_MAIN_CONTROL_GAIN_0      6
#define CAP1188_MAIN_CONTROL_GAIN_1      7


// General Status Register (0x02)
#define CAP1188_GENERAL_STATUS_LED       4
#define CAP1188_GENERAL_STATUS_RESET     3
#define CAP1188_GENERAL_STATUS_MULT      2
#define CAP1188_GENERAL_STATUS_MTP       1
#define CAP1188_GENERAL_STATUS_TOUCH     0

// Sensor Input Status Register (0x03)
#define CAP1188_SENSOR_STATUS_CS8        7
#define CAP1188_SENSOR_STATUS_CS7        6
#define CAP1188_SENSOR_STATUS_CS6        5
#define CAP1188_SENSOR_STATUS_CS5        4
#define CAP1188_SENSOR_STATUS_CS4        3
#define CAP1188_SENSOR_STATUS_CS3        2
#define CAP1188_SENSOR_STATUS_CS2        1
#define CAP1188_SENSOR_STATUS_CS1        0

// LED Status Register (0x04)
#define CAP1188_LED_STATUS_LED8_DN       7
#define CAP1188_LED_STATUS_LED7_DN       6
#define CAP1188_LED_STATUS_LED6_DN       5
#define CAP1188_LED_STATUS_LED5_DN       4
#define CAP1188_LED_STATUS_LED4_DN       3
#define CAP1188_LED_STATUS_LED3_DN       2
#define CAP1188_LED_STATUS_LED2_DN       1
#define CAP1188_LED_STATUS_LED1_DN       0

// Configuration Register (0x20)
#define CAP1188_CONFIG_TIMEOUT           7
#define CAP1188_CONFIG_WAKE              6
#define CAP1188_CONFIG_DIS_DIG_NOISE     5
#define CAP1188_CONFIG_DIS_ANA_NOISE     4
#define CAP1188_CONFIG_EN_MAX_DUR        3

// Interrupt Enable Register (0x27)
#define CAP1188_INTERRUPT_ENABLE_CS8     (1 << 7)
#define CAP1188_INTERRUPT_ENABLE_CS7     (1 << 6)
#define CAP1188_INTERRUPT_ENABLE_CS6     (1 << 5)
#define CAP1188_INTERRUPT_ENABLE_CS5     (1 << 4)
#define CAP1188_INTERRUPT_ENABLE_CS4     (1 << 3)
#define CAP1188_INTERRUPT_ENABLE_CS3     (1 << 2)
#define CAP1188_INTERRUPT_ENABLE_CS2     (1 << 1)
#define CAP1188_INTERRUPT_ENABLE_CS1     (1 << 0)
