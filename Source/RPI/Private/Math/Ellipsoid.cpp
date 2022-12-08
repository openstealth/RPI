#include "Math/Ellipsoid.h"
#include "RPIWorldSettings.h"

#define USE_TRUE_ELLIPSOID 1

#if USE_TRUE_ELLIPSOID

//const double FEllipsoid::RadiusEquatorial = 6378137.0;
const double FEllipsoid::RadiusEquatorial = 6378138.12;
const double FEllipsoid::RadiusEquatorialSq = 40680645877797.1344;// FEllipsoid::RadiusEquatorial * FEllipsoid::RadiusEquatorial;
//const double FEllipsoid::RadiusPolar = 6356752.3142451793;
const double FEllipsoid::RadiusPolar = 6356753.40921733032;
const double FEllipsoid::Flattening = ( fabs ( FEllipsoid::RadiusEquatorial - FEllipsoid::RadiusPolar ) / FEllipsoid::RadiusEquatorial );
const double FEllipsoid::FlatteningInv = 1.0 - FEllipsoid::Flattening;
const double FEllipsoid::FlatteningInvSq = FEllipsoid::FlatteningInv * FEllipsoid::FlatteningInv;

#else

const double FEllipsoid::RadiusEquatorial = 6360e3; // 6378138.12;
const double FEllipsoid::RadiusEquatorialSq = FEllipsoid::RadiusEquatorial * FEllipsoid::RadiusEquatorial;
const double FEllipsoid::RadiusPolar = FEllipsoid::RadiusEquatorial;
const double FEllipsoid::Flattening = 0.0;
const double FEllipsoid::FlatteningInv = 1.0;
const double FEllipsoid::FlatteningInvSq = 1.0;

#endif

// Routine DIRCT1 from FORWRD3D program at:
// ftp://ftp.ngs.noaa.gov/pub/pcsoft/for_inv.3d/Source/forwrd3d.for

FPolar FEllipsoid::GetPosition ( const FPolar& Origin, double Distance, double Azimuth )
{
    if ( fabs ( Distance ) <= 1e-15 )
        return Origin;

#if 1
    double Tangent = FlatteningInv * tan ( Origin.GetLatitudeRadians () );

    const double SinAzimuth = sin ( FMathFP64::Radians ( Azimuth ) );
    const double CosAzimuth = cos ( FMathFP64::Radians ( Azimuth ) );

    double Heading = 0.0;

    if ( CosAzimuth != 0.0 )
        Heading = atan2 ( Tangent, CosAzimuth ) * 2.0;

    const double Cu = 1.0 / sqrt ( Tangent * Tangent + 1.0 );
    const double Su = Tangent * Cu;
    const double Sa = Cu * SinAzimuth;
    const double Ca = 1.0 - Sa * Sa;
    
    double X = sqrt ( ( 1.0 / FlatteningInvSq - 1.0 ) * Ca + 1.0 ) + 1.0;
    X = ( X - 2.0 ) / X;
    
    double C = ( 0.250 * X * X + 1.0 ) / ( 1.0 - X );
    double D = ( 0.375 * X * X - 1.0 ) * X;

    const double AngularDistance = Distance / ( FlatteningInv * RadiusEquatorial * C );

    double Y = AngularDistance;

    double SinY = 0.0;
    double CosY = 0.0;

    double Cz = 0.0;
    double E = 0.0;

    do
    {
        SinY = sin ( Y );
        CosY = cos ( Y );
        Cz = cos ( Heading + Y );
        E = 2.0 * Cz * Cz - 1.0;
        C = Y;

        X = E * CosY;
        Y = ( ( ( 4.0 * SinY * SinY - 3.0 ) * ( 2.0 * E - 1.0 ) * Cz * D / 6.0 + X ) * D * 0.25 - Cz ) * SinY * D + AngularDistance;
    }
    while ( fabs ( Y - C ) > 1e-4 );

    Heading = ( Cu * CosY * CosAzimuth ) - ( Su * SinY );

    const double Latitude = atan2 ( Su * CosY + Cu * SinY * CosAzimuth, FlatteningInv * sqrt ( Sa * Sa + Heading * Heading ) );
       
    C = ( ( 4.0 - 3.0 * Ca ) * Flattening + 4.0 ) * Ca * Flattening * 0.0625;
    D = ( ( E * CosY * C + Cz ) * SinY * C + Y ) * Sa;

    const double Longitude = atan2 ( SinY * SinAzimuth, Cu * CosY - Su * SinY * CosAzimuth )
                           - ( 1.0 - C ) * D * Flattening;
#else
    const double AngularDistance = Distance / RadiusEquatorial;
    const double Bearing = FPolar::Radians ( Azimuth );
    const double Latitude = asin ( sin ( Origin.GetLatitudeRadians () ) * cos ( AngularDistance ) + cos ( Origin.GetLatitudeRadians () ) * sin ( AngularDistance ) * cos ( Azimuth ) );
    const double Longitude = atan2 ( sin ( Bearing ) * sin ( AngularDistance ) * cos ( Origin.GetLatitudeRadians () ), cos ( Bearing ) - sin ( Origin.GetLatitudeRadians () ) * sin ( Latitude ) );
#endif

    return FPolar::FromRadians ( Origin.GetLongitudeRadians () + Longitude, Latitude, Origin.Radius );
}

