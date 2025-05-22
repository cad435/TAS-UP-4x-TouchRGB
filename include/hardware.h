#pragma once
#include <Arduino.h>

#ifdef DEVICE_SEN_UP1_8XTH
    #include "HardwareConfig/OpenKNX/UP1.h"
#endif

#ifdef DEVICE_TAS_UP_4X_TouchRGB

    #define PROG_LED_PIN (6)
    #define PROG_LED_PIN_ACTIVE_ON HIGH
    #define PROG_BUTTON_PIN (7)
    #define PROG_BUTTON_PIN_INTERRUPT_ON FALLING
    #define SAVE_INTERRUPT_PIN (5)
    #define KNX_SERIAL Serial1
    #define KNX_UART_RX_PIN (1)
    #define KNX_UART_TX_PIN (0)

    #define DEVICE_ID "TAS_UP_4X_TouchRGB"
    #define DEVICE_NAME "OpenKNX Touch Display RGB"

    //External I2C sensor, can be used by Sensor-Module
    #define SENSOR_SCL_PIN (21)
    #define SENSOR_SDA_PIN (20)
    //i2C Busses for CAP1188 and Displays
    #define I2C1_SCL_PIN (16)
    #define I2C1_SDA_PIN (15)
    #define I2C2_SCL_PIN (23)
    #define I2C2_SDA_PIN (22)

    //CAP1188 Touch Sensor
    #define CAP1188_RST_PIN (3)
    #define CAP1188_ISR_PIN (19)
    #define CAP1188_I2C_SDA ((pin_size_t)16)
    #define CAP1188_I2C_SCL ((pin_size_t)17)
    #define CAP1188_I2C_WIRECLASS i2c0_inst

    #define CAP1188_TOUCH_THRESHOLD (100) //Touch Threshold, reported values from chip range from -127 to 127. The input value from chip wil be evaluated as abs()

    //Touch-Pins e.g. "LED"-Pins from CAP1188 to RP2040
    #define TOUCH_PIN_TOP_LEFT (27) //Top Left
    #define TOUCH_PIN_TOP_RIGHT (26) //Top Right
    #define TOUCH_PIN_BOTTOM_LEFT (28) //Bottom Left
    #define TOUCH_PIN_BOTTOM_RIGHT (25) //Bottom Right


    #define TTD_LED_FPS (50) //Frames per second for the LED's.
    #define TTD_LED_TRANSITION_TIME (1000) //Time in ms for the LED's to transition from one color to another.
    #define TTD_LED_TRANSITION_DELTA (255 / (TTD_LED_TRANSITION_TIME / TTD_LED_FPS)) //Time in ms for the LED's to transition from one color to another.

    //WS2812 RGB LED
    #define RGB_DIO_PIN (18)
    #define RGB_LED_COUNT (14)  //Number of LEDs in the strip. For simplicity this stays always the same even if LCD's are here. 
                                //If LCD's are present, than the last bits are just not used.
    #define RGB_LED_TYPE SK6812
    //length of that array must always be the same as RGB_LED_COUNT, otherwise it will crash
    //is used so that we can dimm the LED's gradually down at the end of the LED-Band.
    //#define _RGB_LED_DIVIDER_NOLCD {1, 1, 1, 1, 4, 2, 1, 1, 1, 2, 1, 1, 1, 4}; //LED Brightness will be dimmed bown, depending on where they are located. More divider = less brightness.
    //#define _RGB_LED_DIVIDER_HASLCD {4, 2, 1, 1, 1, 2, 4, 1, 1, 1, 1, 1, 1, 1}; //If LCD's are present, the divider is different as not all LEDs are used.

    #define _RGB_LED_DIVIDER_NOLCD {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}; //LED Brightness will be dimmed bown, depending on where they are located. More divider = less brightness.
    #define _RGB_LED_DIVIDER_HASLCD {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    //Displays
    #define DISPLAY_PWR_EN_PIN (12)   
    //Right Separator, if used with display, is connected to Wire 1, address 0x3C
    #define SEPARATOR_RIGHT_I2C_ADDR (0x3C)
    #define SEPARATOR_RIGHT_I2C Wire1

    


#endif