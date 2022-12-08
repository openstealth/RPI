#include "RPIWorldSettings.h"
#include "RPIGameMode.h"
#include "RPIHelper.h"
#include "RPILogger.h"

#include "Math/SunPosition.h"
#include "Math/Ellipsoid.h"
#include "Player/RPIPlayerController.h"
#include "Pawns/RPICamera.h"
#include "Actors/Weather.h"

#include "Materials/MaterialParameterCollection.h" 
#include "Materials/MaterialParameterCollectionInstance.h" 
#include "Components/SkyAtmosphereComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/DirectionalLight.h" 
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Components/SkyLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/PostProcessVolume.h"
#include "EngineUtils.h"

RPI_LOG_CATEGORY_STATIC ( LogRPIWorldSettings );

ARPIWorldSettings::ARPIWorldSettings ( const FObjectInitializer& ObjectInitializer )
    : Super ( ObjectInitializer )
{
    DefaultGameMode = ARPIGameMode::StaticClass ();
}

ARPIWorldSettings::~ARPIWorldSettings ()
{
}

void ARPIWorldSettings::CopyWorldSettings ( const ARPIWorldSettings& WorldSettings )
{
    SetTime ( WorldSettings.DateTime, WorldSettings.DateTimeLocation );
}

void ARPIWorldSettings::BeginPlay ()
{
    Super::BeginPlay ();
    Initialize ();
}

void ARPIWorldSettings::Initialize ()
{
    //WorldOrigin = FRPILevelManager::GetWorldOrigin ( GetWorld () );
    WorldOriginAtSeaLevel = FPolar ( WorldOrigin.Longitude, WorldOrigin.Latitude );

    // check out https://www.stateofartacademy.com/unreal-engine-4-24-sun-sky-breakdown/
    Atmosphere = GetActorOfClass<ASkyAtmosphere> ();
    SunLight = GetActorOfClass<ADirectionalLight> ();
    SkyLight = GetActorOfClass<ASkyLight> ();
    Weather = GetActorOfClass<AWeather> ();

    // См. PrepareSunLightProxy (SkyAtmosphereRendering.cpp)
    // При использовании геоида FAtmosphereSetup::GetTransmittanceAtGroundLevel (SkyAtmosphereCommonData.cpp)
    // возвращает неверное значение для данной позиции объекта (см. комментарии в оригинальном коде). 
    // Поэтому используется точное, но более медленное решение:
    UDirectionalLightComponent * SunLightComponent = SunLight ? static_cast<UDirectionalLightComponent*> ( SunLight->GetLightComponent () ) : nullptr;
    if ( SunLightComponent )
    {
        SunLightComponent->bPerPixelAtmosphereTransmittance = true;
        SunLightComponent->bAffectsWorld = true;
        SunLightComponent->MarkRenderStateDirty ();
    }

    USkyLightComponent * SkyLightComponent = SkyLight ? SkyLight->GetLightComponent () : nullptr;
    if ( SkyLightComponent )
    {
        SkyLightComponent->SetVisibility ( true );
        SkyLightComponent->bAffectsWorld = true;
        SkyLightComponent->MarkRenderStateDirty ();
    }

    USkyAtmosphereComponent * SkyAtmosphereComponent = Atmosphere ? Atmosphere->GetComponent () : nullptr;
    if ( SkyAtmosphereComponent && IsGameWorld () )
    {
#if USE_PLANAR_POSITION
        SkyAtmosphereComponent->TransformMode = ESkyAtmosphereTransformMode::PlanetTopAtAbsoluteWorldOrigin;
#else
        SkyAtmosphereComponent->TransformMode = ESkyAtmosphereTransformMode::PlanetCenterAtComponentTransform;
#endif

        const float KilometersPerUnit = 1e-3f / GetUnitsPerMeter ( GetWorld () );
        SkyAtmosphereComponent->BottomRadius = FEllipsoid::GetSeaLevel ( GetWorld (), WorldOrigin ) * KilometersPerUnit;
        SkyAtmosphereComponent->MarkRenderStateDirty ();

        LogNotify ( LogRPIWorldSettings, "Set SkyAtmosphereComponent.BottomRadius to %f km", SkyAtmosphereComponent->BottomRadius );
    }

#if WITH_EDITOR
    DateTimeLocation = WorldOriginAtSeaLevel;
    SetTime ( TimeOfDay );
#else
    SetTime ( DateTime, DateTimeLocation );
#endif
}

