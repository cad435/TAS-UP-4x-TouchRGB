#pragma once

#include "hardware.h"
#include "KnxHelper.h"
#include "OpenKNX.h"
#include "OpenKNX/TimerInterrupt.h"
#include "../CAP1188/CAP1188.h"
#include <FastLed.h>

class TDD_Module : public OpenKNX::Module
{

  enum TDD_LED_STATE
  {
    STATIC, //LED's are static, no animation
    TODO, //LED's are sheduled for a change, but not yet changed
    RUNNING //lED's are running, animation is running
  };

  public:
    void loop() override;
    void loop1() override;
    void setup() override;
    void setup1() override;
    void readFlash(const uint8_t *iBuffer, const uint16_t iSize) override;
    void processAfterStartupDelay() override;
    void processInputKo(GroupObject& ko) override;

    const std::string name() override;
    const std::string version() override;
    // void writeFlash() override;
    // void readFlash(const uint8_t* data, const uint16_t size) override;
    // uint16_t flashSize() override;

  private:

    bool setupComplete = false; // Flag to check if setup is complete. Used for core1
    
    bool hasLCD = false; // Flag to check if the LCD is connected

    CAP1188* cap;


    // Define the array of leds
    CRGB leds_Original[RGB_LED_COUNT];
    CRGB leds_Target[RGB_LED_COUNT];
    CRGB leds_Current[RGB_LED_COUNT]; // Current color of the LEDs

    uint16_t fadingAmount = 0; // Fading amount for the LEDs


    TDD_LED_STATE ledState = STATIC; // State of the LEDs

    CRGB BaseColor = CRGB(0, 0, 0); // Base color for the LEDs
    uint8_t BaseBrightness_ON = 255; // Base brightness for the LEDs
    uint8_t BaseBrightness_OFF = 0; // Base brightness for the LEDs

    uint8_t Brigtnessdivider[RGB_LED_COUNT]; //Brightness divider for the LEDs, depending on where they are located


    //Fixed FPS LED loop, used for the LED's
    uint32_t LEDFPSTime_ms = 1000 / TTD_LED_FPS; //Time in ms for the LED FPS
    uint32_t lastTimeLEDRun = 0; //Last time the LED was run


    //take base color, base brightness and set the individual LED's depending if LCD's are here or not
    //will initialize a LED-Transition
    void setLEDTargetColor(CRGB _BaseColor);


    void FixedFPSLedLoop(); 

    //Helper functions
    CRGB rgb565ToCRGB(uint16_t color);
    uint16_t CRGBtorgb565(CRGB col);

    uint8_t PercentToUint8(uint8_t percent){ return ((uint16_t)percent * 255) / 100; }

    CRGB DPT_Colour_RGB_to_CRGB(uint32_t ko);


    //takes a KNX string Parameter and spits out a number
    int32_t StringParam2Num(uint8_t* ParamData);




    //helper for mapping hardware.h to the TDD_Module
    uint8_t RGB_LED_DIVIDER_NOLCD[RGB_LED_COUNT] = _RGB_LED_DIVIDER_NOLCD;
    uint8_t RGB_LED_DIVIDER_HASLCD[RGB_LED_COUNT] = _RGB_LED_DIVIDER_HASLCD;
};

extern TDD_Module openknxTDD_Module;