#pragma once
#include "Matrix.h"
#include "Vector.h"

struct FVector4
{
    float X;
    float Y;
    float Z;
    float W;
    
    FVector4(): X(0), Y(0), Z(0), W(0)
    {
        
    }
    FVector4(float InX, float InY, float InZ, float InW)
        : X(InX), Y(InY), Z(InZ), W(InW)
    {
    }

    FVector4(FVector InVector)
    : X(InVector.X), Y(InVector.Y), Z(InVector.Z), W(1.0f)
    {
    }

    FVector4(const FVector4& InVector)
    : X(InVector.X)
    , Y(InVector.Y)
    , Z(InVector.Z)
    , W(InVector.W)
    {
    }

    public:
    static FVector4 Zero() { return {0, 0, 0, 0}; }
    static FVector4 One() { return {1, 1, 1, 1}; }

    static float DotProduct(const FVector4& A, const FVector4& B);
    static FVector4 CrossProduct(const FVector4& A, const FVector4& B);

    static float Distance(const FVector4& V1, const FVector4& V2);

    float Length() const;
    float LengthSquared() const;

    bool Normalize(float Tolerance = 1.e-8f);

    FVector4 GetUnsafeNormal() const;
    FVector4 GetSafeNormal(float Tolerance = 1.e-8f) const;

    float Dot(const FVector4& Other) const;
    FVector4 Cross(const FVector4& Other) const;
    
    FVector4 operator+(const FVector4& Other) const;
    FVector4& operator+=(const FVector4& Other);

    FVector4 operator-(const FVector4& Other) const;
    FVector4& operator-=(const FVector4& Other);

    FVector4 operator*(const FVector4& Other) const;
    FVector4 operator*(float Scalar) const;
    FVector4& operator*=(float Scalar);

    FVector4 operator/(const FVector4& Other) const;
    FVector4 operator/(float Scalar) const;
    FVector4& operator/=(float Scalar);

    FVector4 operator-() const;

    bool operator==(const FVector4& Other) const;
    bool operator!=(const FVector4& Other) const;
    FVector4&& operator*(const FMatrix& matrix) const;
};

inline float FVector4::DotProduct(const FVector4& A, const FVector4& B)
{
    return A.X * B.X + A.Y * B.Y + A.Z * B.Z + A.W * B.W;
}

inline FVector4 FVector4::CrossProduct(const FVector4& A, const FVector4& B)
{
    return {
        A.Y * B.Z - A.Z * B.Y,
        A.Z * B.X - A.X * B.Z,
        A.X * B.Y - A.Y * B.X,
        0.f
    };
}

inline float FVector4::Distance(const FVector4& V1, const FVector4& V2)
{
    return FMath::Sqrt(FMath::Square(V2.X-V1.X) + FMath::Square(V2.Y-V1.Y) + FMath::Square(V2.Z-V1.Z) + FMath::Square(V2.W-V1.W));
}

inline float FVector4::Length() const
{
    return FMath::Sqrt(X*X + Y*Y + Z*Z + W*W);
}

inline float FVector4::LengthSquared() const
{
    return X*X + Y*Y + Z*Z + W*W;
}

inline FVector4 FVector4::GetUnsafeNormal() const
{
    const float Scale = FMath::InvSqrt(LengthSquared());
    
    return FVector4(X, Y, Z, W) * Scale;
}

inline FVector4 FVector4::GetSafeNormal(float Tolerance) const
{
    const float SquareSum = LengthSquared();

    // Not sure if it's safe to add tolerance in there. Might introduce too many errors
    if(SquareSum == 1.f)
    {
        return *this;
    }		
    else if(SquareSum < Tolerance)
    {
        return FVector4();
    }
    const float Scale = FMath::InvSqrt(SquareSum);
    return FVector4(X, Y, Z, W) * Scale;
}

inline bool FVector4::Normalize(float Tolerance)
{
    const float SquareSum = LengthSquared();
    if(SquareSum > Tolerance)
    {
        const float Scale = FMath::InvSqrt(SquareSum);
        X *= Scale; Y *= Scale; Z *= Scale; W *= Scale;
        return true;
    }
    return false;
}

