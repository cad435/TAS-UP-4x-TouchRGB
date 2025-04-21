

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
    #define CAP1188_I2C_ADDR (0x29)
    #define CAP1188_I2C Wire1


    //WS2812 RGB LED
    #define RGB_DIO_PIN (18)

    //Displays
    #define DISPLAY_PWR_EN_PIN (12)   
    //Right Separator, if used with display, is connected to Wire 1, address 0x3C
    #define SEPARATOR_RIGHT_I2C_ADDR (0x3C)
    #define SEPARATOR_RIGHT_I2C Wire1


#endif