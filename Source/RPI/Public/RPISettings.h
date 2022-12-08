#pragma once

#include "Math/Polar.h"

struct FViewSettings
{
    uint32 Width = 1280u;
    uint32 Height = 1024u;
    bool Fullscreen = false;
    bool Border = false;
    bool Topmost = false;
    FPolar Position = FPolar::ZeroPolar;
    FRotator Rotation = FRotator::ZeroRotator;
    FVector2D ViewOffset = FVector2D::ZeroVector;
    FVector ProjectionCenter = FVector::ZeroVector;
    float FOV = 90.0f;
    float VerticalFOV = 45.0f;
    bool UseVerticalFOV = false;
};

struct FCommonSettings
{
    bool bShowCursor = false;
    uint32 Month = 7u;
    uint32 Hour = 12u;
    uint32 Minute = 0u;
    float VerticalSpeed = 1.0f;
    float HorizontalSpeed = 1.0f;
    float RotationSpeed = 1.0f;

    FDateTime Time () const
    {
        return FDateTime ( FDateTime::Today ().GetYear (), Month, 15, Hour, Minute );
    }
};

struct FRenderingSettings
{
    FString Api = "D3D12";
    float FogFalloffScale = 1.0f;
    float FogHeightFalloffScale = 2.0f;
};

class FRPISettings
{
public:

    FCommonSettings Common;
    FRenderingSettings Rendering;
    FViewSettings View;

protected:

    FRPISettings ();

public:

    static const FRPISettings& Get ()
    {
        static FRPISettings Settings;
        return Settings;
    }
};