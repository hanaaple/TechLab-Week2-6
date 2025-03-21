﻿#pragma once
#include "Core/AbstractClass/Singleton.h"
#include "Core/Container/Array.h"
#include "Core/Container/Map.h"
#include "DataTypes/MeshDataTypes.h"
#include "DataTypes/Structs.h"

class MeshResourceCache : public TSingleton<MeshResourceCache>
{
public:
	/**
	 * @return if null, return 0 Size Array
	 */
	const TArray<FVertexSimple>* GetVertexData(EPrimitiveMeshType MeshType);
	/**
	 * @return if null, return 0 Size Array
	 */
	const TArray<uint32>* GetIndexData(EPrimitiveMeshType MeshType);

	bool HasIndexData(EPrimitiveMeshType MeshType)
	{
		GetIndexData(MeshType);
		return IndexDataMap.Contains(MeshType);
	}

private:
	// 수정 불가능한 Base Vertex, Index Data
	TMap<EPrimitiveMeshType, TArray<FVertexSimple>> VertexDataMap;
	TMap<EPrimitiveMeshType, TArray<uint32>> IndexDataMap;
};
