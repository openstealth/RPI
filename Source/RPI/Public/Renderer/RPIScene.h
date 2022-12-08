#pragma once

class UWorld;
class FViewInfo;

struct FPlanetFogSceneProxy;
class FRPILightProxy;

class FRPIScene final
{
private:

    FPlanetFogSceneProxy * PlanetFog = nullptr;
    
public:

    FRPIScene () {}
    ~FRPIScene () {}

    static FRPIScene * Get ( UWorld * World );
    static FRPIScene * Get ( const FViewInfo& View );

    void AddPlanetFog ( FPlanetFogSceneProxy * Fog ); 
    void RemovePlanetFog ( FPlanetFogSceneProxy * Fog );
    const FPlanetFogSceneProxy * GetPlanetFog () const { return PlanetFog; }
};