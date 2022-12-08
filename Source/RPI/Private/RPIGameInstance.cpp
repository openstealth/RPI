#include "RPIGameInstance.h"
#include "RPIWorldSettings.h"
#include "RPISettings.h"
#include "RPILogger.h"
#include "RPIHelper.h"

#include "Player/RPIPlayerController.h"
#include "Actors/Weather.h"
#include "Pawns/RPICamera.h"

#include "Math/Ellipsoid.h"

#include "GameFramework/GameUserSettings.h"
#include "GameMapsSettings.h"
#include "ShaderCore.h"
#include "EngineUtils.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "HAL/FileManagerGeneric.h"

RPI_LOG_CATEGORY_STATIC ( LogRPIGameInstance );

static void SetupCamera ( ARPICamera * Camera, const FRPISettings& Settings )
{
    if ( !Camera )
    {
        LogWarning ( LogRPIGameInstance, "Setup camera failed (camera is null)" );
        return;
    }

    const FViewSettings& View = Settings.View;

    UCameraComponent * CameraComponent = Camera->GetCameraComponent ();

    if ( CameraComponent )
    {
        LogNotify ( LogRPIGameInstance, "Set camera view offset: X=%f Y=%f", View.ViewOffset.X, View.ViewOffset.Y );
        CameraComponent->AddAdditiveOffset ( FTransform ( FRotator ( View.ViewOffset.Y, View.ViewOffset.X, 0 ) ), 0 );

        LogNotify ( LogRPIGameInstance, "Set camera horizontal FOV: %f", View.FOV );
        CameraComponent->SetFieldOfView ( View.FOV );
    
        if ( View.UseVerticalFOV )
        {
            LogNotify ( LogRPIGameInstance, "Set camera vertical FOV: %f", View.VerticalFOV );
            CameraComponent->SetAspectRatio ( View.FOV / View.VerticalFOV );
        }
    }
    else
    {
        LogWarning ( LogRPIGameInstance, "Can't setup camera frustum (camera component is null)" );
    }

    //Camera->SetProjectionCenterOffset ( View.ProjectionCenter );
    //LogNotify ( LogRPIGameInstance, "Set camera projection center: X=%f Y=%f Z=%f", View.ProjectionCenter.X, View.ProjectionCenter.Y, View.ProjectionCenter.Z );

    Camera->SetPosition ( View.Position );
    LogNotify ( LogRPIGameInstance, "Set camera position: Lon=%f Lat=%f Alt=%f", View.Position.Longitude, View.Position.Latitude, View.Position.Radius );

    Camera->SetRotation ( View.Rotation );
    LogNotify ( LogRPIGameInstance, "Set camera rotation: Yaw=%f Pitch=%f Roll=%f", View.Rotation.Yaw, View.Rotation.Pitch, View.Rotation.Roll );

    Camera->SetHorizontalSpeed ( Settings.Common.HorizontalSpeed );
    Camera->SetVerticalSpeed ( Settings.Common.VerticalSpeed );
    Camera->SetRotationSpeed ( Settings.Common.RotationSpeed );
}

URPIGameInstance::URPIGameInstance ( const FObjectInitializer& ObjectInitializer ) : Super ( ObjectInitializer )
{
}

URPIGameInstance::~URPIGameInstance ()
{
}

#if WITH_EDITOR

FGameInstancePIEResult URPIGameInstance::StartPlayInEditorGameInstance ( ULocalPlayer * LocalPlayer, const FGameInstancePIEParameters& Params )
{
    const FGameInstancePIEResult Result = Super::StartPlayInEditorGameInstance ( LocalPlayer, Params );
    
    SetupPlayInEditorView ();
    return Result;
}

void URPIGameInstance::SetupPlayInEditorView ()
{
    FRPISettings Settings = FRPISettings::Get ();

    if ( APlayerController * PlayerController = GetPlayerController () )
    {
        FViewSettings& ViewSettings = Settings.View;

        FVector EditorPosition = FVector::ZeroVector;
        PlayerController->GetActorEyesViewPoint ( EditorPosition, ViewSettings.Rotation );
        ViewSettings.Position = FEllipsoid::PlanarPositionToGeodetic ( GetWorld (), EditorPosition );
    }

    SetupCamera ( GetCamera (), Settings );
}

#endif // WITH_EDITOR

void URPIGameInstance::StartGameInstance ()
{  
    Super::StartGameInstance ();
   
    SetupView ();
    SetupWorldSettings ();
}

void URPIGameInstance::Shutdown ()
{
    Super::Shutdown ();
}

void URPIGameInstance::SetupWorldSettings ()
{
    const FRPISettings& Settings = FRPISettings::Get ();

    if ( ARPIWorldSettings * WorldSettings = GetWorldSettings () )
        WorldSettings->SetTime ( Settings.Common.Time (), Settings.View.Position );

    if ( AWeather * Weather = AWeather::Get ( GetWorld () ) )
        Weather->SetFog ( 0.0f, 0.0f );
}

