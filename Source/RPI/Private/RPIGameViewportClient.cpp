#include "RPIGameViewportClient.h"

URPIGameViewportClient::URPIGameViewportClient ( const FObjectInitializer& ObjectInitializer ) : Super ( ObjectInitializer )
{
}

URPIGameViewportClient::~URPIGameViewportClient ()
{
}

void URPIGameViewportClient::Init ( struct FWorldContext& WorldContext, UGameInstance * OwningGameInstance, bool bCreateNewAudioDevice )
{
    Super::Init ( WorldContext, OwningGameInstance, bCreateNewAudioDevice );

    GNearClippingPlane = 0.1f;
    SetMouseLockMode ( EMouseLockMode::DoNotLock );
    SetHideCursorDuringCapture ( false );
}