// Routine INVER1 from INVERS3D program at:
// ftp://ftp.ngs.noaa.gov/pub/pcsoft/for_inv.3d/Source/invers3d.for

double FEllipsoid::GetDistance ( const FPolar& Origin, const FPolar& Target, double& OriginTargetAzimuth, double& TargetOriginAzimuth )
{
    const double DeltaLongitude = Target.GetLongitudeRadians () - Origin.GetLongitudeRadians ();

    double TangentOrigin = FlatteningInv * tan ( Origin.GetLatitudeRadians () );
    double TangentTarget = FlatteningInv * tan ( Target.GetLatitudeRadians () );

    double CuOrigin = 1.0 / sqrt ( TangentOrigin * TangentOrigin + 1.0 );
    double SuOrigin = CuOrigin * TangentOrigin;
    double CuTarget = 1.0 / sqrt ( TangentTarget * TangentTarget + 1.0 );
    double S = CuOrigin * CuTarget;

    double BackwardAzimuth = S * TangentTarget; // Heading 2 -> 1
    double ForwardAzimuth = BackwardAzimuth * TangentOrigin; // Heading 1 -> 2

    double X = DeltaLongitude;

    double SinX = 0.0;
    double CosX = 0.0;
    double C = 0.0;
    double C2A = 0.0;
    double Cy = 0.0;
    double Cz = 0.0;
    double D = 0.0;
    double E = 0.0;
    double Sa = 0.0;
    double Sy = 0.0;
    double Y = 0.0;

    do
    {
        SinX = sin ( X );
        CosX = cos ( X );

        TangentOrigin = CuTarget * SinX;
        TangentTarget = BackwardAzimuth - SuOrigin * CuTarget * CosX;

        Sy = sqrt ( TangentOrigin * TangentOrigin + TangentTarget * TangentTarget );
        Cy = S * CosX + ForwardAzimuth;

        Y = atan2 ( Sy, Cy );

        if ( ( S * SinX == 0.0 ) && ( Sy == 0.0 ) )
            Sa = 1.0;
        else
            Sa = S * SinX / Sy;

        C2A = 1.0 - Sa * Sa;
        Cz = 2.0 * ForwardAzimuth;

        if ( C2A > 0.0 )
            Cz = -Cz / C2A + Cy;

        E = 2.0 * Cz * Cz - 1.0;
        C = ( ( -3.0 * C2A + 4.0 ) * Flattening + 4.0 ) * C2A * Flattening * 0.0625;
        D = X;
        X = ( E * Cy * C + Cz ) * Sy * C + Y;
        X = ( 1.0 - C ) * X * Sa * Flattening + DeltaLongitude;
   }
   while ( fabs ( D - X ) > 1e-8 );

   OriginTargetAzimuth = atan2 ( TangentOrigin, TangentTarget );

   if ( OriginTargetAzimuth < 0.0 )
       OriginTargetAzimuth += 2.0 * FMathFP64::Pi;

   TargetOriginAzimuth = atan2 ( CuOrigin * SinX, ( ( BackwardAzimuth * CosX ) - ( SuOrigin * CuTarget ) ) ) + FMathFP64::Pi;

   if ( TargetOriginAzimuth < 0.0 )
       TargetOriginAzimuth += 2.0 * FMathFP64::Pi;

   X = sqrt ( ( 1.0 / FlatteningInvSq - 1.0 ) * C2A + 1.0 ) + 1.0;
   X = ( X - 2.0 ) / X;
   C = ( 0.250 * X * X + 1.0 ) * RadiusEquatorial * FlatteningInv / ( 1.0 - X );
   D = ( 0.375 * X * X - 1.0 ) * X;

   const double TermA = ( 4.0 * Sy * Sy - 3.0 );
   const double TermB = ( ( 1.0 - 2.0 * E ) * Cz * D / 6.0 - E * Cy );

   return ( ( 0.25 * TermA * TermB * D + Cz ) * Sy * D + Y ) * C;
}

