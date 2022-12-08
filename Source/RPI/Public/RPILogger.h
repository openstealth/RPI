#pragma once

#include "Misc/FileHelper.h"

#define RPI_LOG_CATEGORY_STATIC(Category)\
    DEFINE_LOG_CATEGORY_STATIC(Category,Log,All)

#define RPI_LOG_CATEGORY_EXTERN(Category)\
    DECLARE_LOG_CATEGORY_EXTERN(Category, Log, All)
    
#define RPI_LOG_CATEGORY(Category)\
    DEFINE_LOG_CATEGORY(Category)

#if UE_BUILD_SHIPPING
    #define LogMessage( Category, Verbosity, Format, ...)
#else
    #define LogMessage(Category, Verbosity, Format, ...) \
         UE_LOG( Category, Verbosity, TEXT(Format), ##__VA_ARGS__);
#endif

#define LogNotify(Category,Format,...)\
    LogMessage(Category,Log,Format, ##__VA_ARGS__)

#define LogWarning(Category,Format,...)\
    LogMessage(Category,Warning,Format, ##__VA_ARGS__)

#define LogError(Category,Format,...)\
    LogMessage(Category,Error, Format, ##__VA_ARGS__)

class FRPILogger final
{
protected:

    FRPILogger ();
    ~FRPILogger ();

public:

    static FRPILogger& Get ()
    {
        static FRPILogger Logger;
        return Logger;
    }
};