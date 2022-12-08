#include "Renderer/RPIScene.h"
#include "RenderingThread.h"
#include "RPIWorldSubsystem.h"
#include "Renderer/Private/SceneRendering.h"

FRPIScene * FRPIScene::Get ( UWorld * World )
{
    //return ARPIWorldSettings::GetScene ( World );
    URPIWorldSubsystem * WorldSubsystem = URPIWorldSubsystem::Get ( World );
    return WorldSubsystem ? WorldSubsystem->GetScene () : nullptr;
}

FRPIScene * FRPIScene::Get ( const FViewInfo& View )
{
    const FSceneInterface * Scene = View.Family ? View.Family->Scene : nullptr;
    return Scene ? Get ( Scene->GetWorld () ) : nullptr;
}

void FRPIScene::AddPlanetFog ( FPlanetFogSceneProxy * Fog )
{
    check ( Fog );
    FRPIScene * Scene = this;

    ENQUEUE_RENDER_COMMAND ( FAddCloudLayerCommand )
    (
        [Scene, Fog]( FRHICommandListImmediate& RHICmdList )
        {
            check ( Scene->PlanetFog != Fog );
            Scene->PlanetFog = Fog;
        }
    );
}

void FRPIScene::RemovePlanetFog ( FPlanetFogSceneProxy * Fog )
{
    check ( Fog );
    FRPIScene * Scene = this;

    ENQUEUE_RENDER_COMMAND ( FRemoveCloudLayerCommand )
    (
        [Scene, Fog]( FRHICommandListImmediate& RHICmdList )
        {
            check ( Scene->PlanetFog == Fog );
            Scene->PlanetFog = nullptr;
        }
    );
}