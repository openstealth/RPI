#include "Pawns/RPICamera.h"
#include "GameFramework/PlayerInput.h"
#include "Components/InputComponent.h"
#include "RPIHelper.h"
#include "Math/Ellipsoid.h"
#include "Camera/CameraComponent.h"

ARPICamera::ARPICamera (const FObjectInitializer& ObjectInitializer) : Super (ObjectInitializer)
{
    ReferencePosition = FPolar ( 360.0, 360.0 );

    BaseEyeHeight = 0.0f;
    bCollideWhenPlacing = false;
    SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    bUseControllerRotationPitch = true;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = true;

    RootComponent = CreateDefaultSubobject<USceneComponent> ( TEXT ( "CameraMovementComponent0" ) );

    CameraComponent = CreateDefaultSubobject<UCameraComponent> ( TEXT ( "CameraComponent" ) );
    if ( CameraComponent )
    {
        CameraComponent->SetupAttachment ( RootComponent );
        CameraComponent->bUsePawnControlRotation = true;
    }

    AutoPossessPlayer = EAutoReceiveInput::Player0;
}

ARPICamera::~ARPICamera ()
{
}

void ARPICamera::CopyViewState ( const ARPICamera& Camera )
{
    if ( CameraComponent && Camera.CameraComponent )
    {
        FTransform AdditiveOffset = FTransform::Identity;
        float AdditiveFovOffset = 0.0f;
        Camera.CameraComponent->GetAdditiveOffset ( AdditiveOffset, AdditiveFovOffset );

        CameraComponent->ClearAdditiveOffset ();
        CameraComponent->AddAdditiveOffset ( AdditiveOffset, AdditiveFovOffset );

        CameraComponent->SetFieldOfView ( Camera.CameraComponent->FieldOfView );
        CameraComponent->SetAspectRatio ( Camera.CameraComponent->AspectRatio );
    }

    //SetProjectionCenterOffset ( Camera.ProjectionCenter );

    SetPosition ( Camera.Position );
    SetRotation ( Camera.Rotation );
    SetHorizontalSpeed ( Camera.HorizontalSpeed );
    SetVerticalSpeed ( Camera.VerticalSpeed );
    SetRotationSpeed ( Camera.RotationSpeed );
}

void ARPICamera::SetupPlayerInputComponent ( UInputComponent * PlayerInputComponent )
{
    check ( PlayerInputComponent );

    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "MoveForward", EKeys::W, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "MoveForward", EKeys::S, -1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "MoveRight", EKeys::A, -1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "MoveRight", EKeys::D, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "MoveUp", EKeys::PageUp, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "MoveUp", EKeys::E, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "MoveUp", EKeys::PageDown, -1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "MoveUp", EKeys::Q, -1.0f ) );
    
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "ChangeSpeedFactor", EKeys::MouseWheelAxis, 0.1f ) );
    
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "Turn", EKeys::MouseX, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "TurnRate", EKeys::Left, -1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "TurnRate", EKeys::Right, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "LookUp", EKeys::MouseY, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "LookUpRate", EKeys::Up, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "LookUpRate", EKeys::Down, -1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "TiltRate", EKeys::LeftBracket, -1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "TiltRate", EKeys::RightBracket, 1.0f ) );

    PlayerInputComponent->BindAxis ( "LookUp", this, &ARPICamera::LookUp );
    PlayerInputComponent->BindAxis ( "LookUpRate", this, &ARPICamera::LookUpRate );
    PlayerInputComponent->BindAxis ( "Turn", this, &ARPICamera::Turn );
    PlayerInputComponent->BindAxis ( "TurnRate", this, &ARPICamera::TurnRate );
    PlayerInputComponent->BindAxis ( "TiltRate", this, &ARPICamera::TiltRate );
    PlayerInputComponent->BindAxis ( "MoveForward", this, &ARPICamera::MoveForward );
    PlayerInputComponent->BindAxis ( "MoveRight", this, &ARPICamera::MoveRight );
    PlayerInputComponent->BindAxis ( "MoveUp", this, &ARPICamera::MoveUp );
    PlayerInputComponent->BindAxis ( "ChangeSpeedFactor", this, &ARPICamera::ChangeMovementSpeed );

    // Binocular
    UPlayerInput::AddEngineDefinedActionMapping ( FInputActionKeyMapping ( "Binocular", EKeys::B ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "BinocularLookUp", EKeys::NumPadEight, 1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "BinocularLookUp", EKeys::NumPadTwo, -1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "BinocularTurn", EKeys::NumPadFour, -1.0f ) );
    UPlayerInput::AddEngineDefinedAxisMapping ( FInputAxisKeyMapping ( "BinocularTurn", EKeys::NumPadSix, 1.0f ) );
}

