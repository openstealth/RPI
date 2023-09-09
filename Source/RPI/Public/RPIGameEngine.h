#pragma once

#include "Runtime/Engine/Classes/Engine/GameEngine.h"
#include "RPIGameEngine.generated.h"

UCLASS()
class RPI_API URPIGameEngine final : public UGameEngine
{
    GENERATED_BODY()

protected:

    virtual void Tick ( float DeltaSeconds, bool bIdleMode ) override;

public:

    URPIGameEngine ( const FObjectInitializer& ObjectInitializer );
    virtual ~URPIGameEngine ();
};