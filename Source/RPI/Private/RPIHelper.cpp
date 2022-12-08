#include "RPIHelper.h"
#include "RPILogger.h"
#include "RPIGameInstance.h"

#include "Async/Async.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

RPI_LOG_CATEGORY_STATIC ( LogRPIHelper );

void FRPIHelper::ExecuteOnGameThread ( TFunction<void ()> Function, FEvent * Signal )
{
    FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady ( Function, TStatId (), nullptr, ENamedThreads::GameThread );

    if ( Signal )
        FTaskGraphInterface::Get ().TriggerEventWhenTaskCompletes ( Signal, Task );
}

void FRPIHelper::ExecuteOnGameThreadAndWait ( TFunction<void ()> Function )
{
    FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady ( Function, TStatId (), nullptr, ENamedThreads::GameThread );
        
    FTaskGraphInterface::Get ().WaitUntilTaskCompletes ( Task );
}

void FRPIHelper::ProcessGameThreadTasks ()
{
    FTaskGraphInterface::Get ().ProcessThreadUntilIdle ( ENamedThreads::GameThread );
}

bool FRPIHelper::IsPropertyChanged ( const FPropertyChangedEvent& Event, const FName& Name )
{
    return Event.Property &&
           ( ( Event.GetPropertyName () == Name ) ||
           ( Event.MemberProperty ? Event.MemberProperty->GetFName () == Name : false ) );
}

void FRPIHelper::ConsoleCommand ( const UObject * WorldContextObject, const FString& Command )
{
    URPIGameInstance * Instance = Cast<URPIGameInstance> ( UGameplayStatics::GetGameInstance ( WorldContextObject ) );
    if ( Instance )
        Instance->ConsoleCommand ( Command );
}

