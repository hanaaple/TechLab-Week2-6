#pragma once
#include "Vector.h"
#include "Matrix.h"
#include "Plane.h"

#define TORAD 0.0174532925199432957f

struct FTransform
{
protected:
	FVector Position;
	FVector Rotation; 
	FVector Scale;
	
public:
	FTransform()
		: Position(FVector(0, 0, 0))
		, Rotation(FVector(0, 0, 0))
		, Scale(FVector(1, 1, 1))
	{
	}

	FTransform(FVector InPosition, FVector InRotation, FVector InScale)
		: Position(InPosition)
		, Rotation(InRotation)
		, Scale(InScale)
	{
	}

	// FTransform(FVector InPosition, FQuat InQuat, FVector InScale)
	// 	: Position(InPosition)
	// 	, Rotation(InQuat)
	// 	, Scale(InScale)
	// {
	// }

	FTransform(const FMatrix& matrix)
	{
		Position = matrix.GetTranslation();
		Rotation = matrix.GetEulerRotation();
		Scale = matrix.GetScale();
	}

	inline FMatrix GetViewMatrix() const
	{
		return FMatrix::LookAtLH(Position, Position + GetForward(), GetUp());
	}
	
	inline virtual void SetPosition(const FVector& InPosition)
	{
		Position = InPosition;
	}
	inline virtual void SetPosition(float x, float y, float z)
	{
		Position = {x, y, z};
	}
	inline virtual void SetRotation(const FVector& InRotation)
	{
		Rotation = InRotation;
	}
	//
	// inline virtual void SetRotation(const FQuat& InRotation)
	// {
	// 	Rotation = InRotation;
	// }
	
	inline virtual void SetRotation(float x, float y, float z)
	{
		SetRotation(FVector(x, y, z));
	}
	
	inline void SetScale(FVector InScale)
	{
		Scale = InScale;
	}
	
	inline void AddScale(FVector InScale)
	{
		Scale.X += InScale.X;
		Scale.Y += InScale.Y;
		Scale.Z += InScale.Z;
	}
	inline void SetScale(float x, float y, float z)
	{
		Scale = {x, y, z};
	}
	FVector GetPosition() const
	{
		return Position;
	}
	// FQuat GetRotation() const 
	// {
	// 	return Rotation;
	// }

	FVector GetEulerRotation() const 
	{
		return Rotation;
	}

	FVector GetScale() const
	{
		return Scale;
	}
	
	FMatrix GetMatrix() const 
	{
		// 순서 맞음.
		return FMatrix::GetScaleMatrix(Scale.X, Scale.Y, Scale.Z)
			* FMatrix::GetRotateMatrix(FQuat(Rotation))
			* FMatrix::GetTranslateMatrix(Position.X, Position.Y, Position.Z);
	}

	FVector GetForward() const
	{
		// 쿼터니언을 회전 행렬로 변환
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);

		// 회전 행렬의 첫 번째 열이 Forward 벡터를 나타냄
		FVector Forward = FVector(
			RotationMatrix.M[0][0],
			RotationMatrix.M[1][0],
			RotationMatrix.M[2][0]
		);

		return Forward.GetSafeNormal();
	}

	FVector GetRight() const
	{
		// 쿼터니언을 회전 행렬로 변환
		FMatrix RotationMatrix = FMatrix::GetRotateMatrix(Rotation);

		// 회전 행렬의 두 번째 열이 Right 벡터를 나타냄
		FVector Forward = FVector(
			RotationMatrix.M[0][1],
			RotationMatrix.M[1][1],
			RotationMatrix.M[2][1]
		);
		return Forward.GetSafeNormal();
	}

	FVector GetUp() const{
		return FVector::CrossProduct(GetForward(), GetRight()).GetSafeNormal();

	}

	void Translate(const FVector& InTranslation)
	{
		Position += InTranslation;
	}

	// InRotate는 Degree 단위
	void Rotate(const FVector& InRotation)
	{
		Rotation += InRotation;
	}

	void RotateYaw(float Angle)
	{
		Rotation.Z += Angle;
	}

	void RotatePitch(float Angle)
	{
		Rotation.Y += Angle;
	}

	void RotateRoll(float Angle)
	{
		Rotation.X += Angle;
	}

	FTransform operator*(const FTransform& OtherMatrix) const
	{		
		FTransform Result = GetMatrix() * OtherMatrix.GetMatrix();
		
		return Result;
	}

	FMatrix Inverse() const
	{
		return GetMatrix().Inverse();
	}
};