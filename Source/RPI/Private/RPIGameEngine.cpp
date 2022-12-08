#include "RPIGameEngine.h"
#include "RPIGameInstance.h"

URPIGameEngine::URPIGameEngine ( const FObjectInitializer& ObjectInitializer ) : Super ( ObjectInitializer )
{
}

URPIGameEngine::~URPIGameEngine ()
{
}

void URPIGameEngine::Tick ( float DeltaSeconds, bool bIdleMode )
{
    URPIGameInstance * RPIGameInstance = Cast<URPIGameInstance> ( GameInstance );
    if ( RPIGameInstance )
        RPIGameInstance->Tick ( DeltaSeconds, bIdleMode );
    Super::Tick ( DeltaSeconds, bIdleMode );
}