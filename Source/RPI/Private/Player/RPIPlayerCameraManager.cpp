#include "Player/RPIPlayerCameraManager.h"

static float WarpAngle ( float Angle )
{
    return fmodf ( Angle, 360.0f );
}

ARPIPlayerCameraManager::ARPIPlayerCameraManager ( const FObjectInitializer& ObjectInitializer ) : Super ( ObjectInitializer )
{
    bUseClientSideCameraUpdates = false;
    bAlwaysApplyModifiers = true;
}

void ARPIPlayerCameraManager::LimitViewPitch ( FRotator& ViewRotation, float InViewPitchMin, float InViewPitchMax )
{
    ViewRotation.Pitch = WarpAngle ( ViewRotation.Pitch );
}

void ARPIPlayerCameraManager::LimitViewRoll ( FRotator& ViewRotation, float InViewRollMin, float InViewRollMax )
{
    ViewRotation.Roll = WarpAngle ( ViewRotation.Roll );
}

void ARPIPlayerCameraManager::LimitViewYaw ( FRotator& ViewRotation, float InViewYawMin, float InViewYawMax )
{
    ViewRotation.Yaw = WarpAngle ( ViewRotation.Yaw );
}