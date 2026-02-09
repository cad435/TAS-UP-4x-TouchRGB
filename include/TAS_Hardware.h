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
    #define KNX_UART_NUM (0)


    #define DEVICE_ID "TAS_UP_4X_TouchRGB"
    #define DEVICE_NAME "OpenKNX Touch Display RGB"

    //External I2C sensor, can be used by Sensor-Module
    #define SENSOR_SCL_PIN (21)
    #define SENSOR_SDA_PIN (20)
    //Spare I2C Bus, possibly displays in the future

    #define I2C2_SCL_PIN (23)
    #define I2C2_SDA_PIN (22)

    //CAP1188 Touch Sensor
    #define CAP1188_RST_PIN (3)
    #define CAP1188_ISR_PIN (19)
    #define CAP1188_I2C_SDA ((pin_size_t)16)
    #define CAP1188_I2C_SCL ((pin_size_t)17)
    #define CAP1188_I2C_WIRECLASS i2c0_inst

    #define CAP1188_TOUCH_THRESHOLD (100) //Touch Threshold, reported values from chip range from -127 to 127. The input value from chip wil be evaluated as abs()

    //Touch-Pins e.g. "LED"-Pins from CAP1188 to RP2040 (Not used, everything is handled via I2C)
    #define TOUCH_PIN_TOP_LEFT (27) //Top Left
    #define TOUCH_PIN_TOP_RIGHT (26) //Top Right
    #define TOUCH_PIN_BOTTOM_LEFT (28) //Bottom Left
    #define TOUCH_PIN_BOTTOM_RIGHT (25) //Bottom Right


    #define CAP1188_MTP_CHANNELS {CAP1188::Pad_A, CAP1188::Pad_B, CAP1188::Pad_C, CAP1188::Pad_D} //Channels that are used for the Tap-Functionality. The channels are defined in the CAP1188 class.
    #define CAP1188_MTP_CHANNELS_COUNT (4) //Number of channels that are used for the Tap-Functionality. The channels are defined in the CAP1188 class.

    #define CAP1188_PROX_CHANNELS {CAP1188::Pad_A, CAP1188::Pad_B, CAP1188::Pad_C, CAP1188::Pad_D} //Channels that are used for the Proximity-Detection. The channels are defined in the CAP1188 class.
    #define CAP1188_PROX_CHANNELS_COUNT (4) //Number of channels that are used for the Proximity-Detection. The channels are defined in the CAP1188 class.

    #define CAP1188_EVALUATE_FREQUENCY (10) //times per second for the CAP1188 evaluation.


    #define TTD_LED_FPS (50) //Frames per second for the LED's.
    #define TTD_LED_TRANSITION_TIME (1000) //Time in ms for the LED's to transition from one color to another.
    #define TTD_LED_TRANSITION_DELTA (255 / (TTD_LED_TRANSITION_TIME / TTD_LED_FPS)) //Time in ms for the LED's to transition from one color to another. This defines one "step" where inbetween the led's are only changing a little bit


    //WS2812 RGB LED
    #define TTD_RGB_DIO_PIN (18)
    #define TTD_LED_TYPE SK6812
    #define TTD_BASE_COLORORDER RGB //RGB Order for the LED's, can be changed to GRB or BRG if needed.


    //!!! LED-Groups MUST be added in the same order as the physical LED chain is built !!!


    #define TTD_LEDGROUP_COUNT (3) //Number of LED-Groups, used for the LED-Controller. This is used to group the LED's together, so that we can control them in groups.
    //LED-Group Sizes of the daisichained LED's. This must reflect the Hardware! This array must be the same size as TTD_LED_GROUP_COUNT.
    #define TTD_LEDGROUP_SIZE {1, 40, 1}
    #define TTD_LEDGROUP_FUNCTIONS {LedGroupFunction::FLAT, LedGroupFunction::SINE, LedGroupFunction::FLAT} //Functions for each LED-Group, must be the same size as TTD_LED_GROUP_COUNT
    #define TTD_LEDGROUP_COLORORDER {RGB, GRB, RGB} //Color order for each LED-Group, must be the same size as TTD_LED_GROUP_COUNT. This is used to set the color order for each group, in case they are different. The color order is defined in the FastLED library, e.g. RGB, GRB, BRG, etc.

    


#endif