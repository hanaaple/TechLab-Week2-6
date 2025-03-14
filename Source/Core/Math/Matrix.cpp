#include "Matrix.h"
#include "Vector.h"
#include "Plane.h"
#include "Transform.h"


FMatrix::FMatrix()
{
	// identity matrix
	M[0][0] = 1.0f; M[0][1] = 0.0f; M[0][2] = 0.0f; M[0][3] = 0.0f;
	M[1][0] = 0.0f; M[1][1] = 1.0f; M[1][2] = 0.0f; M[1][3] = 0.0f;
	M[2][0] = 0.0f; M[2][1] = 0.0f; M[2][2] = 1.0f; M[2][3] = 0.0f;
	M[3][0] = 0.0f; M[3][1] = 0.0f; M[3][2] = 0.0f; M[3][3] = 1.0f;
}

FMatrix::FMatrix(const FVector4& InX, const FVector4& InY, const FVector4& InZ, const FVector4& InW)
{
	// row major
	M[0][0] = InX.X; M[0][1] = InX.Y; M[0][2] = InX.Z; M[0][3] = InX.W;
	M[1][0] = InY.X; M[1][1] = InY.Y; M[1][2] = InY.Z; M[1][3] = InY.W;
	M[2][0] = InZ.X; M[2][1] = InZ.Y; M[2][2] = InZ.Z; M[2][3] = InZ.W;
	M[3][0] = InW.X; M[3][1] = InW.Y; M[3][2] = InW.Z; M[3][3] = InW.W;
}

FMatrix FMatrix::Identity()
{
	return FMatrix();
}

static constexpr float PIDIV4 = PI / 4.0f;

FMatrix FMatrix::operator+(const FMatrix& Other) const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[i][j] + Other.M[i][j];
		}
	}
	return Result;
}
FMatrix FMatrix::operator+=(const FMatrix& Other)
{
	*this = *this + Other;
	return *this;
}
FMatrix FMatrix::operator-(const FMatrix& Other) const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[i][j] - Other.M[i][j];
		}
	}
	return Result;
}
FMatrix FMatrix::operator-=(const FMatrix& Other)
{
	*this = *this - Other;
	return *this;
}
FMatrix FMatrix::operator*(const FMatrix& Other) const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[i][0] * Other.M[0][j] +
				M[i][1] * Other.M[1][j] +
				M[i][2] * Other.M[2][j] +
				M[i][3] * Other.M[3][j];
		}
	}
	return Result;
}
FMatrix FMatrix::operator*=(const FMatrix& Other)
{
	*this = *this * Other;
	return *this;
}
FVector FMatrix::operator*(const FVector& Other) const
{
	FVector result(
		M[0][0] * Other.X + M[0][1] * Other.Y + M[0][2] * Other.Z + M[0][3],
		M[1][0] * Other.X + M[1][1] * Other.Y + M[1][2] * Other.Z + M[1][3],
		M[2][0] * Other.X + M[2][1] * Other.Y + M[2][2] * Other.Z + M[2][3]
	);
	return result;
}
FVector4 FMatrix::operator*(const FVector4& Other) const
{
	FVector4 result(
		M[0][0] * Other.X + M[0][1] * Other.Y + M[0][2] * Other.Z + M[0][3] * Other.W,
		M[1][0] * Other.X + M[1][1] * Other.Y + M[1][2] * Other.Z + M[1][3] * Other.W,
		M[2][0] * Other.X + M[2][1] * Other.Y + M[2][2] * Other.Z + M[2][3] * Other.W,
		M[3][0] * Other.X + M[3][1] * Other.Y + M[3][2] * Other.Z + M[3][3] * Other.W
	);
	return result;
}
FMatrix FMatrix::operator*(float Other) const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[i][j] * Other;
		}
	}
	return Result;
}
FMatrix FMatrix::operator*=(float Other)
{
	*this = *this * Other;
	return *this;
}
bool FMatrix::operator==(const FMatrix& Other) const
{
	bool bEqual = true;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (M[i][j] != Other.M[i][j])
			{
				bEqual = false;
				break;
			}
		}
	}

	return bEqual;
}
bool FMatrix::operator!=(const FMatrix& Other) const
{
	return !(*this == Other);
}

