#include "TDD_Module.h"
#include "ModuleVersionCheck.h"


const std::string TDD_Module::name()
{
    return "TDD_Module";
}

const std::string TDD_Module::version()
{
    return MODULE_VirtualButton_Version;
}

void TDD_Module::setup()
{
    logDebugP("Setup");

    /**  Check if LCD Present **/
    hasLCD = ParamTTD_HasDisplay; //If the LCD is present, set the flag to true   

    logDebugP("Has LCD: %i", hasLCD); //Print the string to the debug output


#pragma region CAP1188-Initialisation

    /**  Initialisation of CAP1188 **/
    cap = new CAP1188(CAP1188_RST_PIN);
    

    pinMode(TOUCH_PIN_TOP_LEFT, INPUT_PULLUP);
    pinMode(TOUCH_PIN_TOP_RIGHT, INPUT_PULLUP);
    pinMode(TOUCH_PIN_BOTTOM_LEFT, INPUT_PULLUP);
    pinMode(TOUCH_PIN_BOTTOM_RIGHT, INPUT_PULLUP);

    if (cap->begin(CAP1188_I2CADDR, new TwoWire(&CAP1188_I2C_WIRECLASS, CAP1188_I2C_SDA, CAP1188_I2C_SCL)))
    logDebugP("CAP1188 begin success");
    else
    {
        logDebugP("CAP1188 begin failed");
        return;
    }

    uint8_t GlobalSen = ParamTTD_CAP_Sensitivity; //Get the sensitivity from the parameter
    cap->SetGlobalSensitivity(GlobalSen); //Set the sensitivity of the CAP1188
    logDebugP("SetGlobalSensitivity: %i", GlobalSen); //Print the sensitivity to the debug output


    
    bool disableAnalogFilter = ParamTTD_CAP_EnableAnalogFilter; //Get the analog filter from the parameter
    cap->disableAnalogNoiseFilter(!disableAnalogFilter); //Enable the analog filter
    logDebugP("disableAnalogNoiseFilter: !%i", disableAnalogFilter); //Print the analog filter to the debug output


    //read the proximity threshold from the parameter and set it to the CAP1188
    uint16_t proximityThreshold = StringParam2Num(ParamTTD_TTDTextProximityThreshold);
    if (proximityThreshold > 1016) //If the proximity threshold is greater than 127, set it to 127
        proximityThreshold = 1016;
    cap->setProximityThreshold(proximityThreshold); //Set the proximity threshold
    logDebugP("proximityThreshold: %i", proximityThreshold); //Print the string to the debug output


    uint8_t touchThreshold = StringParam2Num(ParamTTD_TTDTextTouchThreshold); //Convert the string to int
    if (touchThreshold > 127) //If the touch threshold is greater than 127, set it to 127
        touchThreshold = 127;
    for (int i = 0; i < 8; i++)
        cap->setTouchThreshold(i, touchThreshold); //Set the touch threshold for each pin

    logDebugP("TouchThreshold: %i", touchThreshold); //Print the string to the debug output



#pragma endregion CAP1188-Initialisation

#pragma region LED-Initialisation

    /**  Initialisation of LED's **/
    logDebugP("LED-Initialisation"); //Print the string to the debug output

    FastLED.addLeds<RGB_LED_TYPE, RGB_DIO_PIN, RGB>(leds_Current, RGB_LED_COUNT);

    FastLED.clearData();

    BaseBrightness_ON = PercentToUint8(ParamTTD_LEDMaxBrightness_ON); //Get the brightness from the parameter
    logDebugP("BaseBrightness_ON: %i", BaseBrightness_ON); //Print the brightness to the debug output
    BaseBrightness_OFF = PercentToUint8(ParamTTD_LEDMaxBrightness_OFF); //Get the brightness from the parameter
    logDebugP("BaseBrightness_OFF: %i", BaseBrightness_OFF); //Print the brightness to the debug output
    
    if (hasLCD)
        memcpy(Brigtnessdivider, RGB_LED_DIVIDER_HASLCD, RGB_LED_COUNT); //If the LCD is present, use the divider for the LCD
    else
        memcpy(Brigtnessdivider, RGB_LED_DIVIDER_NOLCD, RGB_LED_COUNT); //If the LCD is not present, use the divider for the non-LCD

    CRGB col = rgb565ToCRGB(ParamTTD_LEDColor); //Convert the color from the parameter to CRGB
    logDebugP("LEDColor: %i|%i|%i", col.r, col.g, col.b); //Print the color to the debug output
    setLEDTargetColor(col); ////Get the color from the parameter andstart the LED-Transition

#pragma endregion LED-Initialisation

    setupComplete = true; // Set the setup complete flag to true

}

