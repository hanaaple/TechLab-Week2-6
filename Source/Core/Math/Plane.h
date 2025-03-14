#pragma once
#include "Vector4.h"

#define PIDIV2 3.141592654/2

struct alignas(16) FQuat : public FVector4{
    using FVector4::X;
    using FVector4::Y;
    using FVector4::Z;
    using FVector4::W;

    FQuat() : FVector4(0.0f, 0.0f, 0.0f, 1.0f) {}
	explicit FQuat(float InX, float InY, float InZ, float InW) : FVector4(InX, InY, InZ, InW) {}
    explicit FQuat(FVector Rotation) : FVector4(EulerToQuaternion(Rotation)) {}
    FQuat(const FVector& Axis, float AngleInDegrees) : FVector4(AxisAngleToQuaternion(Axis, AngleInDegrees)) {}

    FQuat& operator=(const FQuat& other) {
        // Check for self-assignment
        if (this != &other) {
            W = other.W;
            X = other.X;
            Y = other.Y;
            Z = other.Z;
        }
        return *this; // Return the current object to allow chaining assignments
    }

    bool IsNormalized() const {
        float epsilon = 1e-6;
        float magSquared = MagnitudeSquared();
        return FMath::Abs(magSquared - 1.0f) < epsilon; // Check if magnitude is approximately 1
    }

    static FQuat EulerToQuaternion(FVector Euler);
    static FVector QuaternionToEuler(const FQuat& quaternion);
    static FQuat AxisAngleToQuaternion(const FVector& Axis, float AngleInDegrees);

    static FQuat AddQuaternions(const FQuat& q1, const FQuat& q2);
    static FQuat MultiplyQuaternions(const FQuat& q1, const FQuat& q2);
    static FQuat SubtractQuaternions(const FQuat& q1, const FQuat& q2);

    static FQuat MakeFromRotationMatrix(const struct FMatrix& M);
    FVector GetEuler() const { return QuaternionToEuler(*this); }
    FQuat Inverse() const;
    float MagnitudeSquared() const;
};
