#include "VertexDataCreator.h"

#include "Core/Math/MathUtility.h"

TArray<FVertexSimple> VertexDataCreator::CreateConeVertices()
{
	TArray<FVertexSimple> vertices;

	int segments = 36;
	float radius = 1.f;
	float height = 1.f;


	// 원뿔의 바닥
	for (int i = 0; i < segments; ++i)
	{
		float angle = 2.0f * PI * i / segments;
		float nextAngle = 2.0f * PI * (i + 1) / segments;

		float x1 = radius * cos(angle);
		float y1 = radius * sin(angle);
		float x2 = radius * cos(nextAngle);
		float y2 = radius * sin(nextAngle);

		// 바닥 삼각형 (반시계 방향으로 추가)
		vertices.Add({ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
		vertices.Add({ x2, y2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
		vertices.Add({ x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });

		// 옆면 삼각형 (시계 방향으로 추가)
		vertices.Add({ x1, y1, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
		vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
		vertices.Add({ 0.0f, 0.0f, height, 0.0f, 1.0f, 0.0f, 1.0f });
	}
	return vertices;
}

TArray<FVertexSimple> VertexDataCreator::CreateCylinderVertices()
{
	TArray<FVertexSimple> Vertices;
	
	int segments = 36;
	float radius = .03f;
	float height = .5f;


	// 원기둥의 바닥과 윗면
	for (int i = 0; i < segments; ++i)
	{
		float angle = 2.0f * PI * i / segments;
		float nextAngle = 2.0f * PI * (i + 1) / segments;

		float x1 = radius * cos(angle);
		float y1 = radius * sin(angle);
		float x2 = radius * cos(nextAngle);
		float y2 = radius * sin(nextAngle);

		// 바닥 삼각형
		Vertices.Add({ 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
		Vertices.Add({ x2, y2, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });
		Vertices.Add({ x1, y1, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f });

		// 윗면 삼각형
		Vertices.Add({ 0.0f, 0.0f, height, 0.0f, 1.0f, 0.0f, 1.0f });
		Vertices.Add({ x1, y1, height, 0.0f, 1.0f, 0.0f, 1.0f });
		Vertices.Add({ x2, y2, height, 0.0f, 1.0f, 0.0f, 1.0f });

		// 옆면 삼각형 두 개
		Vertices.Add({ x1, y1, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
		Vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
		Vertices.Add({ x1, y1, height, 0.0f, 0.0f, 1.0f, 1.0f });

		Vertices.Add({ x2, y2, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f });
		Vertices.Add({ x2, y2, height, 0.0f, 0.0f, 1.0f, 1.0f });
		Vertices.Add({ x1, y1, height, 0.0f, 0.0f, 1.0f, 1.0f });
	}
	return Vertices;
}

TArray<FVertexSimple> VertexDataCreator::CreateTorusVertices()
{
	TArray<FVertexSimple> Vertices;
	
	float majorStep = 2.0f * PI / majorSegments;
	float minorStep = 2.0f * PI / minorSegments;

	// majorSegments+1 와 minorSegments+1 로 반복 (끝 점을 중복시켜 매끄러운 연결을 위해)
	for (uint32 i = 0; i <= majorSegments; ++i) {
		float theta = i * majorStep;
		float cosTheta = cos(theta);
		float sinTheta = sin(theta);
		// u: 큰 원을 따라 0~1 범위
		float u = static_cast<float>(i) / majorSegments;

		for (uint32 j = 0; j <= minorSegments; ++j) {
			float phi = j * minorStep;
			float cosPhi = cos(phi);
			float sinPhi = sin(phi);
			// v: 튜브 단면을 따라 0~1 범위
			float v = static_cast<float>(j) / minorSegments;

			// 토러스의 방정식
			float x = (majorRadius + minorRadius * cosPhi) * cosTheta;
			float y = (majorRadius + minorRadius * cosPhi) * sinTheta;
			float z = minorRadius * sinPhi;

			// UV를 활용해 자연스러운 색상 생성 (예시)
			float r = u;          // 큰 원을 따라 변화
			float g = v;          // 튜브 단면을 따라 변화
			float b = 1.0f - u;   // 보완적 색상 변화
			float a = 1.0f;       // 불투명

			// uv 빼놓음
			Vertices.Add({x, y, z, r, g, b, a});
		}
	}
	return Vertices;
}

TArray<uint32> VertexDataCreator::CreateTorusIndecies()
{
	TArray<uint32> Indices;
	
	for (uint32 i = 0; i < majorSegments; ++i) {
		for (uint32 j = 0; j < minorSegments; ++j) {
			int current = i * (minorSegments + 1) + j;
			int next = (i + 1) * (minorSegments + 1) + j;

			// 첫 번째 삼각형
			Indices.Add(current);
			Indices.Add(next);
			Indices.Add(current + 1);

			// 두 번째 삼각형
			Indices.Add(current + 1);
			Indices.Add(next);
			Indices.Add(next + 1);
		}
	}
	return Indices;
}