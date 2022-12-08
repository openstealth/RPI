#include "Math/Noise.h"
#include "../ThirdParty/FastNoise/FastNoise.h"

FPerlinNoise::FPerlinNoise ( int32 Seed )
{
    FMath::RandInit ( Seed );

    LUT.AddUninitialized ( LUTSize );

    for ( uint32 i = 0; i < LUTSize; i++ )
    {
        LUT[i].X = FMath::FRand () * 2.0f - 1.0f;
        LUT[i].Y = FMath::FRand () * 2.0f - 1.0f;
        LUT[i].Z = FMath::FRand () * 2.0f - 1.0f;

        LUT[i].Normalize ();
    }

    Folds.AddUninitialized ( Dim );

    for ( uint32 i = 0; i < Dim; i++ )
        Folds[i] = i;

    for ( uint32 i = 0; i < Dim; i++ )
    {
        const uint32 Index = FMath::Rand () % Dim;
        Swap ( Folds[i], Folds[Index] );
    }
}

FPerlinNoise::~FPerlinNoise ()
{
}

int32 FPerlinNoise::Fold ( int32 X, int32 Y, int32 Z ) const
{
    const int32 N = Dim - 1;
    const int32 Fx = Folds[X & N];
    const int32 Fy = Folds[( Fx + Y ) & N];
    return Folds[( Fy + Z ) & N];
}

float FPerlinNoise::Curve ( float T ) const
{
    float Ta = FMath::Abs ( T );

    if ( Ta <= 1.0f )
        return 1.0f + Ta * Ta * ( 2.0f * Ta - 3.0f );
    //return 1 - Ta * Ta * Ta * ( 10 - 15 * Ta + 6 * Ta * Ta );

    return 0.0f;
}

float FPerlinNoise::Omega1 ( int32 X, int32 Y, int32 Z, const FVector& P ) const
{
    float Dot = FVector::DotProduct ( LUT[Fold ( X, Y, Z )], P );
    return Dot * Curve ( P.X ) * Curve ( P.Y ) * Curve ( P.Z );
}

float FPerlinNoise::Noise1 ( const FVector& P ) const
{
    const int32 Px = FMath::FloorToInt ( P.X );
    const int32 Py = FMath::FloorToInt ( P.Y );
    const int32 Pz = FMath::FloorToInt ( P.Z );

    float S = 0.0f;

    for ( int32 X = Px; X <= Px + 1; X++ )
        for ( int32 Y = Py; Y <= Py + 1; Y++ )
            for ( int32 Z = Pz; Z <= Pz + 1; Z++ )
                S += Omega1 ( X, Y, Z, FVector ( P.X - X, P.Y - Y, P.Z - Z ) );

    return S;
}

float FPerlinNoise::CurvePow ( float T )
{
    return 1.0f - FMath::Pow ( T, 10.0f );
}

void FPerlinNoise::Lattice ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, TArray<uint8>& Bytes )
{
#define Voxel(V,X,Y,Z)\
            V[X + (Y) * Width + (Z) * Width * Height]

    const FVector OffsetR = FVector::ZeroVector;
    const FVector OffsetG ( 0.12345f, 0.23456f, 0.34567f );
    const FVector OffsetB ( 0.35792f, 0.46803f, 0.79486f );

    TArray<FVector> V;
    V.AddUninitialized ( Width * Height * Depth );

    const FVector Inverse ( 1.0f / FMath::Max ( Width - 1.0f, 1.0f ),
                            1.0f / FMath::Max ( Height - 1.0f, 1.0f ),
                            1.0f / FMath::Max ( Depth - 1.0f, 1.0f ) );

    // fill the lattice with Noise
    for ( uint32 X = 0; X < Width; X++ )
    {
        for ( uint32 Y = 0; Y < Height; Y++ )
        {
            for ( uint32 Z = 0; Z < Depth; Z++ )
            {
                const FVector P = FVector ( X, Y, Z ) * Inverse * Octaves;

                Voxel ( V, X, Y, Z ) = FVector ( Noise1 ( P + OffsetR ) ,
                                                 Noise1 ( P + OffsetG ) ,
                                                 Noise1 ( P + OffsetB ) );
            }
        }
    }

    // make it periodic by adjuisting border values
    TArray<FVector> U;
    U.AddUninitialized ( Width * Height * Depth );

    for ( uint32 X = 0; X < Width; X++ )
    {
        for ( uint32 Y = 0; Y < Height; Y++ )
        {
            for ( uint32 Z = 0; Z < Depth; Z++ )
            {
                float Dx = CurvePow ( X * Inverse.X );
                float Dy = CurvePow ( Y * Inverse.Y );
                float Dz = CurvePow ( Z * Inverse.Z );

                const FVector P[] =
                {
                    Voxel ( V, X, Y, Z ),
                    Voxel ( V, Width - 1 - X, Y, Z ),
                    Voxel ( V, X, Height - 1 - Y, Z ),
                    Voxel ( V, Width - 1 - X, Height - 1 - Y, Z ),
                    Voxel ( V, X, Y, Depth - 1 - Z ),
                    Voxel ( V, Width - 1 - X, Y, Depth - 1 - Z ),
                    Voxel ( V, X, Height - 1 - Y, Depth - 1 - Z ),
                    Voxel ( V, Width - 1 - X, Height - 1 - Y, Depth - 1 - Z )
                };

                Voxel ( U, X, Y, Z ) = P[0] * Dx * Dy * Dz
                    + P[1] * ( 1 - Dx ) * Dy * Dz
                    + P[2] * Dx * ( 1 - Dy ) * Dz
                    + P[3] * ( 1 - Dx ) * ( 1 - Dy ) * Dz
                    + P[4] * Dx * Dy * ( 1 - Dz )
                    + P[5] * ( 1 - Dx ) * Dy * ( 1 - Dz )
                    + P[6] * Dx * ( 1 - Dy ) * ( 1 - Dz )
                    + P[7] * ( 1 - Dx ) * ( 1 - Dy ) * ( 1 - Dz );
            }
        }
    }

    const uint32 Bpp = 4u;

    Bytes.Reset ();
    Bytes.AddUninitialized ( Width * Height * Depth * Bpp );

    for ( uint32 X = 0; X < Width; X++ )
    {
        for ( uint32 Y = 0; Y < Height; Y++ )
        {
            for ( uint32 Z = 0; Z < Depth; Z++ )
            {
                const FVector N = Voxel ( U, X, Y, Z );

                const uint32 Pixel = X + Y * Width + Z * Width * Height;

                Bytes[Pixel * Bpp + 0] = static_cast<uint8> ( ( N.X + 1.0f ) * 127.5f );
                Bytes[Pixel * Bpp + 1] = static_cast<uint8> ( ( N.Y + 1.0f ) * 127.5f );
                Bytes[Pixel * Bpp + 2] = static_cast<uint8> ( ( N.Z + 1.0f ) * 127.5f );
                Bytes[Pixel * Bpp + 3] = static_cast<uint8> ( 255.0f );                               
            }
        }
    }
}

