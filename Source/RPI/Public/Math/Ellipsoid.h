#pragma once

#include "Polar.h"
#include "Math/MatrixFP64.h"

#define USE_PLANAR_POSITION 0

class RPI_API FEllipsoid
{
public:

    static const double RadiusEquatorial;
    static const double RadiusEquatorialSq;
    static const double RadiusPolar;
    static const double Flattening;
    static const double FlatteningInv;
    static const double FlatteningInvSq;

protected:

    static FPolar GetPosition ( const FPolar& Origin, double Distance, double Azimuth );
    static double GetDistance ( const FPolar& Origin, const FPolar& Target, double& OriginTargetAzimuth, double& TargetOriginAzimuth );

    static FPolar GeocentricToGeodetic ( const FPolar& Geocentric );
    static FPolar GeodeticToGeocentric ( const FPolar& Geodetic );

    static FVectorFP64 GeodeticToPlanar ( const FPolar& Origin, const FPolar& Geodetic, double Azimuth = 0.0 );
    static FPolar PlanarToGeodetic ( const FPolar& Origin, const FVectorFP64& Point );

public:

    static FPolar MovePosition ( UWorld * World, const FPolar& Geodetic, double Side, double Forward, double Up, double Yaw );
    static double GetSeaLevel ( UWorld * World, const FPolar& Geodetic );
    
    static FPolar PositionToGeodetic ( UWorld * World, const FVectorFP64& Position );
    static FMatrixFP64 PositionToTransform ( UWorld * World, const FVectorFP64& Position );
    
    static FVectorFP64 GeodeticToPosition ( UWorld * World, const FPolar& Geodetic );
    static FMatrixFP64 GeodeticToTransform ( UWorld * World, const FPolar& Geodetic, const FRotator& Rotation = FRotator::ZeroRotator );

    static FPolar PlanarPositionToGeodetic ( UWorld * World, const FVectorFP64& Position );
    static FVectorFP64 GeodeticToPlanarPosition ( UWorld * World, const FPolar& Geodetic );

    static FVectorFP64 GetWorldUp ( UWorld* World, const FPolar& Geodetic);
    static FVectorFP64 GetWorldUp ( UWorld* World, const FVectorFP64& Position );
};