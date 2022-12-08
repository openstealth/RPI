#include "Pawns/RPIPawn.h"
#include "Player/RPIPlayerController.h"
#include "Math/Ellipsoid.h"
#include "RPIHelper.h"

ARPIPawn::ARPIPawn ( const FObjectInitializer& ObjectInitializer ) : Super ( ObjectInitializer )
{
    PrimaryActorTick.bCanEverTick = true;
}

ARPIPawn::~ARPIPawn ()
{
}

APlayerController * ARPIPawn::GetPlayerController () const
{
    return static_cast<APlayerController*> ( Controller );
}

bool ARPIPawn::IsKeyDown ( const FKey& Key ) const
{
    APlayerController * PlayerController = GetPlayerController ();
    return PlayerController ? PlayerController->IsInputKeyDown ( Key ) : false;
}

void ARPIPawn::Move ( float Forward, float Side, float Up )
{
    if ( Forward != 0.0f || Side != 0.0f || Up != 0.0f )
    {
        if ( Forward != 0.0f )
            Up += Forward * FMath::Tan ( FMath::DegreesToRadians ( Rotation.Pitch ) );
        
        if ( Side != 0.0f )
            Up += -Side * FMath::Tan ( FMath::DegreesToRadians ( Rotation.Roll ) );

        SetPosition ( FEllipsoid::MovePosition ( GetWorld (), Position, Side, Forward, Up, Rotation.Yaw ) );
    }
}

FMatrixFP64 ARPIPawn::GeodeticToTransform ( UWorld * World, const FPolar& Position, const FRotator& Rotation )
{
    return FEllipsoid::GeodeticToTransform ( World, Position, Rotation );
}

void ARPIPawn::UpdateTransform ()
{
    WorldTransform = GeodeticToTransform ( GetWorld (), Position, Rotation );

#if USE_PLANAR_POSITION
    SetActorTransform ( WorldTransform.ToTransform () );
#else
    const FVectorFP64& CameraReferenceLocation = ARPIPlayerController::GetCameraReferenceLocation ( GetWorld () );

    FMatrixFP64 RenderTransform ( WorldTransform );
    RenderTransform.SetTranslation ( RenderTransform.GetOrigin () - CameraReferenceLocation );
        
    SetActorTransform ( RenderTransform.ToTransform () );
#endif
}

void ARPIPawn::SetPosition ( const FPolar& InGeodetic )
{
    Position = InGeodetic; 
    UpdateTransform ();
}

void ARPIPawn::SetPosition ( const FVectorFP64& InPosition )
{
    SetPosition ( FEllipsoid::PositionToGeodetic ( GetWorld (), InPosition ) );
}

void ARPIPawn::SetRotation ( float Yaw, float Pitch, float Roll )
{
    SetRotation ( FRotator ( Pitch, Yaw, Roll ) );
}

void ARPIPawn::SetRotation ( const FRotator& InRotation )
{
    Rotation = InRotation.Clamp ();
    UpdateTransform ();
}

void ARPIPawn::SetTransform ( const FPolar& InPosition, const FRotator& InRotation )
{
    Position = InPosition;
    Rotation = InRotation;
    UpdateTransform ();
}

void ARPIPawn::BeginPlay ()
{
    Super::BeginPlay ();
}