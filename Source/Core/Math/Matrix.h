#pragma once

struct FVector4;
struct FVector;
struct FQuat;

struct alignas(16) FMatrix
{
	float M[4][4];

	FMatrix();
	FMatrix(const FVector4& InX, const FVector4& InY, const FVector4& InZ, const FVector4& InW);

	static FMatrix Identity();
	static FMatrix Transpose(const FMatrix& Matrix);
	static FMatrix GetTranslateMatrix(float X, float Y, float Z);
	static FMatrix GetTranslateMatrix(FVector Translation);
	static FMatrix GetScaleMatrix(float X, float Y, float Z);
	static FMatrix GetScaleMatrix(const FVector& InScale);
	static FMatrix GetRotateMatrix(const FVector& InEulerAngle);
	static FMatrix GetRotateMatrix(const FQuat& Q);
	static FMatrix LookAtLH(const FVector& EyePosition, const FVector& FocusPoint, const FVector& WorldUp);
	static FMatrix PerspectiveFovLH(float FieldOfView, float AspectRatio, float NearPlane, float FarPlane);
	FMatrix Orthographic(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ);
	static float Determinant2x2(float a, float b, float c, float d);
	static float Determinant3x3(float Matrix[3][3]);

	
	FMatrix operator+(const FMatrix& Other) const;
	FMatrix operator+=(const FMatrix& Other);
	FMatrix operator-(const FMatrix& Other) const;
	FMatrix operator-=(const FMatrix& Other);
	FMatrix operator*(const FMatrix& Other) const;
	FMatrix operator*=(const FMatrix& Other);
	FVector operator*(const FVector& Other) const;
	FVector4 operator*(const FVector4& Other) const;
	FMatrix operator*(float Other) const;
	FMatrix operator*=(float Other);
	bool operator==(const FMatrix& Other) const;
	bool operator!=(const FMatrix& Other) const;

	FMatrix GetTransposed() const;
	float Determinant() const;
	float Cofactor(int row, int col) const;
	FMatrix Inverse() const;

	FVector GetTranslation() const;
	FVector GetScale() const;
	FVector GetEulerRotation() const;
	//FQuat GetRotation() const;

	//FVector4 TransformVector4(const FVector4& Vector) const;

	// class FTransform GetTransform() const;
};
