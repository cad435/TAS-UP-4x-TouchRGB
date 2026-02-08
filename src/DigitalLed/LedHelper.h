#pragma once
#include <FastLed.h>
#include "TAS_Hardware.h"


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