FMatrix FMatrix::GetTransposed() const
{
	FMatrix Result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			Result.M[i][j] = M[j][i];
		}
	}
	return Result;
}

float FMatrix::Determinant() const
{
	float det = 0.0f;

	for (int i = 0; i < 4; ++i)
	{
		float temp[3][3];
		for (int j = 1; j < 4; ++j)
		{
			int colIndex = 0;
			for (int k = 0; k < 4; ++k)
			{
				if (k == i) continue;
				temp[j-1][colIndex++] = M[j][k];
			}
		}
		det += (i % 2 == 0 ? 1.0f : -1.0f) * M[0][i] * Determinant3x3(temp);
	}
	return det;
}

float FMatrix::Cofactor(int row, int col) const
{
	// 3x3 행렬에서 해당 행과 열을 제거하여 나머지 3x3 행렬을 생성
	float temp[3][3];
	int r = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (i == row) continue;
		int c = 0;
		for (int j = 0; j < 4; ++j)
		{
			if (j == col) continue;
			temp[r][c++] = M[i][j];
		}
		++r;
	}

	return Determinant3x3(temp);
}

FMatrix FMatrix::Inverse() const
{
	const float Det = Determinant();
	if (FMath::Abs(Det) < 1.0e-6f)
	{
		return {};
	}

	FMatrix Result;
	const float* m = &M[0][0];
	const float InvDet = 1.0f / Det;

	Result.M[0][0] = InvDet * (m[5] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[9] * m[15] - m[11] * m[13]) + m[7] * (m[9] * m[14] - m[10] * m[13]));
	Result.M[0][1] = -InvDet * (m[1] * (m[10] * m[15] - m[11] * m[14]) - m[2] * (m[9] * m[15] - m[11] * m[13]) + m[3] * (m[9] * m[14] - m[10] * m[13]));
	Result.M[0][2] = InvDet * (m[1] * (m[6] * m[15] - m[7] * m[14]) - m[2] * (m[5] * m[15] - m[7] * m[13]) + m[3] * (m[5] * m[14] - m[6] * m[13]));
	Result.M[0][3] = -InvDet * (m[1] * (m[6] * m[11] - m[7] * m[10]) - m[2] * (m[5] * m[11] - m[7] * m[9]) + m[3] * (m[5] * m[10] - m[6] * m[9]));

	Result.M[1][0] = -InvDet * (m[4] * (m[10] * m[15] - m[11] * m[14]) - m[6] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[14] - m[10] * m[12]));
	Result.M[1][1] = InvDet * (m[0] * (m[10] * m[15] - m[11] * m[14]) - m[2] * (m[8] * m[15] - m[11] * m[12]) + m[3] * (m[8] * m[14] - m[10] * m[12]));
	Result.M[1][2] = -InvDet * (m[0] * (m[6] * m[15] - m[7] * m[14]) - m[2] * (m[4] * m[15] - m[7] * m[12]) + m[3] * (m[4] * m[14] - m[6] * m[12]));
	Result.M[1][3] = InvDet * (m[0] * (m[6] * m[11] - m[7] * m[10]) - m[2] * (m[4] * m[11] - m[7] * m[8]) + m[3] * (m[4] * m[10] - m[6] * m[8]));

	Result.M[2][0] = InvDet * (m[4] * (m[9] * m[15] - m[11] * m[13]) - m[5] * (m[8] * m[15] - m[11] * m[12]) + m[7] * (m[8] * m[13] - m[9] * m[12]));
	Result.M[2][1] = -InvDet * (m[0] * (m[9] * m[15] - m[11] * m[13]) - m[1] * (m[8] * m[15] - m[11] * m[12]) + m[3] * (m[8] * m[13] - m[9] * m[12]));
	Result.M[2][2] = InvDet * (m[0] * (m[5] * m[15] - m[7] * m[13]) - m[1] * (m[4] * m[15] - m[7] * m[12]) + m[3] * (m[4] * m[13] - m[5] * m[12]));
	Result.M[2][3] = -InvDet * (m[0] * (m[5] * m[11] - m[7] * m[9]) - m[1] * (m[4] * m[11] - m[7] * m[8]) + m[3] * (m[4] * m[9] - m[5] * m[8]));

	Result.M[3][0] = -InvDet * (m[4] * (m[9] * m[14] - m[10] * m[13]) - m[5] * (m[8] * m[14] - m[10] * m[12]) + m[6] * (m[8] * m[13] - m[9] * m[12]));
	Result.M[3][1] = InvDet * (m[0] * (m[9] * m[14] - m[10] * m[13]) - m[1] * (m[8] * m[14] - m[10] * m[12]) + m[2] * (m[8] * m[13] - m[9] * m[12]));
	Result.M[3][2] = -InvDet * (m[0] * (m[5] * m[14] - m[6] * m[13]) - m[1] * (m[4] * m[14] - m[6] * m[12]) + m[2] * (m[4] * m[13] - m[5] * m[12]));
	Result.M[3][3] = InvDet * (m[0] * (m[5] * m[10] - m[6] * m[9]) - m[1] * (m[4] * m[10] - m[6] * m[8]) + m[2] * (m[4] * m[9] - m[5] * m[8]));

	return Result;

	float det = Determinant();
        
	// 행렬식이 0이면 역행렬이 존재하지 않음
	if (det == 0.0f)
	{
		return Result;  // 역행렬이 존재하지 않음
	}
        
	float invDet = 1.0f / det;
	
	// 여인자 행렬(cofactor matrix) 계산 후 수반행렬(adjugate matrix)을 구함
	Result.M[0][0] =  Cofactor(0, 0) * invDet;
	Result.M[0][1] = -Cofactor(0, 1) * invDet;
	Result.M[0][2] =  Cofactor(0, 2) * invDet;
	Result.M[0][3] = -Cofactor(0, 3) * invDet;
        
	Result.M[1][0] = -Cofactor(1, 0) * invDet;
	Result.M[1][1] =  Cofactor(1, 1) * invDet;
	Result.M[1][2] = -Cofactor(1, 2) * invDet;
	Result.M[1][3] =  Cofactor(1, 3) * invDet;
	
	Result.M[2][0] =  Cofactor(2, 0) * invDet;
	Result.M[2][1] = -Cofactor(2, 1) * invDet;
	Result.M[2][2] =  Cofactor(2, 2) * invDet;
	Result.M[2][3] = -Cofactor(2, 3) * invDet;
	
	Result.M[3][0] = -Cofactor(3, 0) * invDet;
	Result.M[3][1] =  Cofactor(3, 1) * invDet;
	Result.M[3][2] = -Cofactor(3, 2) * invDet;
	Result.M[3][3] =  Cofactor(3, 3) * invDet;
	
	return Result;
}

