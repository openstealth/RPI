#pragma once

#include "Math/VectorFP64.h"

struct FPolar
{
    static const FPolar ZeroPolar;

public:

    double Longitude = 0.0;
    double Latitude = 0.0;
    double Radius = 0.0;

public:

    FPolar ( double InLongitude = 0.0, double InLatitude = 0.0, double InRadius = 0.0 )
        : Longitude ( InLongitude )
        , Latitude ( InLatitude )
        , Radius ( InRadius )
    {}

    FPolar ( const FPolar& Polar )
        : FPolar ( Polar.Longitude, Polar.Latitude, Polar.Radius )
    {}

    FPolar ( const FPolar& Polar, double InRadius )
        : FPolar ( Polar.Longitude, Polar.Latitude, InRadius )
    {}

    FPolar& Set ( double InLongitude, double InLatitude, double InRadius )
    {
        Longitude = InLongitude;
        Latitude = InLatitude;
        Radius = InRadius;
        return *this;
    }

    FPolar& Clamp ()
    {
        Longitude = FMath::Clamp ( Longitude, -180.0, 180.0 );
        Latitude = FMath::Clamp ( Latitude, -90.0, 90.0 );
        return *this;
    }

    FPolar& Floor ()
    {
        Longitude = FMath::FloorToInt ( Longitude );
        Latitude = FMath::FloorToInt ( Latitude );
        return *this;
    }

    FPolar& Ceil ()
    {
        Longitude = FMath::CeilToInt ( Longitude );
        Latitude = FMath::CeilToInt ( Latitude );
        return *this;
    }

    FPolar& Zero ()
    {
        Longitude = 0.0;
        Latitude = 0.0;
        Radius = 0.0;
        return *this;
    }

    FVectorFP64 Vector () const
    {
        const double LonRad = GetLongitudeRadians ();
        const double LatRad = GetLatitudeRadians ();
        const double RCosLat = cos ( LatRad ) * Radius;
        return FVectorFP64 ( sin ( LatRad ) * Radius,
                             sin ( LonRad ) * RCosLat ,
                             cos ( LonRad ) * RCosLat );
    }

    double GetLongitudeRadians () const { return FMathFP64::Radians ( Longitude ); }
    double GetLatitudeRadians () const { return FMathFP64::Radians ( Latitude ); }

    FPolar& operator = ( const FPolar& Polar )
    {
        Longitude = Polar.Longitude;
        Latitude = Polar.Latitude;
        Radius = Polar.Radius;
        return *this;
    }

    bool operator == ( const FPolar& Polar ) const
    {
        return ( Longitude == Polar.Longitude ) &&
            ( Latitude == Polar.Latitude ) &&
            ( Radius == Polar.Radius );
    }

    bool operator != ( const FPolar& Polar ) const
    {
        return ( Longitude != Polar.Longitude ) ||
            ( Latitude != Polar.Latitude ) ||
            ( Radius != Polar.Radius );
    }

    FPolar& operator += ( const FPolar& Polar )
    {
        Longitude += Polar.Longitude;
        Latitude += Polar.Latitude;
        Radius += Polar.Radius;
        return *this;
    }

    FPolar& operator -= ( const FPolar& Polar )
    {
        Longitude -= Polar.Longitude;
        Latitude -= Polar.Latitude;
        Radius -= Polar.Radius;
        return *this;
    }

    bool IsZero () const
    {
        return Longitude == 0.0 && Latitude == 0.0 && Radius == 0.0;
    }

    static FPolar FromRadians ( double Longitude, double Latitude, double Radius )
    {
        return FPolar ( FMathFP64::Degrees ( Longitude ), FMathFP64::Degrees ( Latitude ), Radius );
    }

    static FPolar FromVector ( const FVectorFP64& V )
    {
        const double Longitude = atan2 ( V.Y, V.Z );
        const double Latitude = FMathFP64::HalfPi - atan2 ( sqrt ( V.Z * V.Z + V.Y * V.Y ), V.X );
        const double Radius = V.Size ();
        return FromRadians ( Longitude, Latitude, Radius );
    }
};

inline
FPolar operator - ( const FPolar& A, const FPolar& B )
{
    return FPolar ( A.Longitude - B.Longitude, A.Latitude - B.Latitude, A.Radius - B.Radius );
}

inline
FPolar operator + ( const FPolar& A, const FPolar& B )
{
    return FPolar ( A.Longitude + B.Longitude, A.Latitude + B.Latitude, A.Radius + B.Radius );
}

inline
FPolar operator * ( const FPolar& A, double F )
{
    return FPolar ( A.Longitude * F, A.Latitude * F, A.Radius * F );
}

inline
FPolar operator * ( double F, const FPolar& A )
{
    return FPolar ( A.Longitude * F, A.Latitude * F, A.Radius * F );
}

inline
FPolar operator / ( const FPolar& A, double F )
{
    const double Frcp = ( ( F == 0.0 ) ? 1.0 : 1.0 / F );
    return FPolar ( A.Longitude * Frcp, A.Latitude * Frcp, A.Radius * Frcp );
}