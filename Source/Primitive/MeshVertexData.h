#pragma once
#include "Core/HAL/PlatformType.h"
#include "DataTypes/Structs.h"
#include "Core/Container/Array.h"

extern TArray<FVertexSimple> LineVertices;
extern TArray<FVertexSimple> SphereVertices;
extern TArray<FVertexSimple> TriangleVertices;
//extern TArray<FVertexSimple> LineGridVertices;

extern TArray<FVertexSimple> BoundingBoxVertices;
extern TArray<uint32> BoundingBoxIndecies;

extern TArray<FVertexSimple> CubeVertices;
extern TArray<uint32> CubeIndecies;

extern TArray<FVertexSimple> QuadVertices;
extern TArray<uint32> QuadIndecies;
