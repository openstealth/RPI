#include "Player/RPIPlayerController.h"
#include "Player/RPIPlayerCameraManager.h"
#include "Pawns/RPICamera.h"

ARPIPlayerController::ARPIPlayerController ( const FObjectInitializer& ObjectInitializer )
    : Super ( ObjectInitializer )
{
    PlayerCameraManagerClass = ARPIPlayerCameraManager::StaticClass ();

    bShowMouseCursor = true;
    CurrentMouseCursor = EMouseCursor::Default;
}

ARPICamera * ARPIPlayerController::GetCamera ( APlayerController * Controller )
{
    return Controller ? static_cast<ARPICamera*> ( Controller->GetPawn () ) : nullptr;
}

const FMatrixFP64& ARPIPlayerController::GetCameraWorldTransform ( APlayerController * Controller )
{
    ARPICamera * Camera = GetCamera ( Controller );
    return Camera ? Camera->GetWorldTransform () : FMatrixFP64::Identity;
}

const FVectorFP64& ARPIPlayerController::GetCameraReferenceLocation ( APlayerController * Controller )
{
    ARPICamera * Camera = GetCamera ( Controller );
    return Camera ? Camera->GetReferenceLocation () : FVectorFP64::ZeroVector;
}