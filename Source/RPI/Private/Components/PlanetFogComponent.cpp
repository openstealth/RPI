#include "Components/PlanetFogComponent.h"
#include "Renderer/PlanetFogSceneProxy.h"
#include "Renderer/RPIScene.h"
#include "RPIWorldSettings.h"
#include "RPIHelper.h"

UPlanetFogComponent::UPlanetFogComponent ( const FObjectInitializer& ObjectInitializer )
    : Super ( ObjectInitializer )
{
    PrimaryComponentTick.bCanEverTick = false;
    bTickInEditor = false;

    SetComponentTickEnabled ( false );
}

UPlanetFogComponent::~UPlanetFogComponent ()
{
}

void UPlanetFogComponent::CreateRenderState_Concurrent ( FRegisterComponentContext * Context )
{
    Super::CreateRenderState_Concurrent ( Context );

    bool bHidden = false;
#if WITH_EDITORONLY_DATA
    bHidden = GetOwner () ? GetOwner ()->bHiddenEdLevel : false;
#endif // WITH_EDITORONLY_DATA
    if ( !ShouldComponentAddToScene () )
        bHidden = true;

    if ( GetVisibleFlag () && !bHidden &&
         ShouldComponentAddToScene () && ShouldRender () && IsRegistered () && ( GetOuter () == NULL || !GetOuter ()->HasAnyFlags ( RF_ClassDefaultObject ) ) )
    {
        PlanetFogSceneProxy = new FPlanetFogSceneProxy (); 
        UpdateRenderState ();
        
        FRPIScene * Scene = FRPIScene::Get ( GetWorld () );
        if ( Scene )
            Scene->AddPlanetFog ( PlanetFogSceneProxy );
    }
}

void UPlanetFogComponent::DestroyRenderState_Concurrent ()
{
    Super::DestroyRenderState_Concurrent ();

    if ( !PlanetFogSceneProxy )
        return;

    FRPIScene * Scene = FRPIScene::Get ( GetWorld () );
    if ( Scene )
        Scene->RemovePlanetFog ( PlanetFogSceneProxy );

    FPlanetFogSceneProxy * SceneProxy = PlanetFogSceneProxy;
    ENQUEUE_RENDER_COMMAND ( FDestroyCloudLayerCommand ) (
        [SceneProxy]( FRHICommandList& RHICmdList )
    {
        delete SceneProxy;
    } );

    PlanetFogSceneProxy = nullptr;
}

void UPlanetFogComponent::UpdateRenderState ()
{
    if ( !PlanetFogSceneProxy )
        return;

    ARPIWorldSettings * WorldSettings = ARPIWorldSettings::Get ( GetWorld () );
    if ( !WorldSettings )
        return;
    
    const float MetersToKilometers = 1e-3f;
    const float KilometersToMeters = 1e3f;
    const float KilometersPerUnit = MetersToKilometers / WorldSettings->GetUnitsPerMeter ();

    FPlanetFogSceneProxy Transient;
    Transient.PlanetRadiusKm = WorldSettings->GetPlanetRadius () * KilometersPerUnit;
    Transient.AtmosphereRadiusKm = WorldSettings->GetAtmosphereRadius () * KilometersPerUnit;
    Transient.TopRadiusKm = Transient.PlanetRadiusKm + Height * MetersToKilometers;
    Transient.StartKm = Start * MetersToKilometers;

    FPlanetFogSceneProxy * SceneProxy = PlanetFogSceneProxy;
    ENQUEUE_RENDER_COMMAND ( FUpdateCloudLayerCommand ) (
        [SceneProxy, Transient]( FRHICommandList& RHICmdList )
    {
        *SceneProxy = Transient;
    } );
}

#if WITH_EDITOR
void UPlanetFogComponent::PostEditChangeProperty ( FPropertyChangedEvent& Event )
{
    if ( IS_PROPERTY_CHANGED ( Event, UPlanetFogComponent, Height ) ||
         IS_PROPERTY_CHANGED ( Event, UPlanetFogComponent, Start ) )
    {
        Set ( Height, Start );
    }

    Super::PostEditChangeProperty ( Event );
}
#endif

bool UPlanetFogComponent::IsValid () const
{
    return ( Height > 0.0f ) && ( Start < FPlanetFogSceneProxy::MaxStartDistanceKm * 1e3f );
}

void UPlanetFogComponent::Set ( float InHeight, float InStart )
{
    Height = InHeight;
    Start = InStart;
    UpdateRenderState ();
}