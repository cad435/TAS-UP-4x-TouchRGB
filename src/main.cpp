#include "HardwareConfig.h"
#include "OpenKNX.h"
#include "Logic.h"
#include "THPSensorModule.h"
#include "VirtualButtonModule.h"
#include "FileTransferModule.h"
#include "TDD_Module/TDD_Module.h"


const uint8_t THPSensorGpioPins[THP_ChannelCount*2] = {SENSOR_SCL_PIN, SENSOR_SDA_PIN};
THPSensorModule  thpsensormodule = THPSensorModule(THPSensorGpioPins);

bool setup_done = false;
void setup()
{
    delay(5000);
    const uint8_t firmwareRevision = 1;
    openknx.init(firmwareRevision);

    openknx.addModule(3, openknxVirtualButtonModule);
    openknx.addModule(2, thpsensormodule);
    openknx.addModule(1, openknxLogic);
    openknx.addModule(9, openknxFileTransferModule);
    openknx.addModule(43, openknxTDD_Module);
    openknx.setup();
    openknx.setup1(); // Call setup1() for the modules
    setup_done = true;
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
        thpsensormodule.loop1();
    }
}