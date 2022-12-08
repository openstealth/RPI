#include "RPIWorldSubsystem.h"
#include "Renderer/RPIScene.h"

URPIWorldSubsystem::URPIWorldSubsystem ()
{
    Scene = new FRPIScene;
}

URPIWorldSubsystem::~URPIWorldSubsystem ()
{
    if ( Scene )
        delete Scene;
}

void URPIWorldSubsystem::Initialize ( FSubsystemCollectionBase& Collection )
{
    Super::Initialize ( Collection );
}

void URPIWorldSubsystem::Deinitialize ()
{
    Super::Deinitialize ();
}

URPIWorldSubsystem * URPIWorldSubsystem::Get ( UWorld * World )
{
    return UWorld::GetSubsystem<URPIWorldSubsystem> ( World );
}

bool URPIWorldSubsystem::IsGameWorld () const
{
    UWorld * World = GetWorld ();
    return World ? ( World->IsPlayInEditor () || World->IsGameWorld () ) : false;
}