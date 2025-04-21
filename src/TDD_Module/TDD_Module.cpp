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

}

void TDD_Module::loop()
{
   
}

void TDD_Module::processInputKo(GroupObject& iKo)
{

}

void TDD_Module::processAfterStartupDelay()
{

}

TDD_Module openknxTDD_Module;