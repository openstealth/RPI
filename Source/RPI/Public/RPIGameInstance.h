#pragma once

#include "Engine/GameInstance.h"
#include "RPIGameInstance.generated.h"

class ARPIPawn;
class ARPICamera;
class ARPIPlayerController;
class ARPIWorldSettings;

UCLASS()
class RPI_API URPIGameInstance final : public UGameInstance
{
    GENERATED_BODY()

protected:

    bool bShowCursor = true;

protected:

#if WITH_EDITOR
    virtual FGameInstancePIEResult StartPlayInEditorGameInstance ( ULocalPlayer * LocalPlayer, const FGameInstancePIEParameters& Params ) override;
    void SetupPlayInEditorView ();
#endif
    
    virtual void StartGameInstance () override;
    virtual void Shutdown () override;

    void SetupWorldSettings ();
    void SetupView ();
    void ProcessInput ();
    bool IsKeyDown ( const FKey& Key ) const;

    ARPIPlayerController * GetPlayerController () const;
    ARPICamera * GetCamera () const;
    ARPIWorldSettings * GetWorldSettings () const;

public:

    URPIGameInstance ( const FObjectInitializer& ObjectInitializer );
    virtual ~URPIGameInstance ();

    void SetCameraSpeed ( float HorizontalSpeed, float VerticalSpeed, float RotationSpeed );
    void ConsoleCommand ( const FString& Command ) const;

    void Exit ();
    void Tick ( float DeltaSeconds, bool bIdleMode );

    static URPIGameInstance * Get ( UWorld * World )
    {
        return World ? static_cast<URPIGameInstance*> ( World->GetGameInstance () ) : nullptr;
    }
};