FMatrix FMatrix::Transpose(const FMatrix& Matrix)
{
	FMatrix Result;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			Result.M[i][j] = Matrix.M[j][i]; // ��� ���� �ٲ� ��ġ
		}
	}
	return Result;
}

FMatrix FMatrix::GetTranslateMatrix(float X, float Y, float Z)
{
	FMatrix Result;
	Result.M[3][0] = X;
	Result.M[3][1] = Y;
	Result.M[3][2] = Z;
	return Result;
}

FMatrix FMatrix::GetTranslateMatrix(FVector Translation)
{
	return GetTranslateMatrix(Translation.X, Translation.Y, Translation.Z);
}

FMatrix FMatrix::GetScaleMatrix(float X, float Y, float Z)
{
	FMatrix Result;
	Result.M[0][0] = X;
	Result.M[1][1] = Y;
	Result.M[2][2] = Z;
	return Result;
}

FMatrix FMatrix::GetScaleMatrix(const FVector& InScale)
{
	return GetScaleMatrix(InScale.X, InScale.Y, InScale.Z);
}


// TODO Check
FMatrix FMatrix::GetRotateMatrix(const FVector& InEulerAngle)
{
	// 각도를 라디안으로 변환
	float Yaw = FMath::DegreesToRadians(InEulerAngle.Y);
	float Pitch = FMath::DegreesToRadians(InEulerAngle.X);
	float Roll = FMath::DegreesToRadians(InEulerAngle.Z);

	FMatrix YawMatrix(
		FVector4(FMath::Cos(Yaw), 0.0f, FMath::Sin(Yaw), 0.0f),    // 1행
		FVector4(0.0f, 1.0f, 0.0f, 0.0f),                            // 2행
		FVector4(-FMath::Sin(Yaw), 0.0f, FMath::Cos(Yaw), 0.0f),    // 3행
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)                             // 4행
	);

	FMatrix PitchMatrix(
		FVector4(1.0f, 0.0f, 0.0f, 0.0f),                           // 1행
		FVector4(0.0f, FMath::Cos(Pitch), -FMath::Sin(Pitch), 0.0f), // 2행
		FVector4(0.0f, FMath::Sin(Pitch), FMath::Cos(Pitch), 0.0f),  // 3행
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)                            // 4행
	);

	FMatrix RollMatrix(
		FVector4(FMath::Cos(Roll), FMath::Sin(Roll), 0.0f, 0.0f),     // 1행
		FVector4(-FMath::Sin(Roll), FMath::Cos(Roll), 0.0f, 0.0f),    // 2행
		FVector4(0.0f, 0.0f, 1.0f, 0.0f),                             // 3행
		FVector4(0.0f, 0.0f, 0.0f, 1.0f)                              // 4행
	);

	return RollMatrix * PitchMatrix * YawMatrix;
	// return GetRotateMatrix(FQuat(InRotation));
}

