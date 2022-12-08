#pragma once

#include "Editor/UnrealEdEngine.h"
#include "RPIUnrealEdEngine.generated.h"

UCLASS()
class RPIEDITOR_API URPIUnrealEdEngine final : public UUnrealEdEngine
{
	GENERATED_BODY()

protected:

    UWorld * GetEditorWorld ();
    ULevel * GetCurrentLevel ();

    template<class Class>
    Class * SpawnDefaultActor ();

    template<class Class>
    bool RemoveDefaultActor ();

    void SetupWorld ();
    void SetupTemplateMap ();
    void SaveStartupMap ();

    virtual UGameInstance * CreateInnerProcessPIEGameInstance ( FRequestPlaySessionParams& InParams, const FGameInstancePIEParameters& InPIEParameters, int32 InPIEInstanceIndex ) override;

public:

    URPIUnrealEdEngine ( const FObjectInitializer& ObjectInitializer );
    virtual ~URPIUnrealEdEngine ();

    virtual void Init ( IEngineLoop * InEngineLoop ) override;
    virtual void Tick ( float DeltaSeconds, bool bIdleMode ) override;
};