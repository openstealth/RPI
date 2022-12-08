#include "Renderer/RPISceneRenderer.h"
#include "Renderer/Private/SceneRendering.h"

FRPISceneRendererParameters FRPISceneRenderer::Parameters;

class FRPISceneRendererResources : public FRenderResource
{
public:

    FBufferRHIRef VertexBuffer;
    FVertexDeclarationRHIRef VertexDeclaration;

protected:

    virtual void InitRHI () override
    {
        TResourceArray<FVector2f, VERTEXBUFFER_ALIGNMENT> Vertices;
        Vertices.SetNumUninitialized ( 3 );
        Vertices[0] = FVector2f ( -1.0f, -1.0f );
        Vertices[1] = FVector2f ( 3.0f, -1.0f );
        Vertices[2] = FVector2f ( -1.0f, 3.0f );

        FRHIResourceCreateInfo CreateInfo ( TEXT ( "RPISceneRendererFullscreenVB" ), &Vertices );
        VertexBuffer = RHICreateVertexBuffer ( Vertices.GetResourceDataSize (), BUF_Static, CreateInfo );

        FVertexDeclarationElementList Elements;
        Elements.Add ( FVertexElement ( 0, 0, VET_Float2, 0, sizeof ( FVector2f ) ) );
        VertexDeclaration = PipelineStateCache::GetOrCreateVertexDeclaration ( Elements );
    }

    virtual void ReleaseRHI () override
    {
        VertexBuffer.SafeRelease ();
        VertexDeclaration.SafeRelease ();
    }
};

static TGlobalResource<FRPISceneRendererResources> RPISceneRendererResources;

class FBaseVS : public FGlobalShader
{
public:

    DECLARE_SHADER_TYPE ( FBaseVS, Global );

public:

    FBaseVS () {}

    FBaseVS ( const ShaderMetaType::CompiledShaderInitializerType& Initializer )
        : FGlobalShader ( Initializer )
    {}

    static bool ShouldCompilePermutation ( const FGlobalShaderPermutationParameters& Parameters )
    {
        return IsFeatureLevelSupported ( Parameters.Platform, ERHIFeatureLevel::SM5 );
    }
};

IMPLEMENT_SHADER_TYPE ( , FBaseVS, TEXT ( "/RPI/Private/SceneCommon.usf" ), TEXT ( "BaseVS" ), SF_Vertex );

void FRPISceneRenderer::SetFullscreenPipeline ( FRHICommandList& CmdList, FGlobalShaderMap * ShaderMap, FGraphicsPipelineStateInitializer& Pipeline, const TShaderRef<FShader>& PixelShader, const TUniformBufferRef<FViewUniformShaderParameters>& ViewUniformBuffer )
{
    TShaderMapRef<FBaseVS> VertexShader ( ShaderMap );

    Pipeline.RasterizerState = TStaticRasterizerState<>::GetRHI ();
    Pipeline.BoundShaderState.VertexDeclarationRHI = RPISceneRendererResources.VertexDeclaration;
    Pipeline.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader ();
    Pipeline.BoundShaderState.PixelShaderRHI = PixelShader.GetPixelShader ();
    Pipeline.PrimitiveType = PT_TriangleStrip;
    //SetGraphicsPipelineState ( CmdList, Pipeline, 0, EApplyRendertargetOption::CheckApply, true );
    SetGraphicsPipelineState ( CmdList, Pipeline, 0 );

    SetUniformBufferParameter ( CmdList, VertexShader.GetVertexShader (), VertexShader->GetUniformBufferParameter<FViewUniformShaderParameters> (), ViewUniformBuffer );
}

void FRPISceneRenderer::SetFullscreenPipeline ( const FViewInfo& View, FRHICommandList& CmdList, FGraphicsPipelineStateInitializer& Pipeline, const TShaderRef<FShader>& PixelShader )
{
    SetFullscreenPipeline ( CmdList, View.ShaderMap, Pipeline, PixelShader, View.ViewUniformBuffer );
}

void FRPISceneRenderer::DrawFullscreenQuad ( FRHICommandList& CmdList )
{
    CmdList.SetStreamSource ( 0u, RPISceneRendererResources.VertexBuffer, 0u );
    CmdList.DrawPrimitive ( 0u, 1u, 1u );
}

void FRPISceneRenderer::Render ( const FViewInfo& View, FRDGBuilder& Builder )
{
    RenderFog ( View, Builder );
}