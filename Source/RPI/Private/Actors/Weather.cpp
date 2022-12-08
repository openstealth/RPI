#include "Actors/Weather.h"

#include "RPIHelper.h"
#include "RPILogger.h"
#include "RPIWorldSettings.h"
#include "RPISettings.h"

#include "Components/SkyAtmosphereComponent.h"
#include "Components/PlanetFogComponent.h"

#include "Kismet/GameplayStatics.h"

#include "Player/RPIPlayerController.h"
#include "Pawns/RPICamera.h"
#include "Math/Ellipsoid.h"

RPI_LOG_CATEGORY_STATIC ( LogWeather );

#define ADJUST_ROTATION_IN_APPLY_WORLD_OFFSET 1

AWeather::AWeather ()
{    
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent> ( TEXT ( "WeatherRoot" ) );

    AttachedToCamera = CreateDefaultSubobject<USceneComponent> ( TEXT ( "AttachedToCameraComponents" ) );
    AttachedToCamera->SetUsingAbsoluteRotation ( true );

    FogComponent = CreateDefaultSubobject<UPlanetFogComponent> ( TEXT ( "FogComponent" ) );
}

void AWeather::SetFog ( float Height, float Start )
{
    if ( !FogComponent )
    {
        LogWarning ( LogWeather, "SetFog: Cloud layer is invalid" );
        return;
    }

    FogComponent->Set ( Height, Start );
    
    LogNotify ( LogWeather, "SetFog: Height=%f Start=%f", Height, Start );
}

void AWeather::BeginPlay ()
{
    Super::BeginPlay ();

    ASkyAtmosphere* Atmosphere = Cast<ASkyAtmosphere> ( UGameplayStatics::GetActorOfClass ( GetWorld (), ASkyAtmosphere::StaticClass () ) );
    SkyAtmosphereComponent = Atmosphere ? Atmosphere->GetComponent () : nullptr;

    ARPICamera * Camera = ARPIPlayerController::GetCamera ( GetWorld () );
    if ( Camera )
    {
        FAttachmentTransformRules AttachRule ( EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false );
        AttachedToCamera->AttachToComponent ( Camera->GetRootComponent (), AttachRule );
#if !USE_PLANAR_POSITION && !ADJUST_ROTATION_IN_APPLY_WORLD_OFFSET
        Camera->OnCameraMoved.AddUObject ( this, &APrecipitations::AdjustRotation );
#endif
    }
}

#if WITH_EDITOR
void AWeather::PostEditChangeProperty ( FPropertyChangedEvent& Event )
{
    if ( IS_PROPERTY_CHANGED ( Event, AWeather, FogHeight ) ||
         IS_PROPERTY_CHANGED ( Event, AWeather, FogStart ) )
    {
        SetFog ( FogHeight, FogStart );
    }

    Super::PostEditChangeProperty ( Event );
}
#endif

void AWeather::ApplyWorldOffset ( const FVector& InOffset, bool bWorldShift )
{
    Super::ApplyWorldOffset ( InOffset, bWorldShift );
#if !USE_PLANAR_POSITION && ADJUST_ROTATION_IN_APPLY_WORLD_OFFSET
    ARPICamera* Camera = ARPIPlayerController::GetCamera ( GetWorld () );
    if ( Camera )
    {
        FPolar Position = Camera->GetPosition ();
        AdjustRotation ( Position.Longitude, Position.Latitude, Position.Radius );
    }
#endif
}

void AWeather::AdjustRotation ( double Longitude, double Latitude, double Altitude )
{
    const FMatrixFP64 Transform = FEllipsoid::GeodeticToTransform ( GetWorld (), FPolar ( Longitude, Latitude ) );
    AttachedToCamera->SetWorldRotation ( Transform.Rotator () );
}

AWeather * AWeather::Get ( UWorld * World )
{
    ARPIWorldSettings * WorldSettings = ARPIWorldSettings::Get ( World );
    return WorldSettings ? WorldSettings->GetWeather () : nullptr;
}