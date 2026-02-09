#include "Logic.h"
#include "OpenKNX.h"
#include "FileTransferModule.h"
#include "THPSensorModule.h"
#include "VirtualButtonModule.h"
#include "TDD_Module/TDD_Module.h"


const uint8_t THPSensorGpioPins[THP_ChannelCount*2] = {SENSOR_SCL_PIN, SENSOR_SDA_PIN};
THPSensorModule  thpsensormodule = THPSensorModule(THPSensorGpioPins);

void setup()
{
   
    // change this also in library.json
    openknx.init();
    openknx.addModule(1, openknxLogic);
    openknx.addModule(2, thpsensormodule);
    openknx.addModule(3, openknxVirtualButtonModule);
    openknx.addModule(9, openknxFileTransferModule);
    openknx.addModule(43, openknxTDD_Module);
   
    openknx.setup();
}

void loop()
{
    openknx.loop();
}

#ifdef OPENKNX_DUALCORE
void setup1()
{
    openknx.setup1(); // Call setup1() for the modules

}

void loop1()
{
    openknx.loop1(); // Call loop1() for the modules
}
#endif




















//#include "HardwareConfig.h"
/*#include <hardware.h>
#include "OpenKNX.h"
#include "Logic.h"
#include "THPSensorModule.h"
#include "VirtualButtonModule.h"
#include "FileTransferModule.h"
#include "TDD_Module/TDD_Module.h"


const uint8_t THPSensorGpioPins[THP_ChannelCount*2] = {SENSOR_SCL_PIN, SENSOR_SDA_PIN};
THPSensorModule  thpsensormodule = THPSensorModule(THPSensorGpioPins);



bool setup_done = false;
bool init_done = false;
void setup()
{
    delay(5000);

    Serial.println("Starting OpenKNX on TAS-UP-4x-TouchRGB");

    delay(1000);

    //const uint8_t firmwareRevision = 1;
    Serial.println("Initializing OpenKNX");
    openknx.init();

    Serial.println("Adding Modules");

    //openknx.addModule(3, openknxVirtualButtonModule);
    //openknx.addModule(2, thpsensormodule);
    //openknx.addModule(1, openknxLogic);
    //openknx.addModule(9, openknxFileTransferModule);
    //openknx.addModule(43, openknxTDD_Module);
    //init_done = true;

    //openknx.setup(); // Call setup() for the modules
    //setup_done = true;

}

void setup1()
{
    while(!init_done)
        delay(100);
    openknx.setup1(); // Call setup1() for the modules
}

void loop()
{
    openknx.loop();
}

void loop1()
{
    while(!setup_done)
        delay(100);
    while(true)
    {
        //openknx.loop1(); // Call loop1() for the modules <-- if we're doing this, somehow the CAP1188 is not behaving exactly the same as when running in singlecore-mode
        thpsensormodule.loop1();
        openknxTDD_Module.loop1(); // Call loop1() for the TDD_Module
    }
}*/