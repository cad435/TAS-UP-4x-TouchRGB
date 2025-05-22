#pragma once
#include <FastLed.h>
#include "hardware.h"

class SerialLed
{

    enum TDD_LED_STATE
    {
      STATIC, //LED's are static, no animation
      TODO, //LED's are sheduled for a change, but not yet changed
      RUNNING //lED's are running, animation is running
    };

public:
    SerialLed(/* args */);

    void begin(bool hasLCD);

    void evaluate(); //call it as fast as you like, it will only run every 20ms

    //take base color, base brightness and set the individual LED's depending if LCD's are here or not
    //will NOT initialize a LED-Transition
    //LED-Transition will be started with "dimmUp()" or "dimmDown()"
    void setLEDTargetColor(CRGB _BaseColor);
    //switches between the "bright" and "dark" state of the LED's
    //Initializes a LED-Transition
    void DimmUp();
    void DimmDown();

    CRGB BaseColor = CRGB(0, 0, 0); // Base color for the LEDs
    uint8_t BaseBrightness_ON = 255; // Base brightness for the LEDs
    uint8_t BaseBrightness_OFF = 0; // Base brightness for the LEDs

private:
    // Define the array of leds

    CRGB leds_Original[RGB_LED_COUNT];
    CRGB leds_Target[RGB_LED_COUNT];
    CRGB leds_Current[RGB_LED_COUNT]; // Current color of the LEDs

    uint16_t fadingAmount = 0; // Fading amount for the LEDs


    TDD_LED_STATE ledState = STATIC; // State of the LEDs



    uint8_t Brigtnessdivider[RGB_LED_COUNT]; //Brightness divider for the LEDs, depending on where they are located


    //Fixed FPS LED loop, used for the LED's
    uint32_t LEDFPSTime_ms = 1000 / TTD_LED_FPS; //Time in ms for the LED FPS
    uint32_t lastTimeLEDRun = 0; //Last time the LED was run

    //LED loop, used for the LED's
    //runs with a fixed FPS, defined in TTD_LED_FPS
    //this will be called every 20ms, so it is not blocking the main loop
    void FixedFPSLedLoop(); 

    //helper for mapping hardware.h to the TDD_Module
    uint8_t RGB_LED_DIVIDER_NOLCD[RGB_LED_COUNT] = _RGB_LED_DIVIDER_NOLCD;
    uint8_t RGB_LED_DIVIDER_HASLCD[RGB_LED_COUNT] = _RGB_LED_DIVIDER_HASLCD;
};


class LEDHelper
{

public:
    LEDHelper(){}
    //Helper functions
    CRGB rgb565ToCRGB(uint16_t color);
    uint16_t CRGBtorgb565(CRGB col);
    CRGB DPT_Colour_RGB_to_CRGB(uint32_t ko);
private:
    /* data */
};



