#pragma once

#include "hardware.h"
#include "KnxHelper.h"
#include "OpenKNX.h"
#include "OpenKNX/TimerInterrupt.h"
#include "../CAP1188/CAP1188.h"
#include <FastLed.h>
#include "../DigitalLed/LEDHelper.h"
#include "../DigitalLed/LedGroupController.h"

class TDD_Module : public OpenKNX::Module
{

  enum TDD_LED_STATE
  {
    STATIC, //LED's are static, no animation
    TODO, //LED's are sheduled for a change, but not yet changed
    RUNNING //lED's are running, animation is running
  };

  public:

    //constructor
    TDD_Module();

    void loop() override;
    void loop1() override;
    void setup() override;
    void setup1() override;
    void processAfterStartupDelay() override;
    void processInputKo(GroupObject& ko) override;

    const std::string name() override;
    const std::string version() override;
    // void writeFlash() override;
    // void readFlash(const uint8_t* iBuffer, const uint16_t iSize) override;
    // uint16_t flashSize() override;

  private:

    bool setupComplete = false; // Flag to check if setup is complete. Used for core1

    CAP1188* cap;
    LedGroupController* LedController; //Pointer to LedGroupController class

    LEDHelper* ledHelper; // Pointer to the LEDHelper class



    uint8_t PercentToUint8(uint8_t percent){ return (uint8_t)round(((uint16_t)percent * (uint16_t)255) / 100.0f); }

    //takes a KNX string Parameter and spits out a number
    int32_t StringParam2Num(uint8_t* ParamData);

};

extern TDD_Module openknxTDD_Module;