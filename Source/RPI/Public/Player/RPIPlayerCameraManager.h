#pragma once

#include "Camera/PlayerCameraManager.h"
#include "RPIPlayerCameraManager.generated.h"

UCLASS()
class ARPIPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_UCLASS_BODY()

public:

    virtual void LimitViewPitch ( FRotator& ViewRotation, float InViewPitchMin, float InViewPitchMax ) override;
    virtual void LimitViewRoll ( FRotator& ViewRotation, float InViewRollMin, float InViewRollMax ) override;
    virtual void LimitViewYaw ( FRotator& ViewRotation, float InViewYawMin, float InViewYawMax ) override;
};
