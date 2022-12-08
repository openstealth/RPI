#pragma once

#include "RPIWorldSubsystem.generated.h"

UCLASS()
class URPIWorldSubsystem final : public UWorldSubsystem
{
    GENERATED_BODY()

protected:

    class FRPIScene * Scene = nullptr;

public:

    URPIWorldSubsystem ();
    virtual ~URPIWorldSubsystem ();

    virtual void Initialize ( FSubsystemCollectionBase& Collection ) override;
    virtual void Deinitialize () override;

    bool IsGameWorld () const;

    FRPIScene * GetScene () const { return Scene; }
    
    static URPIWorldSubsystem * Get ( UWorld * World );
};