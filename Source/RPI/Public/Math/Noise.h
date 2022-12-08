#pragma once

#include "Math/Vector.h"

class FPerlinNoise
{
protected:

    static const uint32 Dim = 128u;
    static const uint32 LUTSize = Dim * Dim * Dim;

    TArray<int32> Folds;
    TArray<FVector> LUT;

protected:

    int32 Fold ( int32 X, int32 Y, int32 Z ) const;
    float Curve ( float T ) const;
    float Omega1 ( int32 X, int32 Y, int32 Z, const FVector& P ) const;
    float Noise1 ( const FVector& P ) const;
    float CurvePow ( float T );

public:

    FPerlinNoise ( int32 Seed = -1 );
    ~FPerlinNoise ();

    void Lattice ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, TArray<uint8>& Bytes );
};

class FNoiseFactory
{
public:

    static void Value ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes );
    static void ValueFractal ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, float Lacunarity, float Gain, TArray<uint8>& Bytes );
    static void Perlin ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes );
    static void PerlinFractal ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, float Lacunarity, float Gain, TArray<uint8>& Bytes );
    static void PerlinLattice ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, TArray<uint8>& Bytes );
    static void Simplex ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes );
    static void SimplexFractal ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, float Lacunarity, float Gain, TArray<uint8>& Bytes );
    static void Cellular ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes );
    static void White ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes );
    static void Cubic ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes );
    static void CubicFractal ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, float Lacunarity, float Gain, TArray<uint8>& Bytes );
};