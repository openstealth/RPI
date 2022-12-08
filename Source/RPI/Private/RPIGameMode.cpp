#include "RPIGameMode.h"
#include "Player/RPIPlayerController.h"
#include "Pawns/RPICamera.h"

ARPIGameMode::ARPIGameMode ( const FObjectInitializer& ObjectInitializer ) 
	: Super ( ObjectInitializer )
{
    DefaultPawnClass = ARPICamera::StaticClass ();
    PlayerControllerClass = ARPIPlayerController::StaticClass ();
}

ARPIGameMode::~ARPIGameMode ()
{
}