FPolar FEllipsoid::GeocentricToGeodetic ( const FPolar& Geocentric )
{
#if 1
    const double Epsilon = 0.081819221;
    const double OneSecond = 4.848136811e-6;

    const double NorthPole = 90.0 - Geocentric.Latitude;
    const double SouthPole = 90.0 + Geocentric.Latitude;

    FPolar Geodetic;
    Geodetic.Longitude = Geocentric.Longitude;

    if ( NorthPole < OneSecond || SouthPole < OneSecond )
    {
        Geodetic.Radius = Geocentric.Radius - RadiusEquatorial * FlatteningInv;
        Geodetic.Latitude = Geocentric.Latitude;
    }
    else
    {
        const double LatTangent = tan ( Geocentric.GetLatitudeRadians () );
        const double R = FlatteningInv * RadiusEquatorial / sqrt ( LatTangent * LatTangent + FlatteningInvSq );
        
        double MuR = atan2 ( sqrt ( FMath::Max ( RadiusEquatorialSq - R * R, 0.0 ) ), FlatteningInv * R );

        if ( Geocentric.Latitude < 0.0 ) 
            MuR = -MuR;

        const double DeltaLat = MuR - Geocentric.GetLatitudeRadians ();
        const double DeltaR = Geocentric.Radius - R / cos ( Geocentric.GetLatitudeRadians () );
        const double Rho = RadiusEquatorial * ( 1.0 - Epsilon ) / pow ( sqrt ( 1.0 - Epsilon * Epsilon * pow ( sin ( MuR ), 2.0 ) ), 3.0 );
        
        Geodetic.Radius = DeltaR * cos ( DeltaLat );
        Geodetic.Latitude = FMathFP64::Degrees ( MuR - atan2 ( DeltaR * sin ( DeltaLat ), Rho + Geodetic.Radius ) );
    }
    return Geodetic;
#else
    return FPolar ( Geocentric.Longitude, Geocentric.Latitude, Geocentric.Radius - RadiusEquatorial );
#endif
}

