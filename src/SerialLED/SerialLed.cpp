#include "SerialLed.h"


SerialLed::SerialLed(/* args */)
{}

void SerialLed::begin(bool hasLCD)
{
    FastLED.addLeds<RGB_LED_TYPE, RGB_DIO_PIN, RGB>(leds_Current, RGB_LED_COUNT);
    FastLED.clearData();


    if (hasLCD)
        memcpy(Brigtnessdivider, RGB_LED_DIVIDER_HASLCD, RGB_LED_COUNT); //If the LCD is present, use the divider for the LCD
    else
        memcpy(Brigtnessdivider, RGB_LED_DIVIDER_NOLCD, RGB_LED_COUNT); //If the LCD is not present, use the divider for the non-LCD
}

void SerialLed::evaluate()
{
    if (millis() - lastTimeLEDRun >= LEDFPSTime_ms) //If the time is up, run the LED's
    {
        FixedFPSLedLoop(); //Run the LED's
        lastTimeLEDRun = millis(); //Set the last time the LED was run to the current time
    }
}


//runs with a fixed FPS, defined in TTD_LED_FPS
void SerialLed::FixedFPSLedLoop()
{
    switch (ledState) //Switch the LED state
    {
        case STATIC: //If the LED's are static, do nothing
            break;
        case TODO: //If the LED's are sheduled for a change, prepare everything
            memcpy(leds_Original, leds_Current, RGB_LED_COUNT); //Copy the current LED's to the original LED's
            fadingAmount = 0; //Set the fading amount to 0
            ledState = RUNNING; //Set the LED state to running
            break;
        case RUNNING: //If the LED's are running, run them
            if (fadingAmount > 255)
                fadingAmount = 255; //If the fading amount is greater than 255, set it to 255

            blend( leds_Original, leds_Target, leds_Current, RGB_LED_COUNT,  (fract8)fadingAmount); //Blend the LED's to the target color
            fadingAmount += TTD_LED_TRANSITION_DELTA; //Increase the fading amount
            if (fadingAmount >= 255) //If the fading amount is greater than or equal to 255, set the LED's to the target color
            {
                memcpy(leds_Current, leds_Target, RGB_LED_COUNT); //Copy the target LED's to the current LED's
                ledState = STATIC; //Set the LED state to static
            }
            FastLED.show(); //Show the LED's
            break;   
        default:
            break;
    }
}

void SerialLed::DimmUp()
{
    for (uint8_t i = 0; i < RGB_LED_COUNT; i++)
    {
        leds_Target[i].nscale8_video(BaseBrightness_ON);
    }
    ledState = TODO; //Set the LED state to TODO
}
void SerialLed::DimmDown()
{
    for (uint8_t i = 0; i < RGB_LED_COUNT; i++)
    {
        leds_Target[i].nscale8_video(BaseBrightness_OFF);
    }
    ledState = TODO; //Set the LED state to TODO
}


void SerialLed::setLEDTargetColor(CRGB _BaseColor)
{
    BaseColor = _BaseColor;
    for (int i = 0; i < RGB_LED_COUNT; i++)
    {
        leds_Target[i] = CRGB(BaseColor.r / Brigtnessdivider[i], BaseColor.g / Brigtnessdivider[i], BaseColor.b / Brigtnessdivider[i]); //Set the target color for the LED's

    }
}


CRGB LEDHelper::rgb565ToCRGB(uint16_t color)
{
    uint8_t r, g, b;
    r = (color >> 11) & 0x1F;
    g = (color >> 5) & 0x3F;
    b = color & 0x1F;

    CRGB col = CRGB(r << 3, g << 2, b << 3); // Convert to 8-bit RGB
    return col;
}

uint16_t LEDHelper::CRGBtorgb565(CRGB col)
{
    return ((col.r & 0xF8) << 8) | ((col.g & 0xFC) << 3) | (col.b >> 3);
}

CRGB LEDHelper::DPT_Colour_RGB_to_CRGB(uint32_t ko)
{
    CRGB col = CRGB(0, 0, 0); //Set the color to black

    uint8_t* p = (uint8_t*)(void*)&ko; // yeah, this is beyond ugly, but it works neatly
    col.r = p[0]; //Get the red value from the ko
    col.g = p[1]; //Get the green value from the ko
    col.b = p[2]; //Get the blue value from the ko
    return col; //Return the color

}