#if WITH_EDITOR

void ARPIWorldSettings::PostEditChangeProperty ( FPropertyChangedEvent& Event )
{
    if ( IS_PROPERTY_CHANGED ( Event, ARPIWorldSettings, TimeOfDay ) )
        SetTime ( TimeOfDay );

    Super::PostEditChangeProperty ( Event );
}

#endif

template<class Class>
Class * ARPIWorldSettings::GetActorOfClass ()
{
    Class * Actor = Cast<Class> ( UGameplayStatics::GetActorOfClass ( GetWorld (), Class::StaticClass () ) );

    if ( Actor )
    {
        LogNotify ( LogRPIWorldSettings, "%s (%s) has been found", *Actor->GetName (), *FRPIHelper::GetClassName <Class> () );
    }

    return Actor;
}

bool ARPIWorldSettings::IsGameWorld () const
{
    UWorld * World = GetWorld ();
    return World ? ( World->IsPlayInEditor () || World->IsGameWorld () ) : false;
}

void ARPIWorldSettings::SetTime ( const FDateTime& Time, const FPolar& Location )
{
    if ( GetWorld ()->bIsTearingDown )
    {
        LogWarning ( LogRPIWorldSettings, "SetTime: Failed due to world is tearing down" );
        return;
    }

    DateTime = Time;
    DateTimeLocation = Location;

    AActor * Light = SunLight ? static_cast<AActor*> ( SunLight ) : static_cast<AActor*> ( Atmosphere );

    if ( Light )
    {
        bool IsDay = true;
        float SunZenithAngleRadians = 0.0f;
        Light->SetActorTransform ( FSunPosition::Get ( Location.Latitude, Location.Longitude, Time, IsDay, SunElevationAngleRadians, SunZenithAngleRadians, !IsGameWorld () ) );
    }

    LogNotify ( LogRPIWorldSettings, "SetTime: %d.%d.%d %d:%d:%d (Lon: %f Lat: %f)", Time.GetDay (), Time.GetMonth (), Time.GetYear (), Time.GetHour (), Time.GetMinute (), Time.GetSecond (), Location.Longitude, Location.Latitude );
}

float ARPIWorldSettings::GetPlanetRadius () const
{
    USkyAtmosphereComponent * SkyAtmosphereComponent = Atmosphere ? Atmosphere->GetComponent () : nullptr;
    return ( SkyAtmosphereComponent ? SkyAtmosphereComponent->BottomRadius * 1e3f : 6360e3f ) * GetUnitsPerMeter ();
}

float ARPIWorldSettings::GetAtmosphereRadius () const
{
    USkyAtmosphereComponent * SkyAtmosphereComponent = Atmosphere ? Atmosphere->GetComponent () : nullptr;
    return ( SkyAtmosphereComponent ? ( SkyAtmosphereComponent->BottomRadius + SkyAtmosphereComponent->AtmosphereHeight ) * 1e3f : 6420e3f ) * GetUnitsPerMeter ();
}

void ARPIWorldSettings::SetTime ( float Time )
{
    SetTime ( FMath::Floor ( Time ), FMath::Frac ( Time ) * 60.0f );
}

void ARPIWorldSettings::SetTime ( uint32 Hour, uint32 Minute )
{
    SetTime ( FDateTime ( DateTime.GetYear (), DateTime.GetMonth (), DateTime.GetDay (), Hour, Minute ), DateTimeLocation );
}

void ARPIWorldSettings::SetTime ( uint32 Month )
{
    SetTime ( FDateTime ( DateTime.GetYear (), Month, DateTime.GetDay (), DateTime.GetHour (), DateTime.GetMinute (), DateTime.GetSecond () ), DateTimeLocation );
}