FPolar FEllipsoid::GeodeticToGeocentric ( const FPolar& Geodetic )
{
#if 1
    const double Lambda = atan ( FlatteningInvSq * tan ( Geodetic.GetLatitudeRadians () ) );

    const double SinLambda = sin ( Lambda );
    const double CosLambda = cos ( Lambda );

#if USE_TRUE_ELLIPSOID
    double SeaLevel = sqrt ( RadiusEquatorialSq / ( 1.0 + ( 1.0 / FlatteningInvSq - 1.0 ) * SinLambda * SinLambda ) );
#else
    double SeaLevel = RadiusEquatorial;
#endif
    
    const double Py = SeaLevel * SinLambda + Geodetic.Radius * sin ( Geodetic.GetLatitudeRadians () );
    const double Px = SeaLevel * CosLambda + Geodetic.Radius * cos ( Geodetic.GetLatitudeRadians () );

    return FPolar ( Geodetic.Longitude, FMathFP64::Degrees ( atan2 ( Py, Px ) ), SeaLevel + Geodetic.Radius );
#else
    return FPolar ( Geodetic.Longitude, Geodetic.Latitude, Geodetic.Radius + RadiusEquatorial );
#endif
}

FVectorFP64 FEllipsoid::GeodeticToPlanar ( const FPolar& Origin, const FPolar& Geodetic, double Azimuth )
{
    double ForwardAzimuth = 0.0, BackwardAzimuth = 0.0;
    const double Distance = GetDistance ( Origin, Geodetic, ForwardAzimuth, BackwardAzimuth );
    
    ForwardAzimuth -= Azimuth;

    return FVectorFP64 ( Distance * cos ( ForwardAzimuth ), Distance * sin ( ForwardAzimuth ), Geodetic.Radius - Origin.Radius );
}

FPolar FEllipsoid::PlanarToGeodetic ( const FPolar& Origin, const FVectorFP64& Point )
{
    FPolar Geodetic ( Origin );

    if ( fabs ( Point.X ) > 1e-8 || fabs ( Point.Y ) > 1e-8 )
    {
        const double Heading = 0.0;
        const double Azimuth = Heading + atan2 ( Point.Y, Point.X );
        const double Distance = sqrt ( Point.X * Point.X + Point.Y * Point.Y );

        Geodetic = GetPosition ( Origin, Distance, FMathFP64::Degrees ( Azimuth ) );
    }
     
    Geodetic.Radius += Point.Z;
    return Geodetic;
}

FPolar FEllipsoid::MovePosition ( UWorld * World, const FPolar& Geodetic, double Side, double Forward, double Up, double Yaw )
{
    const double MetersPerUnit = 1.0 / ARPIWorldSettings::GetUnitsPerMeter ( World );
    FPolar Target = Geodetic;
    if ( Side != 0.0 || Forward != 0.0 )
        Target = GetPosition ( Geodetic, sqrt ( Side * Side + Forward * Forward ) * MetersPerUnit, Yaw + FMathFP64::Degrees ( atan2 ( Side, Forward ) ) );
    Target.Radius += Up * MetersPerUnit;
    return Target; 
}

double FEllipsoid::GetSeaLevel ( UWorld * World, const FPolar& Geodetic )
{
    const double UnitsPerMeter = ARPIWorldSettings::GetUnitsPerMeter ( World );

#if USE_TRUE_ELLIPSOID
    const double Mu = sin ( atan ( FlatteningInvSq * tan ( Geodetic.GetLatitudeRadians () ) ) );
    return sqrt ( RadiusEquatorialSq / ( 1.0 + ( 1.0 / FlatteningInvSq - 1.0 ) * Mu * Mu ) ) * UnitsPerMeter;
#else
    return RadiusEquatorial * UnitsPerMeter;
#endif
}

#if USE_PLANAR_POSITION

FPolar FEllipsoid::PositionToGeodetic ( UWorld * World, const FVectorFP64& Position )
{
    return PlanarPositionToGeodetic ( World, Position );
}

FVectorFP64 FEllipsoid::GeodeticToPosition ( UWorld * World, const FPolar& Geodetic )
{
    return GeodeticToPlanarPosition ( World, Geodetic );
}

