#pragma once

#include "GameFramework/GameModeBase.h"
#include "RPIGameMode.generated.h"

UCLASS()
class RPI_API ARPIGameMode final : public AGameModeBase
{
	GENERATED_BODY()
public:

    ARPIGameMode ( const FObjectInitializer& ObjectInitializer );
    virtual ~ARPIGameMode ();
};
