#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include <wrl/client.h>
#include <unordered_map>
#include "Primitive/PrimitiveVertices.h"
#include "Core/Container/Array.h"

struct BufferInfo
{
public:
	BufferInfo() = default;
	BufferInfo(ID3D11Buffer* InBuffer, int BufferSize, D3D_PRIMITIVE_TOPOLOGY InTopology)
	{
		Buffer = InBuffer;
		Size = BufferSize;
		Topology = InTopology;
	}

	ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }
	int GetSize() const { return Size; }
	D3D_PRIMITIVE_TOPOLOGY GetTopology() const { return Topology; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer;
	D3D_PRIMITIVE_TOPOLOGY Topology;
	int Size;
};


// Vertex Buffer, Index Buffer Cache
// Batch Rendering을 하지 않는 애들만 사용
class FBufferCache
{
private:
	std::unordered_map <EPrimitiveMeshType, BufferInfo> VertexBufferCache;
	std::unordered_map <EPrimitiveMeshType, BufferInfo> IndexBufferCache;
	std::unordered_map <EPrimitiveMeshType, TArray<FVertexSimple>> VertexDataCache;

public:
	FBufferCache();
	~FBufferCache();

	BufferInfo& GetVertexBufferInfo(EPrimitiveMeshType Type);
	BufferInfo& GetIndexBufferInfo(EPrimitiveMeshType Type);
	TArray<FVertexSimple>& GetVertexData(EPrimitiveMeshType Type);

	void Release();
public:
	TArray<FVertexSimple> CreateArrowVertices();
	TArray<FVertexSimple> CreateConeVertices();
	TArray<FVertexSimple> CreateCylinderVertices();

private :
	BufferInfo CreateVertexBufferInfo(EPrimitiveMeshType Type);
	BufferInfo CreateIndexBufferInfo(EPrimitiveMeshType Type);
	TArray<FVertexSimple> CreateVertexData(EPrimitiveMeshType Type);
};