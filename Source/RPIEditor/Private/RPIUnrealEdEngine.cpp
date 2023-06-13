#include "RPIUnrealEdEngine.h"
#include "RPIWorldSettings.h"
#include "RPIGameInstance.h"
#include "RPILogger.h"

#include "Actors/Weather.h"

#include "LevelEditorViewport.h"
#include "CameraController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "Atmosphere/AtmosphericFog.h"
#include "Atmosphere/AtmosphericFogComponent.h"
#include "Engine/DirectionalLight.h" 
#include "Components/DirectionalLightComponent.h"
#include "Engine/SkyLight.h"
#include "Engine/WindDirectionalSource.h"
#include "Engine/ExponentialHeightFog.h"
#include "Engine/PostProcessVolume.h"
#include "GameMapsSettings.h"
#include "FileHelpers.h"

RPI_LOG_CATEGORY_STATIC ( LogRPIUnrealEd );

URPIUnrealEdEngine::URPIUnrealEdEngine ( const FObjectInitializer& ObjectInitializer ) : Super ( ObjectInitializer )
{
}

URPIUnrealEdEngine::~URPIUnrealEdEngine ()
{
}

UWorld * URPIUnrealEdEngine::GetEditorWorld ()
{
    return GetEditorWorldContext ().World ();
}

ULevel * URPIUnrealEdEngine::GetCurrentLevel ()
{
    return GetEditorWorld ()->GetCurrentLevel ();
}

template<class Class>
Class * URPIUnrealEdEngine::SpawnDefaultActor ()
{
    AActor * Actor = UGameplayStatics::GetActorOfClass ( GetEditorWorld (), Class::StaticClass () );

    if ( !Actor )
    {
        Actor = AddActor ( GetCurrentLevel (), Class::StaticClass (), FTransform::Identity );
        //LogNotify ( "RPIUnrealEdEngine: %s (%s) has been added to level", *Actor->GetName (), *FRPIHelper::GetClassName<Class> () );
    }

    if ( Actor )
    {
        Actor->SetFolderPath ( "REQUIRED" );
        //LogNotify ( "RPIUnrealEdEngine: %s (%s) has been added to folder 'Required'", *Actor->GetName (), *FRPIHelper::GetClassName<Class> () );
        Actor->SetLockLocation ( true );
    }

    return Cast<Class> ( Actor );
}

template<class Class>
bool URPIUnrealEdEngine::RemoveDefaultActor ()
{
    UWorld * World = GetEditorWorld ();

    if ( World )
    {
        AActor * Actor = UGameplayStatics::GetActorOfClass ( World, Class::StaticClass () );

        if ( Actor )
            return World->DestroyActor ( Actor );
    }
    return false;
}

void URPIUnrealEdEngine::SetupWorld ()
{
    //RemoveDefaultActor<AAtmosphericFog> ();
    RemoveDefaultActor<AExponentialHeightFog> ();
    RemoveDefaultActor<APostProcessVolume> ();

    SpawnDefaultActor<ASkyAtmosphere> ();
    
    ASkyLight * Sky = SpawnDefaultActor<ASkyLight> ();
    USkyLightComponent * SkyLightComponent = Sky ? Sky->GetLightComponent () : nullptr;
    if ( SkyLightComponent )
        SkyLightComponent->bRealTimeCapture = true;
    
    SpawnDefaultActor<AWeather>();

    ADirectionalLight * SunLight = SpawnDefaultActor<ADirectionalLight> ();
    UDirectionalLightComponent * SunLightComponent = SunLight ? static_cast<UDirectionalLightComponent*> ( SunLight->GetLightComponent () ) : nullptr;
    if ( SunLightComponent )
    {
        SunLightComponent->bAtmosphereSunLight = true;
        SunLightComponent->bPerPixelAtmosphereTransmittance = true;
        SunLightComponent->SetMobility ( EComponentMobility::Movable );
    };

    ARPIWorldSettings * WorldSettings = ARPIWorldSettings::Get ( GetEditorWorld () );
    if ( WorldSettings )
        WorldSettings->Initialize ();
}

void URPIUnrealEdEngine::SetupTemplateMap ()
{
    OnGetTemplateMapInfos ().BindLambda ( [] () -> const TArray<FTemplateMapInfo>&
    {
        static const TArray<FTemplateMapInfo> EmptyTemplateMapInfos;
        return EmptyTemplateMapInfos;
    } );
    
    FEditorDelegates::MapChange.AddLambda ( [this] ( uint32 MapChangeFlags )
    {
        if ( MapChangeFlags == MapChangeEventFlags::NewMap || 
             MapChangeFlags == MapChangeEventFlags::Default )
            SetupWorld ();
    } );
}

void URPIUnrealEdEngine::SaveStartupMap ()
{
    SetupWorld ();

    FString StartupMap = GetDefault<UGameMapsSettings> () -> EditorStartupMap.GetLongPackageName ();

    if ( StartupMap.IsEmpty () )
        StartupMap = TEXT ( "/Game/Maps/Empty" );

    StartupMap += FPackageName::GetMapPackageExtension ();

    const FString Path = FPackageName::LongPackageNameToFilename ( StartupMap );        

    UWorld * World = GetEditorWorld ();
    if ( World && !FPackageName::DoesPackageExist ( Path ) )
    {
        if ( FEditorFileUtils::SaveLevel ( World->GetCurrentLevel (), Path ) )
        {
            //FMainFrameActionCallbacks::CookContent ( "WindowsNoEditor" );
        }
    }
}

UGameInstance * URPIUnrealEdEngine::CreateInnerProcessPIEGameInstance ( FRequestPlaySessionParams& InParams, const FGameInstancePIEParameters& InPIEParameters, int32 InPIEInstanceIndex )
{
    float LinearSpeed = 1.0f;
    float AngularSpeed = 1.0f;

    FEditorViewportClient * Viewport = static_cast<FEditorViewportClient*> ( GetActiveViewport ()->GetClient () );   
    if ( Viewport )
    {
        FEditorCameraController * CameraController = Viewport->GetCameraController ();
        float SpeedScale = CameraController ? CameraController->GetConfig ().MovementAccelerationRate : 1.0f;
        LinearSpeed = Viewport->GetCameraSpeed () * Viewport->GetCameraSpeedScalar () * SpeedScale * 1e-3f;
    }

    URPIGameInstance * GameInstance = static_cast<URPIGameInstance*> ( Super::CreateInnerProcessPIEGameInstance ( InParams, InPIEParameters, InPIEInstanceIndex ) );

    if ( GameInstance )
        GameInstance->SetCameraSpeed ( LinearSpeed, LinearSpeed, AngularSpeed );

    return GameInstance;
}

void URPIUnrealEdEngine::Init ( IEngineLoop * InEngineLoop )
{
    Super::Init ( InEngineLoop );

    SaveStartupMap ();
    SetupTemplateMap ();
}

void URPIUnrealEdEngine::Tick ( float DeltaSeconds, bool bIdleMode )
{
    Super::Tick ( DeltaSeconds, bIdleMode );

    URPIGameInstance * RPIGameInstance = URPIGameInstance::Get ( PlayWorld );
    if ( RPIGameInstance )
        RPIGameInstance->Tick ( DeltaSeconds, bIdleMode );
}