inline float FVector4::Dot(const FVector4& Other) const
{
    return DotProduct(*this, Other);
}

inline FVector4 FVector4::Cross(const FVector4& Other) const
{
    return CrossProduct(*this, Other);
}

inline FVector4 FVector4::operator+(const FVector4& Other) const
{
    return {X + Other.X, Y + Other.Y, Z + Other.Z , W + Other.W};
}

inline FVector4& FVector4::operator+=(const FVector4& Other)
{
    X += Other.X; Y += Other.Y; Z += Other.Z; W += Other.W;
    return *this;
}

inline FVector4 FVector4::operator-(const FVector4& Other) const
{
    return {X - Other.X, Y - Other.Y, Z - Other.Z , W - Other.W};
}

inline FVector4& FVector4::operator-=(const FVector4& Other)
{
    X -= Other.X; Y -= Other.Y; Z -= Other.Z; W -= Other.W;
    return *this;
}

inline FVector4 FVector4::operator*(const FVector4& Other) const
{
    return {X * Other.X, Y * Other.Y, Z * Other.Z , W * Other.W};
}

inline FVector4 FVector4::operator*(float Scalar) const
{
    return {X * Scalar, Y * Scalar, Z * Scalar , W * Scalar};
}

inline FVector4& FVector4::operator*=(float Scalar)
{
    X *= Scalar; Y *= Scalar; Z *= Scalar; W *= Scalar;
    return *this;
}

inline FVector4 FVector4::operator/(const FVector4& Other) const
{
    return {X / Other.X, Y / Other.Y, Z / Other.Z , W / Other.W};
}

inline FVector4 FVector4::operator/(float Scalar) const
{
    return {X / Scalar, Y / Scalar, Z / Scalar , W / Scalar};
}

inline FVector4& FVector4::operator/=(float Scalar)
{
    X /= Scalar; Y /= Scalar; Z /= Scalar; W /= Scalar;
    return *this;
}

inline FVector4 FVector4::operator-() const
{
    return {-X, -Y, -Z, -W};
}

inline bool FVector4::operator==(const FVector4& Other) const
{
    return X == Other.X && Y == Other.Y && Z == Other.Z && W == Other.W;
}

inline bool FVector4::operator!=(const FVector4& Other) const
{
    return !(*this == Other);
}

inline FVector4&& FVector4::operator*(const FMatrix& Matrix) const
{
    return {
        X * Matrix.M[0][0] + Y * Matrix.M[1][0] + Z * Matrix.M[2][0] + W * Matrix.M[3][0],
        X * Matrix.M[0][1] + Y * Matrix.M[1][1] + Z * Matrix.M[2][1] + W * Matrix.M[3][1],
        X * Matrix.M[0][2] + Y * Matrix.M[1][2] + Z * Matrix.M[2][2] + W * Matrix.M[3][2],
        X * Matrix.M[0][3] + Y * Matrix.M[1][3] + Z * Matrix.M[2][3] + W * Matrix.M[3][3]
    };
}

inline FVector::FVector(const FVector4& vector4) : X(vector4.X), Y(vector4.Y), Z(vector4.Z) {}

inline FVector FVector::operator*(const FMatrix& Matrix) const
{
    FVector Result;
    Result.X = X * Matrix.M[0][0] + Y * Matrix.M[1][0] + Z * Matrix.M[2][0] + Matrix.M[3][0];
    Result.Y = X * Matrix.M[0][1] + Y * Matrix.M[1][1] + Z * Matrix.M[2][1] + Matrix.M[3][1];
    Result.Z = X * Matrix.M[0][2] + Y * Matrix.M[1][2] + Z * Matrix.M[2][2] + Matrix.M[3][2];

    return Result;
}

inline FVector FVector::operator*=(const FMatrix& Matrix) const
{
    return (*this) * Matrix;
}