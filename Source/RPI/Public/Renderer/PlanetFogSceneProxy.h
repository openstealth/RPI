#pragma once

struct FPlanetFogSceneProxy
{
    static constexpr float MaxStartDistanceKm = 10.0f;

    float PlanetRadiusKm = 0.0f;
    float AtmosphereRadiusKm = 0.0f;
    float TopRadiusKm = 0.0f;
    float StartKm = 0.0f;

    float Thickness () const { return TopRadiusKm - PlanetRadiusKm; }

    bool IsValid () const
    {
        return ( TopRadiusKm > PlanetRadiusKm ) && ( StartKm <= MaxStartDistanceKm );
    }
};