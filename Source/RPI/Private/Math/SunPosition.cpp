#include "Math/SunPosition.h"
#include "Misc/DateTime.h"

static uint32 GetDaylightSavingTime ( float Longitude )
{
    return 0u;
}

static uint32 GetTimeZone ( float Longitude )
{
    return FMath::Sign ( Longitude ) * FMath::CeilToInt ( Longitude * 24 / 360.0 );
}

static uint32 GetTimeOffset ( float Longitude )
{
    return GetTimeZone ( Longitude ) + GetDaylightSavingTime ( Longitude );
}

static FTransform GetWorldTransform ( float Latitude, float Longitude )
{
    return FTransform ( FRotationMatrix ( FRotator ( Latitude, 0, -Longitude ) ).GetTransposed () );
}

FTransform FSunPosition::Get ( float Latitude, float Longitude, const FDateTime& Time, bool& IsDay, float& SunElevationAngleRadiansOut, float& SunZenithAngleRadiansOut, bool Local )
{
    const float TimeOffset = GetTimeOffset ( Longitude );

    const double LatitudeRad = FMath::DegreesToRadians ( Latitude );

    // Get the julian day (number of days since Jan 1st of the year 4713 BC)
    const double JulianDay = Time.GetJulianDay () + ( Time.GetTimeOfDay ().GetTotalHours () - TimeOffset ) / 24.0;
    const double JulianCentury = ( JulianDay - 2451545.0 ) / 36525.0;

    // Get the sun's mean longitude , referred to the mean equinox of julian date
    const double GeomMeanLongSunDeg = FMath::Fmod ( 280.46646 + JulianCentury * ( 36000.76983 + JulianCentury * 0.0003032 ), 360.0f );
    const double GeomMeanLongSunRad = FMath::DegreesToRadians ( GeomMeanLongSunDeg );

    // Get the sun's mean anomaly
    const double GeomMeanAnomSunDeg = 357.52911 + JulianCentury * ( 35999.05029 - 0.0001537 * JulianCentury );
    const double GeomMeanAnomSunRad = FMath::DegreesToRadians ( GeomMeanAnomSunDeg );

    // Get the earth's orbit eccentricity
    const double EccentEarthOrbit = 0.016708634 - JulianCentury * ( 0.000042037 + 0.0000001267 * JulianCentury );

    // Get the sun's equation of the center
    const double SunEqOfCtr = FMath::Sin ( GeomMeanAnomSunRad ) * ( 1.914602 - JulianCentury * ( 0.004817 + 0.000014 * JulianCentury ) )
        + FMath::Sin ( 2.0 * GeomMeanAnomSunRad ) * ( 0.019993 - 0.000101 * JulianCentury )
        + FMath::Sin ( 3.0 * GeomMeanAnomSunRad ) * 0.000289;

    // Get the sun's true longitude
    const double SunTrueLongDeg = GeomMeanLongSunDeg + SunEqOfCtr;

    // Get the sun's true anomaly
    const double SunTrueAnomDeg = GeomMeanAnomSunDeg + SunEqOfCtr;
    const double SunTrueAnomRad = FMath::DegreesToRadians ( SunTrueAnomDeg );

    // Get the earth's distance from the sun
    const double SunRadVectorAUs = ( 1.000001018 * ( 1.0 - EccentEarthOrbit * EccentEarthOrbit ) ) / ( 1.0 + EccentEarthOrbit * FMath::Cos ( SunTrueAnomRad ) );

    // Get the sun's apparent longitude
    const double SunAppLongDeg = SunTrueLongDeg - 0.00569 - 0.00478 * FMath::Sin ( FMath::DegreesToRadians ( 125.04 - 1934.136 * JulianCentury ) );
    const double SunAppLongRad = FMath::DegreesToRadians ( SunAppLongDeg );

    // Get the earth's mean obliquity of the ecliptic
    const double MeanObliqEclipticDeg = 23.0 + ( 26.0 + ( ( 21.448 - JulianCentury * ( 46.815 + JulianCentury * ( 0.00059 - JulianCentury * 0.001813 ) ) ) ) / 60.0 ) / 60.0;

    // Get the oblique correction
    const double ObliqCorrDeg = MeanObliqEclipticDeg + 0.00256 * FMath::Cos ( FMath::DegreesToRadians ( 125.04 - 1934.136 * JulianCentury ) );
    const double ObliqCorrRad = FMath::DegreesToRadians ( ObliqCorrDeg );

    // Get the sun's right ascension
    const double SunRtAscenRad = FMath::Atan2 ( FMath::Cos ( ObliqCorrRad ) * FMath::Sin ( SunAppLongRad ), FMath::Cos ( SunAppLongRad ) );
    const double SunRtAscenDeg = FMath::RadiansToDegrees ( SunRtAscenRad );

    // Get the sun's declination
    const double SunDeclinRad = FMath::Asin ( FMath::Sin ( ObliqCorrRad ) * FMath::Sin ( SunAppLongRad ) );
    const double SunDeclinDeg = FMath::RadiansToDegrees ( SunDeclinRad );

    const double VarY = FMath::Pow ( FMath::Tan ( ObliqCorrRad / 2.0 ), 2.0 );

    // Get the equation of time
    const double EqOfTimeMinutes = 4.0 * FMath::RadiansToDegrees ( VarY * FMath::Sin ( 2.0 * GeomMeanLongSunRad ) - 2.0 * EccentEarthOrbit * FMath::Sin ( GeomMeanAnomSunRad ) + 4.0 * EccentEarthOrbit * VarY * FMath::Sin ( GeomMeanAnomSunRad ) * FMath::Cos ( 2.0 * GeomMeanLongSunRad ) - 0.5 * VarY * VarY * FMath::Sin ( 4.0 * GeomMeanLongSunRad ) - 1.25 * EccentEarthOrbit * EccentEarthOrbit * FMath::Sin ( 2.0 * GeomMeanAnomSunRad ) );

    // Get the hour angle of the sunrise
    const double HASunriseDeg = FMath::RadiansToDegrees ( FMath::Acos ( FMath::Cos ( FMath::DegreesToRadians ( 90.833 ) ) / ( FMath::Cos ( LatitudeRad ) * FMath::Cos ( SunDeclinRad ) ) - FMath::Tan ( LatitudeRad ) * FMath::Tan ( SunDeclinRad ) ) );
    //	double SunlightDurationMinutes = 8.0 * HASunriseDeg;

    // Get the local time of the sun's rise and set
    const double SolarNoonLST = ( 720.0 - 4.0 * Longitude - EqOfTimeMinutes + TimeOffset * 60.0 ) / 1440.0;
    const double SunriseTimeLST = SolarNoonLST - HASunriseDeg * 4.0 / 1440.0;
    const double SunsetTimeLST = SolarNoonLST + HASunriseDeg * 4.0 / 1440.0;

    const double CurrentTime = FMath::Fmod ( Time.GetTimeOfDay ().GetTotalMinutes (), 1440.0 ) / 1440.0;
    IsDay = ( CurrentTime > SunriseTimeLST * 1.05 ) && ( CurrentTime < SunsetTimeLST / 1.05 );

    // Get the true solar time
    const double TrueSolarTimeMinutes = FMath::Fmod ( Time.GetTimeOfDay ().GetTotalMinutes () + EqOfTimeMinutes + 4.0 * Longitude - 60.0 * TimeOffset, 1440.0 );

    // Get the hour angle of current time
    const double HourAngleDeg = TrueSolarTimeMinutes < 0 ? TrueSolarTimeMinutes / 4.0 + 180 : TrueSolarTimeMinutes / 4.0 - 180.0;
    const double HourAngleRad = FMath::DegreesToRadians ( HourAngleDeg );

    // Get the solar zenith angle
    const double SolarZenithAngleRad = FMath::Acos ( FMath::Sin ( LatitudeRad ) * FMath::Sin ( SunDeclinRad ) + FMath::Cos ( LatitudeRad ) * FMath::Cos ( SunDeclinRad ) * FMath::Cos ( HourAngleRad ) );
    const double SolarZenithAngleDeg = FMath::RadiansToDegrees ( SolarZenithAngleRad );

    // Get the sun elevation
    const double SolarElevationAngleDeg = 90.0 - SolarZenithAngleDeg;
    const double SolarElevationAngleRad = FMath::DegreesToRadians (SolarElevationAngleDeg);
    SunElevationAngleRadiansOut = static_cast<float>(SolarElevationAngleRad);
    SunZenithAngleRadiansOut    = static_cast<float>(SolarZenithAngleRad);
    const double TanOfSolarElevationAngle = FMath::Tan (SolarElevationAngleRad);

    // Get the approximated atmospheric refraction
    double ApproxAtmosphericRefractionDeg = 0.0;
    if ( SolarElevationAngleDeg <= 85.0 )
    {
        if ( SolarElevationAngleDeg > 5.0 )
        {
            ApproxAtmosphericRefractionDeg = 58.1 / TanOfSolarElevationAngle - 0.07 / FMath::Pow ( TanOfSolarElevationAngle, 3 ) + 0.000086 / FMath::Pow ( TanOfSolarElevationAngle, 5 ) / 3600.0;
        }
        else
        {
            if ( SolarElevationAngleDeg > -0.575 )
            {
                ApproxAtmosphericRefractionDeg = 1735.0 + SolarElevationAngleDeg * ( -518.2 + SolarElevationAngleDeg * ( 103.4 + SolarElevationAngleDeg * ( -12.79 + SolarElevationAngleDeg * 0.711 ) ) );
            }
            else
            {
                ApproxAtmosphericRefractionDeg = -20.772 / TanOfSolarElevationAngle;
            }
        }
        ApproxAtmosphericRefractionDeg /= 3600.0;
    }

    // Get the corrected solar elevation
    const double SolarElevationCorrectedForAtmRefractionDeg = SolarElevationAngleDeg + ApproxAtmosphericRefractionDeg;

    // Get the solar azimuth 
    const double Temp = FMath::RadiansToDegrees ( FMath::Acos ( ( ( FMath::Sin ( LatitudeRad ) * FMath::Cos ( SolarZenithAngleRad ) ) - FMath::Sin ( SunDeclinRad ) ) / ( FMath::Cos ( LatitudeRad ) * FMath::Sin ( SolarZenithAngleRad ) ) ) );
    const double SolarAzimuthAngleDegcwfromN = HourAngleDeg > 0.0f ? FMath::Fmod ( Temp + 180.0f, 360.0f ) : FMath::Fmod ( 540.0f - Temp, 360.0f );

    // offset elevation angle to fit with UE coords system
    const float Elevation = 180.0f + SolarElevationAngleDeg;
    //SunElevationAngleRadiansOut = FMath::DegreesToRadians(Elevation);
    const float CorrectedElevation = 180.0f + SolarElevationCorrectedForAtmRefractionDeg;
    const float Azimuth = SolarAzimuthAngleDegcwfromN;
    //const float SolarNoon = FTimespan::FromDays ( SolarNoonLST );
    //const float SunriseTime = FTimespan::FromDays ( SunriseTimeLST );
    //const float SunsetTime = FTimespan::FromDays ( SunsetTimeLST );

    const FTransform Transform ( FRotator ( Elevation, Azimuth, 0.0f ) );    
    return Local ? Transform : Transform * GetWorldTransform ( Latitude, Longitude );
}