#pragma once

#include "Modules/ModuleManager.h"

class FRPIModule : public IModuleInterface
{
public:

    virtual void StartupModule () override;
    virtual void ShutdownModule () override;
};