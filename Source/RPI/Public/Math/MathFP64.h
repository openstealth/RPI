#pragma once

#include "HAL/PlatformCrt.h"

struct FMathFP64
{
public:

    static constexpr double Pi = 3.1415926535897932384626433832795;
    static constexpr double TwoPi = Pi * 2.0;
    static constexpr double FourPi = Pi * 4.0;
    static constexpr double HalfPi = Pi * 0.5;

public:

    static double Radians ( double Angle )
    {
        static constexpr double DegreesToRadians = Pi / 180.0;
        return DegreesToRadians * Angle;
    }

    static double Degrees ( double Angle )
    {
        static constexpr double RadiansToDegrees = 180.0 / Pi;
        return RadiansToDegrees * Angle;
    }
};