UPawnMovementComponent * ARPICamera::GetMovementComponent () const
{
    return nullptr;
}

UCameraComponent * ARPICamera::GetCameraComponent ()
{
    return CameraComponent;
}

void ARPICamera::ChangeMovementSpeed ( float Value )
{
    HorizontalSpeed = FMath::Max ( HorizontalSpeed + Value, 0.0f );
    VerticalSpeed = FMath::Max ( VerticalSpeed + Value, 0.0f );
}

void ARPICamera::MoveForward ( float Value )
{
    Move ( Value * HorizontalSpeed, 0.0f, 0.0f );
}

void ARPICamera::MoveRight ( float Value )
{
    Move ( 0.0f, Value * HorizontalSpeed, 0.0f );
}

void ARPICamera::MoveUp ( float Value )
{
    Move ( 0.0f, 0.0f, Value * VerticalSpeed );
}

void ARPICamera::Turn ( float Value )
{
    if ( IsKeyDown ( EKeys::LeftMouseButton ) || IsKeyDown ( EKeys::RightMouseButton ) )
        TurnRate ( Value * RotationSpeed );
}

void ARPICamera::TurnRate ( float Value )
{
    if ( Value != 0.0f )
        SetRotation ( Rotation.Yaw + Value * RotationSpeed, Rotation.Pitch, Rotation.Roll );
}

void ARPICamera::LookUp ( float Value )
{
    if ( IsKeyDown ( EKeys::LeftMouseButton ) || IsKeyDown ( EKeys::RightMouseButton ) )
        LookUpRate ( Value * RotationSpeed );
}

void ARPICamera::LookUpRate ( float Value )
{
    if ( Value != 0.0f )
        SetRotation ( Rotation.Yaw, Rotation.Pitch + Value * RotationSpeed, Rotation.Roll );
}

void ARPICamera::TiltRate ( float Value )
{
    if ( Value != 0.0f )
        SetRotation ( Rotation.Yaw, Rotation.Pitch, Rotation.Roll + Value * RotationSpeed );
}

void ARPICamera::UpdateTransform ()
{
    const FVectorFP64 PreviousLocation = WorldTransform.GetOrigin ();
    WorldTransform = GeodeticToTransform ( GetWorld (), Position, Rotation );
    
    APlayerController * PlayerController = GetPlayerController ();
    if ( PlayerController )
        PlayerController->SetControlRotation ( WorldTransform.Rotator () );
    
#if USE_PLANAR_POSITION
    SetActorLocation ( WorldTransform.GetOrigin ().ToVector () );    
#else
    const FVectorFP64 CurrentLocation = WorldTransform.GetOrigin ();
    const FVector DeltaLocation = ( CurrentLocation - PreviousLocation ).ToVector ();

    const double Threshold = 1.0;
    const FPolar DeltaPosition = Position - ReferencePosition;
    if ( FMath::Abs ( DeltaPosition.Longitude ) >= Threshold ||
         FMath::Abs ( DeltaPosition.Latitude ) >= Threshold )
    {
        ULevel * Level = GetLevel ();

        if ( Level )
        {
            const FVector WorldOffset = ( ReferenceLocation - CurrentLocation ).ToVector ();
            Level->ApplyWorldOffset ( WorldOffset, false );
        }
        
        SetActorLocation ( FVector::ZeroVector );
        ReferencePosition = FPolar ( Position ).Floor ();
        ReferenceLocation = CurrentLocation;
    }
    else
    if ( !DeltaLocation.IsZero () )
    {
        AddActorWorldOffset ( DeltaLocation );
    }
#endif
}