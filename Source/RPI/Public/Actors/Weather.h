#pragma once

#include "GameFramework/Actor.h"
#include "Weather.generated.h"

UCLASS()
class RPI_API AWeather : public AActor
{
    GENERATED_BODY()

private:

    UPROPERTY(Transient)
    USceneComponent * AttachedToCamera = nullptr;

    class USkyAtmosphereComponent * SkyAtmosphereComponent = nullptr;

    UPROPERTY()
    class UPlanetFogComponent * FogComponent = nullptr;

public:

#if WITH_EDITORONLY_DATA
    UPROPERTY(EditAnywhere, Category="Control", meta=(UIMin = "0", UIMax = "5000"))
    float FogHeight = 0.0f;

    UPROPERTY(EditAnywhere, Category="Control", meta=(UIMin = "0", UIMax = "10000"))
    float FogStart = 0.0f;
#endif

private:

    UFUNCTION()
    void AdjustRotation ( double Longitude, double Latitude, double Altitude );

protected:

    virtual void BeginPlay () override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty ( FPropertyChangedEvent& Event ) override;
#endif

public:	

    AWeather ();

    void ApplyWorldOffset ( const FVector& InOffset, bool bWorldShift ) override;
    void SetFog ( float Height, float Start );

    static AWeather * Get ( UWorld * World );
};