void TDD_Module::readFlash(const uint8_t *iBuffer, const uint16_t iSize)
{
    if (iSize == 0) // first call - without data
        return;

    logDebugP("readFlash with Size: %i", iSize); //Print the string to the debug output

}

void TDD_Module::setup1()
{

    while(!setupComplete); // wait till core0 has completed the setup

    //alarm_pool_add_repeating_timer_us(alarmPool1, LEDFPSTime_ms * 1000, ((repeating_timer *rt))FixedFPSCallback, nullptr, &fixedLEDTimer);//would be nice, but not working, so ditch it 

    lastTimeLEDRun = millis(); //Set the last time the LED was run to the current time

    //this will than proceed to loop1();
}

void TDD_Module::loop()
{
    //cap evaluate will only execute once 100ms, so we can fire here as fast as we can.
    //cap->evaluate(); //Evaluate the CAP1188
}

void TDD_Module::loop1()
{
    if (millis() - lastTimeLEDRun >= LEDFPSTime_ms) //If the time is up, run the LED's
    {
        FixedFPSLedLoop(); //Run the LED's
        lastTimeLEDRun = millis(); //Set the last time the LED was run to the current time
    }
}

//runs with a fixed FPS, defined in TTD_LED_FPS
void TDD_Module::FixedFPSLedLoop()
{
    switch (ledState) //Switch the LED state
    {
        case STATIC: //If the LED's are static, do nothing
            break;
        case TODO: //If the LED's are sheduled for a change, prepare everything
            memcpy(leds_Original, leds_Current, RGB_LED_COUNT); //Copy the current LED's to the original LED's
            fadingAmount = 0; //Set the fading amount to 0
            ledState = RUNNING; //Set the LED state to running
            logDebugP("LED-Transition started"); //Print the string to the debug output
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

void TDD_Module::processInputKo(GroupObject& iKo)
{
    switch(iKo.asap())
    {
    case TTD_KoLEDColor:
        setLEDTargetColor(DPT_Colour_RGB_to_CRGB(KoTTD_LEDColor.value(DPT_Colour_RGB)));
        break;
    case TTD_KoLEDBrightness_ON:
        BaseBrightness_ON = KoTTD_LEDBrightness_ON.value(DPT_Scaling); //Set the brightness of the LED's to the value from the parameter
        break;
    case TTD_KoLEDBrightness_OFF:
        BaseBrightness_OFF = KoTTD_LEDBrightness_OFF.value(DPT_Scaling); //Set the brightness of the LED's to the value from the parameter
        break;

    
    default:
        break;
    }
}

void TDD_Module::processAfterStartupDelay()
{
    logDebugP("processAfterStartupDelay");
    ledState = TODO; //Set the LED state to TODO
}

void TDD_Module::setLEDTargetColor(CRGB _BaseColor)
{
    BaseColor = _BaseColor;
    for (int i = 0; i < RGB_LED_COUNT; i++)
    {
        leds_Target[i] = CRGB(BaseColor.r / Brigtnessdivider[i], BaseColor.g / Brigtnessdivider[i], BaseColor.b / Brigtnessdivider[i]); //Set the target color for the LED's
    }
    ledState = TODO; //Set the LED state to TODO
}

CRGB TDD_Module::rgb565ToCRGB(uint16_t color)
{
    uint8_t r, g, b;
    r = (color >> 11) & 0x1F;
    g = (color >> 5) & 0x3F;
    b = color & 0x1F;

    CRGB col = CRGB(r << 3, g << 2, b << 3); // Convert to 8-bit RGB
    return col;
}

uint16_t TDD_Module::CRGBtorgb565(CRGB col)
{
    return ((col.r & 0xF8) << 8) | ((col.g & 0xFC) << 3) | (col.b >> 3);
}

CRGB TDD_Module::DPT_Colour_RGB_to_CRGB(uint32_t ko)
{
    CRGB col = CRGB(0, 0, 0); //Set the color to black

    uint8_t* p = (uint8_t*)(void*)&ko; // yeah, this is beyond ugly, but it works neatly
    col.r = p[0]; //Get the red value from the ko
    col.g = p[1]; //Get the green value from the ko
    col.b = p[2]; //Get the blue value from the ko
    return col; //Return the color

}


int32_t TDD_Module::StringParam2Num(uint8_t* ParamData)
{
    //read the proximity threshold from the parameter and set it to the CAP1188
    String str = "";
    for (uint8_t i = 0; i < 32; i++) //safety-Guard, max 32 chars
    {
        if ((char)ParamData[i] == '\0') //If the character is null, break the loop
            break;
            str += (char)ParamData[i]; //Copy the string to the string variable

        if (i==31)
            return -1; //If the string is too long, return -1
    }   

    return str.toInt(); //Convert the string to int
}


TDD_Module openknxTDD_Module;