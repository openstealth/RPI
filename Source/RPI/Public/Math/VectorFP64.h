#pragma once

#include "Math/MathFP64.h"

struct FVectorFP64
{
    static const FVectorFP64 ZeroVector;

    double X, Y, Z;

    FVectorFP64 ( double V )
        : FVectorFP64 ( V, V, V )
    {}

    FVectorFP64 ( double Vx = 0.0, double Vy = 0.0, double Vz = 0.0 )
        : X ( Vx ), Y ( Vy ), Z ( Vz )
    {}

    FVectorFP64 ( const double * V )
        : FVectorFP64 ( V[0], V[1], V[2] )
    {}

    FVectorFP64 ( const FVector& V )
        : FVectorFP64 ( V.X, V.Y, V.Z )
    {}

    FVectorFP64 ( const FVector3f& V )
        : FVectorFP64 ( V.X, V.Y, V.Z )
    {}

    double Size () const
    {
        return sqrt ( X * X + Y * Y + Z * Z );
    }

    double Dot ( const FVectorFP64& V ) const
    {
        return X * V.X + Y * V.Y + Z * V.Z;
    }

    FVectorFP64 Cross ( const FVectorFP64& V ) const
    {
        return FVectorFP64
        (
            Y * V.Z - Z * V.Y,
            Z * V.X - X * V.Z,
            X * V.Y - Y * V.X
        );
    }

    FVectorFP64 GetSafeNormal ( double Tolerance = 1e-8 ) const
    {
        const double SquareSum = X * X + Y * Y + Z * Z;

	    // Not sure if it's safe to add tolerance in there. Might introduce too many errors
        if ( SquareSum == 1.0 )
		    return *this;
        else
        if ( SquareSum < Tolerance )
		    return FVectorFP64 ();

        const double Scale = 1.0 / sqrt ( SquareSum );
        return FVectorFP64 ( X * Scale, Y * Scale, Z * Scale );
    }

    FVector ToVector () const
    {
        return FVector ( X, Y, Z );
    }

    FVectorFP64 operator - () const
    {
        return FVectorFP64 ( -X, -Y, -Z );
    }

    void operator += ( const FVectorFP64& V )
    {
        *this = *this + V;
    }

    void operator -= ( const FVectorFP64& V )
    {
        *this = *this - V;
    }

    void operator *= ( double F )
    {
        *this = *this * F;
    }

    void operator /= ( double F )
    {
        *this = *this / F;
    }

    friend FVectorFP64 operator + ( const FVectorFP64& V, const FVectorFP64& U )
    {
        return FVectorFP64 ( V.X + U.X, V.Y + U.Y, V.Z + U.Z );
    }

    friend FVectorFP64 operator - ( const FVectorFP64& V, const FVectorFP64& U )
    {
        return FVectorFP64 ( V.X - U.X, V.Y - U.Y, V.Z - U.Z );
    }

    friend double operator | ( const FVectorFP64& V, const FVectorFP64& U )
    {
        return V.Dot ( U );
    }

    friend FVectorFP64 operator ^ ( const FVectorFP64& V, const FVectorFP64& U )
    {
        return V.Cross ( U );
    }

    friend FVectorFP64 operator * ( const FVectorFP64& V, double F )
    {
        return FVectorFP64 ( V.X * F, V.Y * F, V.Z * F );
    }

    friend FVectorFP64 operator / ( const FVectorFP64& V, double F )
    {
        return V * ( 1.0 / F );
    }

    operator const double * () const { return &X; }
};

struct FVector4FP64
{
    static const FVector4FP64 ZeroVector;

    double X, Y, Z, W;

    FVector4FP64 ( double Vx = 0.0, double Vy = 0.0, double Vz = 0.0, double Vw = 0.0 )
        : X ( Vx ), Y ( Vy ), Z ( Vz ), W ( Vw )
    {}

    FVector4FP64 ( double V )
        : FVector4FP64 ( V, V, V, V )
    {}

    FVector4FP64 ( const double * V )
        : FVector4FP64 ( V[0], V[1], V[2], V[3] )
    {}

    FVector4FP64 ( const FVectorFP64& V, double Vw = 0.0 )
        : FVector4FP64 ( V.X, V.Y, V.Z, Vw )
    {}

    FVector4FP64 ( const FVector4& V )
        : FVector4FP64 ( V.X, V.Y, V.Z, V.W )
    {}

    FVector4FP64 ( const FPlane& P )
        : FVector4FP64 ( P.X, P.Y, P.Z, P.W )
    {}

    double Size () const
    {
        return sqrt ( X * X + Y * Y + Z * Z + W * W );
    }

    FVector4FP64 operator - () const
    {
        return FVector4FP64 ( -X, -Y, -Z, -W );
    }

    double Dot ( const FVector4FP64& V ) const
    {
        return X * V.X + Y * V.Y + Z * V.Z + W * V.W;
    }

    friend FVector4FP64 operator + ( const FVector4FP64& V, const FVector4FP64& U )
    {
        return FVector4FP64 ( V.X + U.X, V.Y + U.Y, V.Z + U.Z, V.W + U.W );
    }

    friend FVector4FP64 operator - ( const FVector4FP64& V, const FVector4FP64& U )
    {
        return FVector4FP64 ( V.X - U.X, V.Y - U.Y, V.Z - U.Z, V.W - U.W );
    }

    friend double operator | ( const FVector4FP64& V, const FVector4FP64& U )
    {
        return V.Dot ( U );
    }

    friend FVector4FP64 operator * ( const FVector4FP64& V, double F )
    {
        return FVector4FP64 ( V.X * F, V.Y * F, V.Z * F, V.W * F );
    }

    friend FVector4FP64 operator / ( const FVector4FP64& V, double F )
    {
        return V * ( 1.0 / F );
    }

    operator const double * () const { return &X; }
};