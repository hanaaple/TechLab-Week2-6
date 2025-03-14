#pragma once
#include "Core/Container/Array.h"
#include "Core/HAL/PlatformType.h"
struct FVertexSimple
{
    float X, Y, Z;    // Position
    float R, G, B, A; // Color
};

enum class EPrimitiveMeshType : uint8
{
	EPT_None,
	EPT_Triangle,
	EPT_Cube,
	EPT_Sphere,
	EPT_Line,
	EPT_BoundingBox,
	EPT_GridLine,
	EPT_Cylinder,
	EPT_Cone,
	EPT_Box,
	EPT_Max,
};

extern FVertexSimple LineGridVertices[10000];

extern FVertexSimple LineVertices[2];
extern FVertexSimple CubeVertices[36];
extern FVertexSimple SphereVertices[2400];
extern FVertexSimple TriangleVertices[3];
extern TArray<uint32> CubeIndecies;