// NoiseFactory

static void CreateNoise ( FastNoise& Noise, uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes )
{
    Bytes.Reset ();
    Bytes.AddUninitialized ( Width * Height * Depth * 4 );

    for ( uint32 X = 0; X < Width; X++ )
    {
        for ( uint32 Y = 0; Y < Height; Y++ )
        {
            for ( uint32 Z = 0; Z < Depth; Z++ )
            {
                const uint32 Pixel = X + Y * Width + Z * Width * Height;

                Bytes[Pixel * 4 + 0] = static_cast<uint8> ( Noise.GetNoise ( X, Y, Z ) * 255.0f );
                Bytes[Pixel * 4 + 1] = static_cast<uint8> ( Noise.GetNoise ( X, Y, Z ) * 255.0f );
                Bytes[Pixel * 4 + 2] = static_cast<uint8> ( Noise.GetNoise ( X, Y, Z ) * 255.0f );
                Bytes[Pixel * 4 + 3] = static_cast<uint8> ( Noise.GetNoise ( X, Y, Z ) * 255.0f );
            }
        }
    }
}

void FNoiseFactory::Value ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::Value );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::ValueFractal ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, float Lacunarity, float Gain, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::ValueFractal );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    Noise.SetFractalType ( FastNoise::FractalType::FBM );
    Noise.SetFractalOctaves ( Octaves );
    Noise.SetFractalLacunarity ( Lacunarity );
    Noise.SetFractalGain ( Gain );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::Perlin ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::Perlin );    
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::PerlinFractal ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, float Lacunarity, float Gain, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::PerlinFractal );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    Noise.SetFractalType ( FastNoise::FractalType::FBM );
    Noise.SetFractalOctaves ( Octaves );
    Noise.SetFractalLacunarity ( Lacunarity );
    Noise.SetFractalGain ( Gain );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::PerlinLattice ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, TArray<uint8>& Bytes )
{
    FPerlinNoise ().Lattice ( Width, Height, Depth, Octaves, Bytes );
}

void FNoiseFactory::Simplex ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::Simplex );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::SimplexFractal ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, float Lacunarity, float Gain, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::SimplexFractal );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    Noise.SetFractalType ( FastNoise::FractalType::FBM );
    Noise.SetFractalOctaves ( Octaves );
    Noise.SetFractalLacunarity ( Lacunarity );
    Noise.SetFractalGain ( Gain );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::Cellular ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::Cellular );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    Noise.SetCellularReturnType ( FastNoise::CellularReturnType::Distance );
    Noise.SetCellularDistanceFunction ( FastNoise::CellularDistanceFunction::Euclidean );
    //Noise.SetCellularDistance2Indices ( 0, 0 );
    //Noise.SetCellularJitter ( Jitter );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::White ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::WhiteNoise );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::Cubic ( uint32 Width, uint32 Height, uint32 Depth, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::Cubic );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}

void FNoiseFactory::CubicFractal ( uint32 Width, uint32 Height, uint32 Depth, uint32 Octaves, float Lacunarity, float Gain, TArray<uint8>& Bytes )
{
    FastNoise Noise;
    Noise.SetNoiseType ( FastNoise::NoiseType::CubicFractal );
    Noise.SetInterp ( FastNoise::Interp::Quintic );
    Noise.SetFractalType ( FastNoise::FractalType::FBM );
    Noise.SetFractalOctaves ( Octaves );
    Noise.SetFractalLacunarity ( Lacunarity );
    Noise.SetFractalGain ( Gain );
    CreateNoise ( Noise, Width, Height, Depth, Bytes );
}