FMatrix FMatrix::GetRotateMatrix(const FQuat& Q)
{
	// 쿼터니언 요소 추출
	FMatrix Result;

	// 사원수에서 회전 행렬로 변환 (Row-major 방식)
	float xx = Q.X * Q.X;
	float yy = Q.Y * Q.Y;
	float zz = Q.Z * Q.Z;
	float xy = Q.X * Q.Y;
	float xz = Q.X * Q.Z;
	float yz = Q.Y * Q.Z;
	float wx = Q.W * Q.X;
	float wy = Q.W * Q.Y;
	float wz = Q.W * Q.Z;

	// Row-major 방식으로 원소 배치
	Result.M[0][0] = 1.0f - 2.0f * (yy + zz);
	Result.M[0][1] = 2.0f * (xy - wz);
	Result.M[0][2] = 2.0f * (xz + wy);
	Result.M[0][3] = 0.0f;

	Result.M[1][0] = 2.0f * (xy + wz);
	Result.M[1][1] = 1.0f - 2.0f * (xx + zz);
	Result.M[1][2] = 2.0f * (yz - wx);
	Result.M[1][3] = 0.0f;

	Result.M[2][0] = 2.0f * (xz - wy);
	Result.M[2][1] = 2.0f * (yz + wx);
	Result.M[2][2] = 1.0f - 2.0f * (xx + yy);
	Result.M[2][3] = 0.0f;

	Result.M[3][0] = 0.0f;
	Result.M[3][1] = 0.0f;
	Result.M[3][2] = 0.0f;
	Result.M[3][3] = 1.0f;

	return Result;
}
/// <summary>
/// 뷰 변환 행렬을 생성합니다.
/// </summary>
/// <param name="EyePosition">카메라의 포지션입니다.</param>
/// <param name="FocusPoint">카메라가 바라보는 곳의 포지션입니다.</param>
/// <param name="UpDirection">카메라의 위쪽 방향입니다.</param>
/// <returns>뷰 변환 행렬을 반환합니다.</returns>
FMatrix FMatrix::LookAtLH(const FVector& EyePosition, const FVector& FocusPoint, const FVector& WorldUp)
{
	FVector Forward = (FocusPoint - EyePosition).GetSafeNormal();
	FVector Right = FVector::CrossProduct(WorldUp, Forward).GetSafeNormal();
	FVector Up = FVector::CrossProduct(Forward, Right).GetSafeNormal();

	// row major
	FMatrix Result = FMatrix(
		FVector4(Right.X, Up.X, Forward.X, 0.0f),
		FVector4(Right.Y, Up.Y, Forward.Y, 0.0f),
		FVector4(Right.Z, Up.Z, Forward.Z, 0.0f),
		FVector4(-Right.Dot(EyePosition), -Up.Dot(EyePosition), -Forward.Dot(EyePosition), 1.0f)
	);

	return Result;
}
FMatrix FMatrix::PerspectiveFovLH(float FieldOfView, float AspectRatio, float NearPlane, float FarPlane)
{
	FMatrix Result;
	float YScale = 1.0f / tan(FieldOfView / 2.0f);
	float XScale = YScale / AspectRatio;
	Result.M[0][0] = XScale;
	Result.M[1][1] = YScale;
	Result.M[2][2] = FarPlane / (FarPlane - NearPlane);
	Result.M[2][3] = 1.0f;
	Result.M[3][2] = -NearPlane * FarPlane / (FarPlane - NearPlane);
	Result.M[3][3] = 0.0f;
	return Result;
}

