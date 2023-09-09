#include "Renderer/RPISceneRenderer.h"
#include "Renderer/RPIScene.h"
#include "Renderer/PlanetFogSceneProxy.h"

#include "Runtime/Renderer/Private/ScenePrivate.h"
#include "DataDrivenShaderPlatformInfo.h"

static FAutoConsoleVariableRef CVarFogRendererHeightFalloffScale
(
    TEXT ( "r.FogRenderer.HeightFalloffScale" ),
    FRPISceneRenderer::Parameters.FogRendererParameters.HeightFalloffScale,
    TEXT ( "Height falloff scale (km) for computing fog extinction coefficient."
           " Controls density value Dy for given altitude A: Dy = exp ( -A * HeightFalloffScale / FogThickness )." )
);

static FAutoConsoleVariableRef CVarFogRendererFalloffScale
(
    TEXT ( "r.FogRenderer.FalloffScale" ),
    FRPISceneRenderer::Parameters.FogRendererParameters.FalloffScale,
    TEXT ( "Falloff scale (km) for fog computing extinction coefficient."
           " Controls density value Dx for given distance L: Dx = L * FalloffScale / FogStart."  )
);

DECLARE_GPU_STAT ( PlanetFogRendering );

class FFogRenderingResources : public FRenderResource
{
protected:

    mutable TRefCountPtr<IPooledRenderTarget> FogTransmittaceLutTexture;

protected:

    virtual void ReleaseRHI () override
    {
        FogTransmittaceLutTexture.SafeRelease ();
    }

public:

    static const FFogRenderingResources& Get ()
    {
        static TGlobalResource<FFogRenderingResources> Resources;
        return Resources;
    }

    TRefCountPtr<IPooledRenderTarget>& GetFogTransmittanceLutTexture () const
    {
        if ( !FogTransmittaceLutTexture.IsValid () )
        {
            FRHICommandListImmediate& CmdList = FRHICommandListExecutor::GetImmediateCommandList ();

            FPooledRenderTargetDesc Desc;
            Desc = FPooledRenderTargetDesc::Create2DDesc ( FIntPoint ( 2048u, 1024u ), EPixelFormat::PF_FloatRGBA, FClearValueBinding::None, TexCreate_None, TexCreate_ShaderResource | TexCreate_HideInVisualizeTexture | TexCreate_NoTiling | TexCreate_RenderTargetable | TexCreate_UAV, false, 1u, true, false, false );
            GRenderTargetPool.FindFreeElement ( CmdList, Desc, FogTransmittaceLutTexture, TEXT ( "PlanetFogRendering.FogTransmittanceLut" ) );
        }

        return FogTransmittaceLutTexture;
    }
};

BEGIN_SHADER_PARAMETER_STRUCT(FFogUniformShaderParameters,)
    SHADER_PARAMETER ( float, PlanetRadiusKm )
    SHADER_PARAMETER ( float, AtmosphereRadiusKm )
    SHADER_PARAMETER ( float, RadiusKm )
    SHADER_PARAMETER ( float, HeightFalloffKm )
    SHADER_PARAMETER ( float, FalloffKm )
END_GLOBAL_SHADER_PARAMETER_STRUCT()

class FFogPS : public FGlobalShader
{
public:

    DECLARE_SHADER_TYPE ( FFogPS, Global );
    
    SHADER_USE_PARAMETER_STRUCT ( FFogPS, FGlobalShader );
    BEGIN_SHADER_PARAMETER_STRUCT ( FParameters, )
        RENDER_TARGET_BINDING_SLOTS ()
        SHADER_PARAMETER_STRUCT_REF ( FViewUniformShaderParameters, ViewUniformBuffer )
        SHADER_PARAMETER_STRUCT_INCLUDE ( FFogUniformShaderParameters, FogUniformBuffer )
        SHADER_PARAMETER_RDG_TEXTURE ( Texture2D, SceneDepthTexture )
    END_SHADER_PARAMETER_STRUCT ()

public:
    
    static bool ShouldCompilePermutation ( const FGlobalShaderPermutationParameters& Parameters )
    {
        return IsFeatureLevelSupported ( Parameters.Platform, ERHIFeatureLevel::SM5 );
    }    
};

IMPLEMENT_SHADER_TYPE ( , FFogPS, TEXT ( "/RPI/Private/PlanetFog.usf" ), TEXT ( "FogPS" ), SF_Pixel );

static const FPlanetFogSceneProxy * GetPlanetFogSceneProxy ( const FViewInfo& View )
{
    FRPIScene * Scene = FRPIScene::Get ( View );
    const FPlanetFogSceneProxy * Proxy = Scene ? Scene->GetPlanetFog () : nullptr;
    return ( Proxy && Proxy->IsValid () ) ? Proxy : nullptr;
}

