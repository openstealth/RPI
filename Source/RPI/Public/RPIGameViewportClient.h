#pragma once

#include "Engine/Classes/Engine/GameViewportClient.h"
#include "RPIGameViewportClient.generated.h"

UCLASS()
class RPI_API URPIGameViewportClient final : public UGameViewportClient
{
	GENERATED_BODY()

protected:

    virtual void Init ( struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true ) override;

public:

    URPIGameViewportClient ( const FObjectInitializer& ObjectInitializer );
    virtual ~URPIGameViewportClient ();
};