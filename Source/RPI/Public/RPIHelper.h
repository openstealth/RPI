#pragma once

#include "Engine/Engine.h"

#define IS_PROPERTY_CHANGED(Event,Class,Member)\
    FRPIHelper::IsPropertyChanged ( Event, GET_MEMBER_NAME_CHECKED ( Class, Member ) )

struct RPI_API FRPIHelper
{
public:

    static void ExecuteOnGameThread ( TFunction<void ()> Function, FEvent * Signal = nullptr );
    static void ExecuteOnGameThreadAndWait ( TFunction<void ()> Function );
    static void ProcessGameThreadTasks ();

    template<class Class>
    static FString GetClassName ()
    {
        UClass * StaticClass = Class::StaticClass ();
        return FString ( StaticClass->GetPrefixCPP () ) + StaticClass->GetName ();
    }

    static bool IsPropertyChanged ( const FPropertyChangedEvent& Event, const FName& Name );
    static void ConsoleCommand ( const UObject* WorldContextObject, const FString& Command );
};