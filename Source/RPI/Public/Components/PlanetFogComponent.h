#pragma once

#include "Components/ActorComponent.h"
#include "Math/Polar.h" 
#include "PlanetFogComponent.generated.h"

UCLASS(ClassGroup = Rendering, collapsecategories, hidecategories = (Object, Mobility, Activation, "Components|Activation"), editinlinenew, meta = (BlueprintSpawnableComponent), MinimalAPI)
class UPlanetFogComponent final : public USceneComponent
{
    GENERATED_UCLASS_BODY()

private:

    struct FPlanetFogSceneProxy * PlanetFogSceneProxy = nullptr;

protected:

    // Fog layer height from current sea level, m
    UPROPERTY(EditAnywhere, meta=(ClampMin = "0.0", ClampMax = "60000.0"))
    float Height = 0.0f;
    
    // Distance from which fog starts, m
    UPROPERTY(EditAnywhere, meta=(ClampMin = "0.0", ClampMax = "10000.0"))
    float Start = 0.0f;

protected:

    virtual void CreateRenderState_Concurrent ( FRegisterComponentContext * Context ) override;
    virtual void DestroyRenderState_Concurrent () override;
    void UpdateRenderState ();

#if WITH_EDITOR
    virtual void PostEditChangeProperty ( FPropertyChangedEvent& Event ) override;
#endif

public:

    virtual ~UPlanetFogComponent ();

    void Set ( float Height, float Start );
    bool IsValid () const;
};