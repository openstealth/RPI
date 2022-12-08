#pragma once

#include "Math/VectorFP64.h"
#include "Math/Polar.h"

struct FMatrixFP64
{
    static const FMatrixFP64 Identity;

public:

    double M[4][4];

public:

    FMatrixFP64 ()
    {
        SetRow ( 0, FVector4FP64 ( 1.0, 0.0, 0.0, 0.0 ) );
        SetRow ( 1, FVector4FP64 ( 0.0, 1.0, 0.0, 0.0 ) );
        SetRow ( 2, FVector4FP64 ( 0.0, 0.0, 1.0, 0.0 ) );
        SetRow ( 3, FVector4FP64 ( 0.0, 0.0, 0.0, 1.0 ) );
    }

    FMatrixFP64 ( double M00, double M01, double M02, double M03 ,
                  double M10, double M11, double M12, double M13 ,
                  double M20, double M21, double M22, double M23 ,
                  double M30, double M31, double M32, double M33 )
    {
        M[0][0] = M00; M[0][1] = M01; M[0][2] = M02;  M[0][3] = M03;
        M[1][0] = M10; M[1][1] = M11; M[1][2] = M12;  M[1][3] = M13;
        M[2][0] = M20; M[2][1] = M21; M[2][2] = M22;  M[2][3] = M23;
        M[3][0] = M30; M[3][1] = M31; M[3][2] = M32;  M[3][3] = M33;
    }

    FMatrixFP64 ( double N[4][4] )
    {
        for ( uint32 i = 0; i < 4; i++ )
            for ( uint32 j = 0; j < 4; j++ )
                M[i][j] = N[i][j]; 
    }

    FMatrixFP64 ( const FVector4FP64& R0, const FVector4FP64& R1, const FVector4FP64& R2, const FVector4FP64& R3 )
    {
        SetRow ( 0, R0 );
        SetRow ( 1, R1 );
        SetRow ( 2, R2 );
        SetRow ( 3, R3 );
    }

    FMatrixFP64 ( const FMatrix& N )
    {
        for ( uint32 i = 0; i < 4; i++ )
            for ( uint32 j = 0; j < 4; j++ )
                M[i][j] = N.M[i][j];
    }

    FMatrixFP64 ( const FTransform& T )
        : FMatrixFP64 ( T.ToMatrixWithScale () )
    {}

