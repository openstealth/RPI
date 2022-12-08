#pragma once

#include "Math/Polar.h"
#include "Math/MatrixFP64.h"
#include "RPIPawn.generated.h"

UCLASS()
class RPI_API ARPIPawn : public APawn
{
    GENERATED_BODY()

protected:

    FPolar Position;
    FRotator Rotation;
    FMatrixFP64 WorldTransform;

protected:

    static FMatrixFP64 GeodeticToTransform ( UWorld * World, const FPolar& Position, const FRotator& Rotation = FRotator::ZeroRotator );

    void UpdateWorldTransform ();
    virtual void UpdateTransform ();
    
    virtual bool IsKeyDown ( const FKey& Key ) const;
    virtual APlayerController * GetPlayerController () const;

public:

    ARPIPawn ( const FObjectInitializer& ObjectInitializer );
    virtual ~ARPIPawn ();

    const FPolar& GetPosition () const { return Position; }
    const FRotator& GetRotation () const { return Rotation; }
    const FMatrixFP64& GetWorldTransform () const { return WorldTransform; }
    const FVector GetForwardVector () const { return WorldTransform.Rotator ().Vector (); }
    
    virtual void Move ( float Forward, float Side, float Up );

    virtual void SetPosition ( const FPolar& InPosition );
    virtual void SetPosition ( const FVectorFP64& InPosition );

    virtual void SetRotation ( float Yaw, float Pitch, float Roll );
    virtual void SetRotation ( const FRotator& InRotation );

    virtual void SetTransform ( const FPolar& Position, const FRotator& Rotation ); 

    virtual void BeginPlay () override;
};