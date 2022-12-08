#pragma once

#include "Pawns/RPIPawn.h"
#include "Math/MatrixFP64.h"
#include "RPICamera.generated.h"

class USphereComponent;
class UCameraComponent;

UCLASS()
class RPI_API ARPICamera final : public ARPIPawn
{
    GENERATED_BODY()

protected:

    float HorizontalSpeed = 1.0f;
    float VerticalSpeed = 1.0f;
    float RotationSpeed = 1.0f;

    UCameraComponent * CameraComponent = nullptr;

    FPolar ReferencePosition;
    FVectorFP64 ReferenceLocation;

protected:

    virtual void UpdateTransform () override;

    void ChangeMovementSpeed ( float Value );

    void MoveForward ( float Value );
    void MoveRight ( float Value );
    void MoveUp ( float Value );

    void Turn ( float Value );
    void TurnRate ( float Value );
    void LookUp ( float Value );
    void LookUpRate ( float Value );
    void TiltRate ( float Value );

public:

    ARPICamera ( const FObjectInitializer& ObjectInitializer );
    virtual ~ARPICamera ();

    void CopyViewState ( const ARPICamera& Camera );

    void SetHorizontalSpeed ( float Speed ) { HorizontalSpeed = Speed; }
    void SetVerticalSpeed ( float Speed ) { VerticalSpeed = Speed; }
    void SetRotationSpeed ( float Speed ) { RotationSpeed = Speed; }

    const FPolar& GetReferencePosition () const { return ReferencePosition; }
    const FVectorFP64& GetReferenceLocation () const { return ReferenceLocation; }
        
    virtual void SetupPlayerInputComponent ( UInputComponent * PlayerInputComponent ) override;
    virtual UPawnMovementComponent * GetMovementComponent () const override;
    UCameraComponent * GetCameraComponent ();
};