FMatrix FMatrix::Orthographic(float Left, float Right, float Bottom, float Top, float NearZ, float FarZ) {
	float InvWidth = 1.0f / (Right - Left);
	float InvHeight = 1.0f / (Top - Bottom);
	float InvDepth = 1.0f / (FarZ - NearZ);
	return FMatrix(
		{2.0f * InvWidth, 0.0f, 0.0f, 0.0f},
		{0.0f, 2.0f * InvHeight, 0.0f, 0.0f},
		{0.0f, 0.0f, InvDepth, 0.0f},
		{-(Right + Left) * InvWidth, -(Top + Bottom) * InvHeight, -NearZ * InvDepth, 1.0f}
	);
}

float FMatrix::Determinant2x2(float a, float b, float c, float d)
{
	return a * d - b * c;
}

float FMatrix::Determinant3x3(float Matrix[3][3])
{
	return Matrix[0][0] * Determinant2x2(Matrix[1][1], Matrix[1][2], Matrix[2][1], Matrix[2][2]) -
		   Matrix[0][1] * Determinant2x2(Matrix[1][0], Matrix[1][2], Matrix[2][0], Matrix[2][2]) +
		   Matrix[0][2] * Determinant2x2(Matrix[1][0], Matrix[1][1], Matrix[2][0], Matrix[2][1]);
}

FVector FMatrix::GetTranslation() const
{
	return {M[3][0], M[3][1], M[3][2]};
}

FVector FMatrix::GetScale() const
{
	float X = FVector(M[0][0], M[0][1], M[0][2]).Length();
	float Y = FVector(M[1][0], M[1][1], M[1][2]).Length();
	float Z = FVector(M[2][0], M[2][1], M[2][2]).Length();
	return { X, Y, Z };

	// float X = FVector(M[0][0], M[1][0], M[2][0]).Length();
	// float Y = FVector(M[0][1], M[1][1], M[2][1]).Length();
	// float Z = FVector(M[0][2], M[1][2], M[2][2]).Length();
	// return { X, Y, Z };
	
	//return FVector(M[0][0], M[1][1], M[2][2]);
}

FVector FMatrix::GetEulerRotation() const
{
	FVector EulerAngles;

	EulerAngles.X = FMath::Atan2(M[2][1], M[2][2]);
	EulerAngles.Y = FMath::Atan2(-M[2][0], FMath::Sqrt(FMath::Square(M[2][1]) + FMath::Square(M[2][2])));
	EulerAngles.Z = FMath::Atan2(M[1][0], M[0][0]);

	// 라디안 -> 도로 변환
	EulerAngles *= FMath::RadiansToDegrees(1.0f);

	return EulerAngles;
}

// FQuat FMatrix::GetRotation() const
// {
// 	FQuat Q = FQuat::MakeFromRotationMatrix(*this);
// 	return Q;
// }
//
// FTransform FMatrix::GetTransform() const
// {
// 	return FTransform(GetTranslation(), GetEulerRotation(), GetScale());
// }
