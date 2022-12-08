#pragma once

#include "Math/Polar.h"
#include "GameFramework/WorldSettings.h"
#include "RPIWorldSettings.generated.h"

UCLASS()
class RPI_API ARPIWorldSettings final : public AWorldSettings
{
    GENERATED_BODY()

protected:

    class AWeather * Weather = nullptr;
    class ASkyAtmosphere * Atmosphere = nullptr;
    class ADirectionalLight * SunLight = nullptr;
    class ASkyLight * SkyLight = nullptr;
    
    FDateTime DateTime = FDateTime::Today ();
    FPolar DateTimeLocation = FPolar::ZeroPolar;
    float SunElevationAngleRadians = 0;

    FPolar WorldOrigin;
    FPolar WorldOriginAtSeaLevel;

public:

#if WITH_EDITORONLY_DATA

    UPROPERTY(EditAnywhere, Category="RPI Environment", meta=(UIMin = "0", UIMax = "23.99"))
    float TimeOfDay = 12.0f;

#endif

protected:

    virtual void BeginPlay () override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty ( FPropertyChangedEvent& Event ) override;
#endif

    template<class Class>
    Class * GetActorOfClass ();

public:

    ARPIWorldSettings ( const FObjectInitializer& ObjectInitializer );
    virtual ~ARPIWorldSettings ();

    void Initialize ();

    void CopyWorldSettings ( const ARPIWorldSettings& WorldSettings );
    bool IsGameWorld () const;

    void SetTime ( const FDateTime& Time, const FPolar& Location );
    void SetTime ( float Time );
    void SetTime ( uint32 Hour, uint32 Minute );
    void SetTime ( uint32 Month );

    AWeather * GetWeather () const { return Weather; }
    const FDateTime& GetTime () const { return DateTime; }
    const FPolar& GetTimeLocation () const { return DateTimeLocation; }
    
    float GetPlanetRadius () const;
    float GetAtmosphereRadius () const;
    float GetUnitsPerMeter () const { return WorldToMeters; }

    const FPolar& GetWorldOrigin () const { return WorldOrigin; }
    const FPolar& GetWorldOriginAtSeaLevel () const { return WorldOriginAtSeaLevel; }

    static float GetPlanetRadius ( UWorld * World )
    {
        ARPIWorldSettings * WorldSettings = Get ( World );
        return WorldSettings ? WorldSettings->GetPlanetRadius () : 0.0f;
    }

    static FPolar GetWorldOrigin ( UWorld * World )
    {
        ARPIWorldSettings * WorldSettings = Get ( World );
        return WorldSettings ? WorldSettings->WorldOrigin : FPolar::ZeroPolar;
    }

    static FPolar GetWorldOriginAtSeaLevel ( UWorld * World )
    {
        ARPIWorldSettings * WorldSettings = Get ( World );
        return WorldSettings ? WorldSettings->WorldOriginAtSeaLevel : FPolar::ZeroPolar;
    }

    static float GetUnitsPerMeter ( UWorld * World )
    {
        ARPIWorldSettings * WorldSettings = Get ( World );
        return WorldSettings ? WorldSettings->GetUnitsPerMeter () : 100.0f;
    }

    static ARPIWorldSettings * Get ( UWorld * World )
    {
        return World ? static_cast<ARPIWorldSettings*> ( World->GetWorldSettings () ) : nullptr;
    }   
};