FMatrixFP64 FEllipsoid::GeodeticToTransform ( UWorld * World, const FPolar& Geodetic, const FRotator& Rotation )
{
    const double UnitsPerMeter = ARPIWorldSettings::GetUnitsPerMeter ( World );
    
    // Используется только широта и долгота точки отсчета, так как
    // необходимо, чтобы она лежала на поверхности земной сферы,
    // но не на поверхности сцены
    const FPolar Origin = ARPIWorldSettings::GetWorldOriginAtSeaLevel ( World );
    return FRotationTranslationMatrixFP64 ( Rotation, GeodeticToPlanar ( Origin, Geodetic ) * UnitsPerMeter );
}

#else

FPolar FEllipsoid::PositionToGeodetic ( UWorld * World, const FVectorFP64& Position )
{
    const double MetersPerUnit = 1.0 / ARPIWorldSettings::GetUnitsPerMeter ( World );
    const FVectorFP64 GeoPosition = Position * MetersPerUnit;
    return GeocentricToGeodetic ( FPolar::FromVector ( GeoPosition ) );
}

FVectorFP64 FEllipsoid::GeodeticToPosition ( UWorld * World, const FPolar& Geodetic )
{
    const double UnitsPerMeter = ARPIWorldSettings::GetUnitsPerMeter ( World );
    return GeodeticToGeocentric ( Geodetic ).Vector () * UnitsPerMeter;
}

FMatrixFP64 FEllipsoid::GeodeticToTransform ( UWorld * World, const FPolar& Geodetic, const FRotator& Rotation )
{
    const double UnitsPerMeter = ARPIWorldSettings::GetUnitsPerMeter ( World );
    const FPolar Geocentric = GeodeticToGeocentric ( Geodetic );
    
#if 0
    // Чтобы сориентировать орты базиса, нужно вычислить следующую матрицу:
    FMatrixFP64 GeoMatrix =
        FRotationMatrixFP64 ( FRotator ( -Geocentric.Latitude, 0, 0 ) ) * // сначала вокруг оси Y (то есть тангаж)
        FRotationMatrixFP64 ( FRotator ( 0, 0, Geocentric.Longitude ) ); // затем вокруг оси X (то есть крен)
#else 
    // Эквивалент, но с сменьшим количеством умножений матриц и вызовов тригонометрических функций:
    FPolarRotationMatrixFP64 GeoMatrix ( Geocentric );
#endif

    GeoMatrix.SetTranslation ( Geocentric.Vector () * UnitsPerMeter );
    return FRotationMatrixFP64 ( Rotation ) * GeoMatrix;
}

#endif

FMatrixFP64 FEllipsoid::PositionToTransform ( UWorld * World, const FVectorFP64& Position )
{
    return GeodeticToTransform ( World, PositionToGeodetic ( World, Position ) );
}

FPolar FEllipsoid::PlanarPositionToGeodetic ( UWorld * World, const FVectorFP64& Position )
{
    const double MetersPerUnit = 1.0 / ARPIWorldSettings::GetUnitsPerMeter ( World );
    const FPolar Origin = ARPIWorldSettings::GetWorldOriginAtSeaLevel ( World );
    return PlanarToGeodetic ( Origin, Position * MetersPerUnit );
}

FVectorFP64 FEllipsoid::GeodeticToPlanarPosition ( UWorld * World, const FPolar& Geodetic )
{
    const double UnitsPerMeter = ARPIWorldSettings::GetUnitsPerMeter ( World );
    const FPolar Origin = ARPIWorldSettings::GetWorldOriginAtSeaLevel ( World );
    return GeodeticToPlanar ( Origin, Geodetic ) * UnitsPerMeter;
}

FVectorFP64 FEllipsoid::GetWorldUp(UWorld* World, const FPolar& Geodetic)
{
#if USE_PLANAR_POSITION
    return FVectorFP64(0, 0, 1);
#else
    return GeodeticToPosition(World, Geodetic).ToVector().GetSafeNormal();
#endif
}

FVectorFP64 FEllipsoid::GetWorldUp(UWorld* World, const FVectorFP64& Position)
{
#if USE_PLANAR_POSITION
    return FVectorFP64(0, 0, 1);
#else
    return GetWorldUp( World, PlanarPositionToGeodetic(World, Position));
#endif
}
