#pragma once

struct FSunPosition
{
    static FTransform Get ( float Latitude, float Longitude, const FDateTime& Time, bool& IsDay, float& SunElevationAngleRadiansOut, float& SunZenithAngleRadiansOut, bool Local = false );
};