    FMatrixFP64 Inverse () const
    {
        double Tmp[4][4];
        Tmp[0][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
        Tmp[0][1] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
        Tmp[0][2] = M[1][2] * M[2][3] - M[1][3] * M[2][2];

        Tmp[1][0] = M[2][2] * M[3][3] - M[2][3] * M[3][2];
        Tmp[1][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
        Tmp[1][2] = M[0][2] * M[2][3] - M[0][3] * M[2][2];

        Tmp[2][0] = M[1][2] * M[3][3] - M[1][3] * M[3][2];
        Tmp[2][1] = M[0][2] * M[3][3] - M[0][3] * M[3][2];
        Tmp[2][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

        Tmp[3][0] = M[1][2] * M[2][3] - M[1][3] * M[2][2];
        Tmp[3][1] = M[0][2] * M[2][3] - M[0][3] * M[2][2];
        Tmp[3][2] = M[0][2] * M[1][3] - M[0][3] * M[1][2];

        double Det[4];
        Det[0] = M[1][1] * Tmp[0][0] - M[2][1] * Tmp[0][1] + M[3][1] * Tmp[0][2];
        Det[1] = M[0][1] * Tmp[1][0] - M[2][1] * Tmp[1][1] + M[3][1] * Tmp[1][2];
        Det[2] = M[0][1] * Tmp[2][0] - M[1][1] * Tmp[2][1] + M[3][1] * Tmp[2][2];
        Det[3] = M[0][1] * Tmp[3][0] - M[1][1] * Tmp[3][1] + M[2][1] * Tmp[3][2];

        const double Determinant = M[0][0] * Det[0] - M[1][0] * Det[1] + M[2][0] * Det[2] - M[3][0] * Det[3];

        if ( Determinant == 0.0 )
            return FMatrixFP64 ();

        const double RDet = 1.0 / Determinant;

        double Result[4][4];
        Result[0][0] = RDet * Det[0];
        Result[0][1] = -RDet * Det[1];
        Result[0][2] = RDet * Det[2];
        Result[0][3] = -RDet * Det[3];
        Result[1][0] = -RDet * ( M[1][0] * Tmp[0][0] - M[2][0] * Tmp[0][1] + M[3][0] * Tmp[0][2] );
        Result[1][1] = RDet * ( M[0][0] * Tmp[1][0] - M[2][0] * Tmp[1][1] + M[3][0] * Tmp[1][2] );
        Result[1][2] = -RDet * ( M[0][0] * Tmp[2][0] - M[1][0] * Tmp[2][1] + M[3][0] * Tmp[2][2] );
        Result[1][3] = RDet * ( M[0][0] * Tmp[3][0] - M[1][0] * Tmp[3][1] + M[2][0] * Tmp[3][2] );

        Result[2][0] = RDet * ( M[1][0] * ( M[2][1] * M[3][3] - M[2][3] * M[3][1] ) -
                                M[2][0] * ( M[1][1] * M[3][3] - M[1][3] * M[3][1] ) +
                                M[3][0] * ( M[1][1] * M[2][3] - M[1][3] * M[2][1] ) );

        Result[2][1] = -RDet * ( M[0][0] * ( M[2][1] * M[3][3] - M[2][3] * M[3][1] ) -
                                 M[2][0] * ( M[0][1] * M[3][3] - M[0][3] * M[3][1] ) +
                                 M[3][0] * ( M[0][1] * M[2][3] - M[0][3] * M[2][1] ) );

        Result[2][2] = RDet * ( M[0][0] * ( M[1][1] * M[3][3] - M[1][3] * M[3][1] ) -
                                M[1][0] * ( M[0][1] * M[3][3] - M[0][3] * M[3][1] ) +
                                M[3][0] * ( M[0][1] * M[1][3] - M[0][3] * M[1][1] ) );

        Result[2][3] = -RDet * ( M[0][0] * ( M[1][1] * M[2][3] - M[1][3] * M[2][1] ) -
                                 M[1][0] * ( M[0][1] * M[2][3] - M[0][3] * M[2][1] ) +
                                 M[2][0] * ( M[0][1] * M[1][3] - M[0][3] * M[1][1] ) );

        Result[3][0] = -RDet * ( M[1][0] * ( M[2][1] * M[3][2] - M[2][2] * M[3][1] ) -
                                 M[2][0] * ( M[1][1] * M[3][2] - M[1][2] * M[3][1] ) +
                                 M[3][0] * ( M[1][1] * M[2][2] - M[1][2] * M[2][1] ) );

        Result[3][1] = RDet * ( M[0][0] * ( M[2][1] * M[3][2] - M[2][2] * M[3][1] ) -
                                M[2][0] * ( M[0][1] * M[3][2] - M[0][2] * M[3][1] ) +
                                M[3][0] * ( M[0][1] * M[2][2] - M[0][2] * M[2][1] ) );

        Result[3][2] = -RDet * ( M[0][0] * ( M[1][1] * M[3][2] - M[1][2] * M[3][1] ) -
                                 M[1][0] * ( M[0][1] * M[3][2] - M[0][2] * M[3][1] ) +
                                 M[3][0] * ( M[0][1] * M[1][2] - M[0][2] * M[1][1] ) );

        Result[3][3] = RDet * ( M[0][0] * ( M[1][1] * M[2][2] - M[1][2] * M[2][1] ) -
                                M[1][0] * ( M[0][1] * M[2][2] - M[0][2] * M[2][1] ) +
                                M[2][0] * ( M[0][1] * M[1][2] - M[0][2] * M[1][1] ) );

        return FMatrixFP64 ( Result );
    }

    FMatrixFP64& SetRow ( uint32 Row, const FVector4FP64& V )
    {
        M[Row][0] = V.X;
        M[Row][1] = V.Y;
        M[Row][2] = V.Z;
        M[Row][3] = V.W;
        return *this;
    }

    const FVector4FP64 GetRow ( uint32 Row ) const
    {
        return ( Row < 4 ) ? FVector4FP64 ( M[Row] ) : FVector4FP64 ();
    }

    const FVector4FP64 GetColumn ( uint32 Column ) const
    {
        return ( Column < 4 ) ? FVector4FP64 ( M[0][Column], M[1][Column], M[2][Column], M[3][Column] ) : FVector4FP64 ();
    }

    FVectorFP64 GetScaledAxis ( EAxis::Type InAxis ) const
    {
        switch ( InAxis )
        {
            case EAxis::X:
                return FVectorFP64 ( M[0][0], M[0][1], M[0][2] );

            case EAxis::Y:
                return FVectorFP64 ( M[1][0], M[1][1], M[1][2] );

            case EAxis::Z:
                return FVectorFP64 ( M[2][0], M[2][1], M[2][2] );

            default:
                ensure ( 0 );
                return FVectorFP64 ();
        }
    }

    FMatrixFP64& SetTranslation ( const FVectorFP64& Translation )
    {
        M[3][0] = Translation.X;
        M[3][1] = Translation.Y;
        M[3][2] = Translation.Z;
        return *this;
    }

    FVectorFP64 TransformPosition ( const FVectorFP64& V ) const
    {
        return FVectorFP64 ( TransformFVector4 ( FVector4FP64 ( V, 1.0 ) ) );
    }

    FVectorFP64 InverseTransformPosition ( const FVectorFP64& V ) const
    {
        const FMatrixFP64 InvSelf = Inverse ();
        return InvSelf.TransformPosition ( V );
    }

    FVectorFP64 TransformVector ( const FVectorFP64& V ) const
    {
        return FVectorFP64 ( TransformFVector4 ( V ) );
    }

    FVector4FP64 TransformFVector4 ( const FVector4FP64& V ) const
    {
        return FVector4FP64 ( V.Dot ( GetColumn ( 0 ) ) ,
                              V.Dot ( GetColumn ( 1 ) ) ,
                              V.Dot ( GetColumn ( 2 ) ) ,
                              V.Dot ( GetColumn ( 3 ) ) );
    }

    FRotator Rotator () const
    {
        return ToMatrix ().Rotator ();
        /*const FVectorFP64 XAxis = GetScaledAxis ( EAxis::X );
        const FVectorFP64 YAxis = GetScaledAxis ( EAxis::Y );
        const FVectorFP64 ZAxis = GetScaledAxis ( EAxis::Z );

        FRotator Rotator (
            FMathFP64::Degrees ( atan2 ( XAxis.Z, sqrt ( XAxis.X * XAxis.X + XAxis.Y * XAxis.Y ) ) ),
            FMathFP64::Degrees ( atan2 ( XAxis.Y, XAxis.X ) ),
            0
        );

        const FVectorFP64 SYAxis = FRotationMatrixFP64 ( Rotator ).GetScaledAxis ( EAxis::Y );
        Rotator.Roll = FMathFP64::Degrees ( atan2 ( ZAxis | SYAxis, YAxis | SYAxis ) );
        Rotator.DiagnosticCheckNaN ();
        return Rotator;*/
    }

    FQuat ToQuat () const
    {
        return FQuat ( ToMatrix () );
    }

    FVectorFP64 GetOrigin () const
    {
        return FVectorFP64 ( M[3][0], M[3][1], M[3][2] );
    }

    FMatrixFP64 GetTransposed () const
    {
        FMatrixFP64	Result;

        Result.M[0][0] = M[0][0];
        Result.M[0][1] = M[1][0];
        Result.M[0][2] = M[2][0];
        Result.M[0][3] = M[3][0];

        Result.M[1][0] = M[0][1];
        Result.M[1][1] = M[1][1];
        Result.M[1][2] = M[2][1];
        Result.M[1][3] = M[3][1];

        Result.M[2][0] = M[0][2];
        Result.M[2][1] = M[1][2];
        Result.M[2][2] = M[2][2];
        Result.M[2][3] = M[3][2];

        Result.M[3][0] = M[0][3];
        Result.M[3][1] = M[1][3];
        Result.M[3][2] = M[2][3];
        Result.M[3][3] = M[3][3];

        return Result;
    }

    inline FMatrixFP64 RemoveTranslation () const
    {
        FMatrixFP64 Result = *this;
        Result.M[3][0] = 0.0;
        Result.M[3][1] = 0.0;
        Result.M[3][2] = 0.0;
        return Result;
    }

    FMatrix ToMatrix () const
    {
        return FMatrix ( FPlane ( M[0][0], M[0][1], M[0][2], M[0][3] ) ,
                         FPlane ( M[1][0], M[1][1], M[1][2], M[1][3] ) ,
                         FPlane ( M[2][0], M[2][1], M[2][2], M[2][3] ) ,
                         FPlane ( M[3][0], M[3][1], M[3][2], M[3][3] ) );
    }

    FMatrix44f ToMatrix44f () const
    {
        return FMatrix44f ( FPlane4f ( M[0][0], M[0][1], M[0][2], M[0][3] ) ,
                            FPlane4f ( M[1][0], M[1][1], M[1][2], M[1][3] ) ,
                            FPlane4f ( M[2][0], M[2][1], M[2][2], M[2][3] ) ,
                            FPlane4f ( M[3][0], M[3][1], M[3][2], M[3][3] ) );
    }

    FTransform ToTransform () const
    {
        return FTransform ( ToMatrix () );
    }

    FMatrixFP64 operator * ( const FMatrixFP64& N ) const
    {
        FMatrixFP64 F;
        for ( uint32 i = 0; i < 4; i++ )
            for ( uint32 j = 0; j < 4; j++ )
                F.M[i][j] = M[i][0] * N.M[0][j] + M[i][1] * N.M[1][j] + M[i][2] * N.M[2][j] + M[i][3] * N.M[3][j];
        return F;
    }
};

struct FTranslationMatrixFP64 final : public FMatrixFP64
{
    FTranslationMatrixFP64 ( const FVectorFP64& T )
        : FMatrixFP64 ( 1.0, 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0, 0.0,
                        0.0, 0.0, 1.0, 0.0,
                        T.X, T.Y, T.Z, 1.0 )
    {}
};

struct FRotationTranslationMatrixFP64 : public FMatrixFP64
{
    FRotationTranslationMatrixFP64 ( double Pitch, double Yaw, double Roll, const FVectorFP64& Origin )
    {
        const double SP = sin ( FMathFP64::Radians ( Pitch ) );
        const double CP = cos ( FMathFP64::Radians ( Pitch ) );

        const double SY = sin ( FMathFP64::Radians ( Yaw ) );
        const double CY = cos ( FMathFP64::Radians ( Yaw ) );

        const double SR = sin ( FMathFP64::Radians ( Roll ) );
        const double CR = cos ( FMathFP64::Radians ( Roll ) );

        M[0][0]	= CP * CY;
        M[0][1]	= CP * SY;
        M[0][2]	= SP;
        M[0][3]	= 0.0;

        M[1][0]	= SR * SP * CY - CR * SY;
        M[1][1]	= SR * SP * SY + CR * CY;
        M[1][2]	= -SR * CP;
        M[1][3]	= 0.0;

        M[2][0]	= -( CR * SP * CY + SR * SY );
        M[2][1]	= CY * SR - CR * SP * SY;
        M[2][2]	= CR * CP;
        M[2][3]	= 0.0;

        M[3][0]	= Origin.X;
        M[3][1]	= Origin.Y;
        M[3][2]	= Origin.Z;
        M[3][3]	= 1.0;
    }

    FRotationTranslationMatrixFP64 ( const FRotator& Rotator, const FVectorFP64& Origin )
        : FRotationTranslationMatrixFP64 ( Rotator.Pitch, Rotator.Yaw, Rotator.Roll, Origin )
    {}
};

struct FRotationMatrixFP64 final : public FRotationTranslationMatrixFP64
{
    FRotationMatrixFP64 ( double Pitch, double Yaw, double Roll )
        : FRotationTranslationMatrixFP64 ( Pitch, Yaw, Roll, FVectorFP64::ZeroVector )
    {}

    FRotationMatrixFP64 ( const FRotator& Rotator )
        : FRotationMatrixFP64 ( Rotator.Pitch, Rotator.Yaw, Rotator.Roll )
    {}
};

struct FPolarRotationMatrixFP64 : public FMatrixFP64
{
    FPolarRotationMatrixFP64 ( double Longitude, double Latitude )
    {
        const double SP = sin ( FMathFP64::Radians ( Latitude ) );
        const double CP = cos ( FMathFP64::Radians ( Latitude ) );

        const double SR = sin ( -FMathFP64::Radians ( Longitude ) );
        const double CR = cos ( -FMathFP64::Radians ( Longitude ) );

        M[0][0] =  CP; M[0][1] =  SR * SP; M[0][2] = -CR * SP; M[0][3] = 0.0;
        M[1][0] = 0.0; M[1][1] =       CR; M[1][2] =       SR; M[1][3] = 0.0;
        M[2][0] =  SP; M[2][1] = -SR * CP; M[2][2] =  CR * CP; M[2][3] = 0.0;
        M[3][0] = 0.0; M[3][1] =      0.0; M[3][2] =      0.0; M[3][3] = 1.0;
    }

    FPolarRotationMatrixFP64 ( const FPolar& Polar )
        : FPolarRotationMatrixFP64 ( Polar.Longitude, Polar.Latitude )
    {}
};

struct FPolarRotationTranslationMatrixFP64 final : public FPolarRotationMatrixFP64
{
    FPolarRotationTranslationMatrixFP64 ( const FPolar& Polar )
        : FPolarRotationMatrixFP64 ( Polar )
    {
        SetTranslation ( Polar.Vector () );
    }

    FPolarRotationTranslationMatrixFP64 ( double Latitude, double Longitude, double Radius )
        : FPolarRotationMatrixFP64 ( FPolar ( Longitude, Latitude, Radius ) )
    {}
};

struct FInverseRotationMatrixFP64 final : public FMatrixFP64
{
public:
    
    FInverseRotationMatrixFP64 ( const FRotator& Rot )
        : FMatrixFP64 (
            FMatrixFP64 ( // Yaw            
                FVector4FP64 ( +cos ( FMathFP64::Radians ( Rot.Yaw ) ), -sin ( FMathFP64::Radians ( Rot.Yaw ) ), 0.0, 0.0 ),
                FVector4FP64 ( +sin ( FMathFP64::Radians ( Rot.Yaw ) ), +cos ( FMathFP64::Radians ( Rot.Yaw ) ), 0.0, 0.0 ),
                FVector4FP64 ( 0.0, 0.0, 1.0, 0.0 ),
                FVector4FP64 ( 0.0, 0.0, 0.0, 1.0 ) ) *
            FMatrixFP64 ( // Pitch
                FVector4FP64 ( +cos ( FMathFP64::Radians ( Rot.Pitch ) ), 0.0, -sin ( FMathFP64::Radians ( Rot.Pitch ) ), 0.0 ),
                FVector4FP64 ( 0.0, 1.0, 0.0, 0.0 ),
                FVector4FP64 ( +sin ( FMathFP64::Radians ( Rot.Pitch ) ), 0.0, +cos ( FMathFP64::Radians ( Rot.Pitch ) ), 0.0 ),
                FVector4FP64 ( 0.0, 0.0, 0.0, 1.0 ) ) *
            FMatrixFP64 ( // Roll
                FVector4FP64 ( 1.0, 0.0, 0.0, 0.0 ),
                FVector4FP64 ( 0.0, +cos ( FMathFP64::Radians ( Rot.Roll ) ), +sin ( FMathFP64::Radians ( Rot.Roll ) ), 0.0 ),
                FVector4FP64 ( 0.0, -sin ( FMathFP64::Radians ( Rot.Roll ) ), +cos ( FMathFP64::Radians ( Rot.Roll ) ), 0.0 ),
                FVector4FP64 ( 0.0, 0.0, 0.0, 1.0 ) )
        )
    {}
};

struct FLooAtMatrixFP64 final : public FMatrixFP64
{
    FLooAtMatrixFP64 ( const FVectorFP64& EyePosition, const FVectorFP64& LookAtPosition, const FVectorFP64& UpVector )
    {
        const FVectorFP64 ZAxis = ( LookAtPosition - EyePosition ).GetSafeNormal ();
        const FVectorFP64 XAxis = ( UpVector ^ ZAxis ).GetSafeNormal ();
        const FVectorFP64 YAxis = ZAxis ^ XAxis;

        for ( uint32 i = 0; i < 3; i++ )
        {
            M[i][0] = ( &XAxis.X )[i];
            M[i][1] = ( &YAxis.X )[i];
            M[i][2] = ( &ZAxis.X )[i];
            M[i][3] = 0.0;
        }

        M[3][0] = -EyePosition | XAxis;
        M[3][1] = -EyePosition | YAxis;
        M[3][2] = -EyePosition | ZAxis;
        M[3][3] = 1.0;
    }
};

struct FScaleMatrixFP64 : public FMatrixFP64
{
public:

    FScaleMatrixFP64 ( double Scale )
        : FMatrixFP64 ( FVector4FP64 ( Scale, 0, 0, 0 ),
                        FVector4FP64 ( 0, Scale, 0, 0 ),
                        FVector4FP64 ( 0, 0, Scale, 0 ),
                        FVector4FP64 ( 0, 0, 0, 1 ) )
    {}

    FScaleMatrixFP64 ( const FVectorFP64& Scale )
        : FMatrixFP64 ( FVector4FP64 ( Scale.X, 0, 0, 0 ),
                        FVector4FP64 ( 0, Scale.Y, 0, 0 ),
                        FVector4FP64 ( 0, 0, Scale.Z, 0 ),
                        FVector4FP64 ( 0, 0, 0, 1 ) )
    {}
};

struct FLookAtMatrixFP64 final : public FMatrixFP64
{
    FLookAtMatrixFP64 ( const FVectorFP64& EyePosition, const FVectorFP64& LookAtPosition, const FVectorFP64& UpVector )
    {
        const FVectorFP64 ZAxis = ( LookAtPosition - EyePosition ).GetSafeNormal ();
        const FVectorFP64 XAxis = ( UpVector ^ ZAxis ).GetSafeNormal ();
        const FVectorFP64 YAxis = ZAxis ^ XAxis;

        for ( uint32 RowIndex = 0; RowIndex < 3; RowIndex++ )
        {
            M[RowIndex][0] = ( &XAxis.X )[RowIndex];
            M[RowIndex][1] = ( &YAxis.X )[RowIndex];
            M[RowIndex][2] = ( &ZAxis.X )[RowIndex];
            M[RowIndex][3] = 0.0f;
        }
        M[3][0] = -EyePosition | XAxis;
        M[3][1] = -EyePosition | YAxis;
        M[3][2] = -EyePosition | ZAxis;
        M[3][3] = 1.0f;
    }
};