void GetFogShaderParameters ( const FViewInfo& View, float& FogRadiusKm, float& FogHeightFalloffKm, float& FogFalloffKm )
{
    FogRadiusKm = 0.0f;
    FogHeightFalloffKm = 0.0f;
    FogFalloffKm = 0.0f;

    const FPlanetFogSceneProxy * Proxy = GetPlanetFogSceneProxy ( View );
    
    if ( !Proxy )
        return;

    const FFogRendererParameters& RendererParameters = FRPISceneRenderer::Parameters.FogRendererParameters;

    FogHeightFalloffKm = RendererParameters.HeightFalloffScale / FMath::Max ( Proxy->Thickness (), 1e-6f );
    FogFalloffKm = RendererParameters.FalloffScale / FMath::Max ( Proxy->StartKm, 1e-6f );

#if 0
    FogRadiusKm = ( FalloffKm > 0.0f ) ? Proxy->FogRadiusKm : 0.0f;
#else    
    const float ExtinctionThreshold = 1e-3f;
    
    if ( FogFalloffKm > 0.0f )
    {
        FogRadiusKm = ( FogHeightFalloffKm > 0.0f ) ? ( -FMath::Loge ( ExtinctionThreshold / FogFalloffKm ) / FogHeightFalloffKm + Proxy->PlanetRadiusKm ) : Proxy->TopRadiusKm;
        FogRadiusKm = FMath::Min ( FogRadiusKm, Proxy->TopRadiusKm );
    }
#endif
}

static void SetupFogUniformParameters ( const FViewInfo& View, const FPlanetFogSceneProxy& Proxy, FFogUniformShaderParameters& Parameters )
{
    const FFogRendererParameters& RendererParameters = FRPISceneRenderer::Parameters.FogRendererParameters;

    Parameters.PlanetRadiusKm = Proxy.IsValid () ? Proxy.PlanetRadiusKm : 0.0f;
    Parameters.AtmosphereRadiusKm = Proxy.IsValid () ? Proxy.AtmosphereRadiusKm : 0.0f;

    GetFogShaderParameters ( View, Parameters.RadiusKm, Parameters.HeightFalloffKm, Parameters.FalloffKm );
}

void FRPISceneRenderer::RenderFog ( const FViewInfo& View, FRDGBuilder& Builder )
{
    const FPlanetFogSceneProxy * FogProxy = GetPlanetFogSceneProxy ( View );

    if ( !FogProxy )
        return;
    
    FFogUniformShaderParameters FogParameters;
    SetupFogUniformParameters ( View, *FogProxy, FogParameters );

    RDG_EVENT_SCOPE ( Builder, "PlanetFogRendering" );
    RDG_GPU_STAT_SCOPE ( Builder, PlanetFogRendering );

    const FSceneTextures& SceneTextures = View.GetSceneTextures ();

    FGlobalShaderMap * ShaderMap = View.ShaderMap;
    TUniformBufferRef<FViewUniformShaderParameters> ViewUniformBuffer = View.ViewUniformBuffer;

    TShaderMapRef<FFogPS> FogPS ( ShaderMap );

    FFogPS::FParameters * PsParameters = Builder.AllocParameters<FFogPS::FParameters> ();
    PsParameters->ViewUniformBuffer = ViewUniformBuffer;
    PsParameters->FogUniformBuffer = FogParameters;
    PsParameters->SceneDepthTexture = SceneTextures.Depth.Target;
    PsParameters->RenderTargets[0] = FRenderTargetBinding ( SceneTextures.Color.Target, ERenderTargetLoadAction::ELoad );
    PsParameters->RenderTargets.DepthStencil = FDepthStencilBinding ( SceneTextures.Depth.Target, ERenderTargetLoadAction::ELoad, ERenderTargetLoadAction::ENoAction, FExclusiveDepthStencil::DepthRead_StencilNop );

    const FIntRect Viewport ( View.ViewRect );

    Builder.AddPass ( RDG_EVENT_NAME ( "PlanetFogRenderingPass" ), PsParameters, ERDGPassFlags::Raster,
    [ShaderMap, ViewUniformBuffer, Viewport, FogPS, PsParameters] ( FRHICommandList& CmdList )
    {
        CmdList.SetViewport ( Viewport.Min.X, Viewport.Min.Y, 0.0f, Viewport.Max.X, Viewport.Max.Y, 1.0f );

        FGraphicsPipelineStateInitializer Pipeline;
        CmdList.ApplyCachedRenderTargets ( Pipeline );

        Pipeline.BlendState = TStaticBlendState<EColorWriteMask::CW_RGBA, EBlendOperation::BO_Add, EBlendFactor::BF_One, EBlendFactor::BF_SourceAlpha, EBlendOperation::BO_Add, EBlendFactor::BF_Zero, EBlendFactor::BF_SourceAlpha>::GetRHI ();
        Pipeline.DepthStencilState = TStaticDepthStencilState<false, ECompareFunction::CF_Always>::GetRHI ();
        SetFullscreenPipeline ( CmdList, ShaderMap, Pipeline, FogPS, ViewUniformBuffer );

        SetShaderParameters ( CmdList, FogPS, FogPS.GetPixelShader (), *PsParameters );
        DrawFullscreenQuad ( CmdList );
    } );
}