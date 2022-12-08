#include "RPI.h"
#include "RPISettings.h"
#include "Renderer/RPISceneRenderer.h"

#include "Misc/Paths.h"
#include "RendererInterface.h"
#include "EngineModule.h"

template<typename T>
void SetConsoleVariable ( const TCHAR * Name, T Value )
{
    IConsoleVariable * CVarShaderDevelopmentMode = IConsoleManager::Get ().FindConsoleVariable ( Name );
    if ( CVarShaderDevelopmentMode )
        CVarShaderDevelopmentMode->Set ( Value );
}

static void ConfigureShaderDevelopment ()
{
    SetConsoleVariable ( TEXT ( "r.ShaderDevelopmentMode" ), 1 );
    //SetConsoleVariable ( TEXT ( "r.DumpShaderDebugInfo" ), 1 );
}

void FRPIModule::StartupModule ()
{
    const FRPISettings& Settings = FRPISettings::Get ();

    SetConsoleVariable ( TEXT ( "r.EnableDebugSpam_GetObjectPositionAndScale" ), 0 );

    SetConsoleVariable ( TEXT ( "t.UnacceptableFrameTimeThreshold" ), 1e9f );
    SetConsoleVariable ( TEXT ( "t.TargetFrameTimeThreshold" ), 1e9f );

    FString ShaderDirectory = FPaths::Combine ( FPaths::ProjectDir (), TEXT ( "Shaders" ) );
    AddShaderSourceDirectoryMapping ( "/RPI", ShaderDirectory );

#if 1
    ConfigureShaderDevelopment ();
#endif

    const FRenderingSettings& Rendering = Settings.Rendering;

    FRPISceneRendererParameters& RendererParameters = FRPISceneRenderer::Parameters;

    FFogRendererParameters& FogRendererParameters = RendererParameters.FogRendererParameters;
    FogRendererParameters.FalloffScale = Rendering.FogFalloffScale;
    FogRendererParameters.HeightFalloffScale = Rendering.FogHeightFalloffScale;

    const FPostOpaqueRenderDelegate Delegate = FPostOpaqueRenderDelegate::CreateLambda
    (
        [] ( FPostOpaqueRenderParameters& RenderParameters )
        {
            //const FViewInfo& View = *static_cast<const FViewInfo*> ( RenderParameters.Uid );
            FRPISceneRenderer::Render ( *RenderParameters.View, *RenderParameters.GraphBuilder );
        }
    );

    GetRendererModule ().RegisterPostOpaqueRenderDelegate ( Delegate );
    //GetRendererModule ().RegisterOverlayRenderDelegate ( Delegate );
}

void FRPIModule::ShutdownModule ()
{
    //ResetAllShaderSourceDirectoryMappings ();
}

IMPLEMENT_PRIMARY_GAME_MODULE ( FRPIModule, RPI, "RPI" );