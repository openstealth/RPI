#pragma once

class FViewInfo;
class FRDGBuilder;
class FGraphicsPipelineStateInitializer;
class FShader;

struct FFogRendererParameters
{
    float FalloffScale = 1.0f;
    float HeightFalloffScale = 2.0f;
};

struct FRPISceneRendererParameters
{
    FFogRendererParameters FogRendererParameters;
};

class FRPISceneRenderer final
{
public:

    static FRPISceneRendererParameters Parameters;

protected:

    static void SetFullscreenPipeline ( FRHICommandList& CmdList, FGlobalShaderMap * ShaderMap, FGraphicsPipelineStateInitializer& Pipeline, const TShaderRef<FShader>& PixelShader, const TUniformBufferRef<FViewUniformShaderParameters>& ViewUniformBuffer );
    static void SetFullscreenPipeline ( const FViewInfo& View, FRHICommandList& CmdList, FGraphicsPipelineStateInitializer& Pipeline, const TShaderRef<FShader>& PixelShader );
    static void DrawFullscreenQuad ( FRHICommandList& CmdList );
    
    static void RenderFog ( const FViewInfo& View, FRDGBuilder& Builder );

public:

    static void Render ( const FViewInfo& View, FRDGBuilder& Builder );
};