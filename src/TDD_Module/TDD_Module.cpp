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
    logDebugP("Setup, CPU: %i", get_core_num()); 
    

    /**  Check if LCD Present **/
    hasLCD = ParamTTD_HasDisplay; //If the LCD is present, set the flag to true   

    logDebugP("Has LCD: %i", hasLCD); //Print the string to the debug output


#pragma region CAP1188-Initialisation

    /**  Initialisation of CAP1188 **/
    cap = new CAP1188(CAP1188_RST_PIN);

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
    cap->setProximityThreshold(ParamTTD_TTDProximityThreshold); //Set the proximity threshold
    logDebugP("proximityThreshold: %i", ParamTTD_TTDProximityThreshold); //Print the string to the debug output


    uint8_t touchThreshold = ParamTTD_TTDTouchThreshold;
    for (int i = 0; i < 8; i++)
        cap->setTouchThreshold(i, touchThreshold); //Set the touch threshold for each pin

    logDebugP("TouchThreshold: %i", touchThreshold); //Print the string to the debug output



#pragma endregion CAP1188-Initialisation

#pragma region LED-Initialisation

    /**  Initialisation of LED's **/
    logDebugP("LED-Initialisation"); //Print the string to the debug output

    serialLed = new SerialLed(); //Create a new instance of the SerialLed class
    serialLed->begin(hasLCD); //Begin the SerialLed class
    ledHelper = new LEDHelper(); //Create a new instance of the LEDHelper class



    serialLed->BaseBrightness_ON = PercentToUint8(ParamTTD_LEDMaxBrightness_ON); //Get the brightness from the parameter
    logDebugP("BaseBrightness_ON: %i", serialLed->BaseBrightness_ON); //Print the brightness to the debug output
    serialLed->BaseBrightness_OFF = PercentToUint8(ParamTTD_LEDMaxBrightness_OFF); //Get the brightness from the parameter
    logDebugP("BaseBrightness_OFF: %i", serialLed->BaseBrightness_OFF); //Print the brightness to the debug output
    

    CRGB col = ledHelper->rgb565ToCRGB(ParamTTD_LEDColor); //Convert the color from the parameter to CRGB
    logDebugP("LEDColor: %i|%i|%i", col.r, col.g, col.b); //Print the color to the debug output
    serialLed->setLEDTargetColor(col); ////Get the color from the parameter andstart the LED-Transition

#pragma endregion LED-Initialisation

    setupComplete = true; // Set the setup complete flag to true
    logDebugP("Setup Done");

}

void TDD_Module::processAfterStartupDelay()
{
    logDebugP("processAfterStartupDelay");
    serialLed->DimmDown(); //turn on the LED's to the "dark" state
    delay(1000); //Wait for 1 second
}

void TDD_Module::setup1()
{
    logDebugP("Setup1, CPU: %i", get_core_num()); 
    while(!setupComplete); // wait till core0 has completed the setup

    //this will than proceed to loop1();
}

void TDD_Module::loop()
{

    //fire here as fast as we can, will be executed when openKNX feels like.    
    if(openknx.freeLoopTime())
    {
        cap->evaluate(); //Evaluate the CAP1188
    }
    
    if(openknx.freeLoopTime() && cap->hasChanged(CAP1188::Pad_A)) //If the pad A has Changed, change the KO
    {
        KoTTD_StatePAD_A.value(cap->isTouched(CAP1188::Pad_A), Dpt(1,1)); //Send the touch value to the KO
        logDebugP("PAD_A Value %i to KO %i", cap->isTouched(CAP1188::Pad_A), KoTTD_StatePAD_A.asap()); //Print the touch value to the debug output
    }
            
    
    if(openknx.freeLoopTime() && cap->hasChanged(CAP1188::Pad_B)) //If the pad B has Changed, change the KO
    {
        KoTTD_StatePAD_B.value(cap->isTouched(CAP1188::Pad_B), Dpt(1,1)); //Send the touch value to the KO
        logDebugP("PAD_B Value %i to KO %i", cap->isTouched(CAP1188::Pad_B), KoTTD_StatePAD_B.asap()); //Print the touch value to the debug output
    }
            

    if(openknx.freeLoopTime() && cap->hasChanged(CAP1188::Pad_C)) //If the pad C has Changed, change the KO
    {
        KoTTD_StatePAD_C.value(cap->isTouched(CAP1188::Pad_C), Dpt(1,1)); //Send the touch value to the KO
        logDebugP("PAD_C Value %i to KO %i", cap->isTouched(CAP1188::Pad_C), KoTTD_StatePAD_C.asap()); //Print the touch value to the debug output
    }
                         
    if(openknx.freeLoopTime() && cap->hasChanged(CAP1188::Pad_D)) //If the pad D has Changed, change the KO
    {
        KoTTD_StatePAD_D.value(cap->isTouched(CAP1188::Pad_D), Dpt(1,1)); //Send the touch value to the KO
        logDebugP("PAD_D Value %i to KO %i", cap->isTouched(CAP1188::Pad_D), KoTTD_StatePAD_D.asap()); //Print the touch value to the debug output
    }                  

}

void TDD_Module::loop1()
{
    //call as fast as possible, will be only executed every 20ms
    serialLed->evaluate();
}

void TDD_Module::processInputKo(GroupObject& iKo)
{
    switch(iKo.asap())
    {
    case TTD_KoLEDColor:
        serialLed->setLEDTargetColor(ledHelper->DPT_Colour_RGB_to_CRGB(KoTTD_LEDColor.value(DPT_Colour_RGB)));
        break;
    case TTD_KoLEDBrightness_ON:
        serialLed->BaseBrightness_ON = KoTTD_LEDBrightness_ON.value(DPT_Scaling); //Set the brightness of the LED's to the value from the parameter
        break;
    case TTD_KoLEDBrightness_OFF:
        serialLed->BaseBrightness_OFF = KoTTD_LEDBrightness_OFF.value(DPT_Scaling); //Set the brightness of the LED's to the value from the parameter
        break;

    
    default:
        break;
    }
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