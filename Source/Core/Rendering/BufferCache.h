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
class FBufferCache
{
private:
	std::unordered_map <EPrimitiveComponentType, BufferInfo> VertexBufferCache;
	std::unordered_map <EPrimitiveComponentType, BufferInfo> IndexBufferCache;

public:
	FBufferCache();
	~FBufferCache();
 
	BufferInfo& GetVertexBufferInfo(EPrimitiveComponentType Type);
	BufferInfo& GetIndexBufferInfo(EPrimitiveComponentType Type);

	void Release();
public:
	TArray<FVertexSimple> CreateArrowVertices();
	TArray<FVertexSimple> CreateConeVertices();
	TArray<FVertexSimple> CreateCylinderVertices();

private :
	BufferInfo CreateVertexBufferInfo(EPrimitiveComponentType Type);
	BufferInfo CreateIndexBufferInfo(EPrimitiveComponentType Type);
};