void URPIGameInstance::SetupView ()
{
    const FRPISettings& Settings = FRPISettings::Get ();

    SetupCamera ( GetCamera (), Settings );

    bShowCursor = Settings.Common.bShowCursor;
}

void URPIGameInstance::ConsoleCommand ( const FString& Command ) const
{
    ARPIPlayerController * PlayerController = GetPlayerController ();
    if ( PlayerController )
    {
        LogNotify ( LogRPIGameInstance, "Executing console command %s", *Command );
        PlayerController->ConsoleCommand ( Command );
    }
    else
    {
        LogError ( LogRPIGameInstance, "Can't execute console command %s: PlayerController is NULL", *Command );
    }
}

void URPIGameInstance::Exit ()
{
#if 1
    RequestEngineExit ( TEXT ( "RPI has requested exit" ) );
#else
    ConsoleCommand ( "Exit" );
#endif
}

void URPIGameInstance::Tick ( float DeltaSeconds, bool bIdleMode )
{
    ProcessInput ();
}

void URPIGameInstance::SetCameraSpeed ( float HorizontalSpeed, float VerticalSpeed, float RotationSpeed )
{
    if ( ARPICamera * Camera = GetCamera () )
    {
        Camera->SetHorizontalSpeed ( HorizontalSpeed );
        Camera->SetVerticalSpeed ( VerticalSpeed );
        Camera->SetRotationSpeed ( RotationSpeed );
    }
}

void URPIGameInstance::ProcessInput ()
{
    APlayerController * PlayerController = GetPlayerController ();
    if ( PlayerController )
    {
        PlayerController->bShowMouseCursor = bShowCursor;
        PlayerController->CurrentMouseCursor = bShowCursor ? EMouseCursor::Default : EMouseCursor::None;
    }

    ARPIWorldSettings * WorldSettings = GetWorldSettings ();
    AWeather * Weather = AWeather::Get ( GetWorld () );

    const bool bIsControlDown = IsKeyDown ( EKeys::LeftControl ) || IsKeyDown ( EKeys::RightControl );
    const bool bIsAltDown = IsKeyDown ( EKeys::LeftAlt ) || IsKeyDown ( EKeys::RightAlt );
    const bool bIsShiftDown = IsKeyDown ( EKeys::LeftShift ) || IsKeyDown ( EKeys::RightShift );

    const bool bIsUpArrowDown = IsKeyDown ( EKeys::NumPadEight );
    const bool bIsDownArrowDown = IsKeyDown ( EKeys::NumPadTwo );
    const bool bIsLeftArrowDown = IsKeyDown ( EKeys::NumPadFour );
    const bool bIsRightArrowDown = IsKeyDown ( EKeys::NumPadSix );

    if ( ( bIsAltDown && IsKeyDown ( EKeys::F4 ) ) || IsKeyDown ( EKeys::Escape ) )
    {
        Exit ();
    }
    else
    if ( IsKeyDown ( EKeys::M ) && WorldSettings )
    {
        const FTimespan DeltaTime ( 0, 1, 0 );
        FDateTime Time = WorldSettings->GetTime ();
        Time += bIsControlDown ? DeltaTime : -DeltaTime;
        WorldSettings->SetTime ( Time, WorldSettings->GetTimeLocation () );
    }
    else
    if ( IsKeyDown ( EKeys::F ) && WorldSettings && Weather )
    {
        static float FogHeight = 0.0f;
        static float FogStart = 0.0f;

        const float DeltaHeight = bIsUpArrowDown ? 1.0f : ( bIsDownArrowDown ? -1.0f : 0.0f );
        const float DeltaStart = bIsRightArrowDown ? 1.0f : ( bIsLeftArrowDown ? -1.0f : 0.0f );
        
        if ( DeltaHeight || DeltaStart )
        {
            FogHeight = FMath::Clamp ( FogHeight + DeltaHeight, 0.0f, WorldSettings->GetAtmosphereRadius () - WorldSettings->GetPlanetRadius () );
            FogStart = FMath::Clamp ( FogStart + DeltaStart, 0.0f, 1e4f );

            Weather->SetFog ( FogHeight, FogStart );
        }
    }
    else
    if ( IsKeyDown ( EKeys::L ) )
    {
        //SetLightsEnable ( !bIsShiftDown );
    }
}

bool URPIGameInstance::IsKeyDown ( const FKey& Key ) const
{
    APlayerController * PlayerController = GetPlayerController ();
    return PlayerController ? PlayerController->IsInputKeyDown ( Key ) : false;
}

ARPIPlayerController * URPIGameInstance::GetPlayerController () const
{
    return ARPIPlayerController::Get ( GetWorld () );
}

ARPICamera * URPIGameInstance::GetCamera () const
{
    return ARPIPlayerController::GetCamera ( GetWorld () );
}

ARPIWorldSettings * URPIGameInstance::GetWorldSettings () const
{
    return ARPIWorldSettings::Get ( GetWorld () );
}