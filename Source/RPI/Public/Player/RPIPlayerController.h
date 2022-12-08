#pragma once

#include "GameFramework/PlayerController.h"
#include "RPIPlayerController.generated.h"

class ARPICamera;
struct FMatrixFP64;
struct FVectorFP64;

UCLASS()
class ARPIPlayerController final : public APlayerController
{
    GENERATED_UCLASS_BODY()

public:

    static ARPIPlayerController * Get ( APlayerController * Controller )
    {
        return Controller ? static_cast<ARPIPlayerController*> ( Controller ) : nullptr;
    }

    static ARPIPlayerController * Get ( UWorld * World )
    {
        return World ? Get ( World->GetFirstPlayerController () ) : nullptr;
    }

    static ARPICamera * GetCamera ( APlayerController * Controller );

    static ARPICamera * GetCamera ( UWorld * World )
    {
        return GetCamera ( Get ( World ) );
    }

    static const FMatrixFP64& GetCameraWorldTransform ( APlayerController * Controller );
    
    static const FMatrixFP64& GetCameraWorldTransform ( UWorld * World )
    {
        return GetCameraWorldTransform ( Get ( World ) );
    }

    static const FVectorFP64& GetCameraReferenceLocation ( APlayerController * Controller );

    static const FVectorFP64& GetCameraReferenceLocation ( UWorld * World )
    {
        return GetCameraReferenceLocation ( Get ( World ) );
    }
};
