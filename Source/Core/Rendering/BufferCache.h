#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>

#include <wrl/client.h>
#include <unordered_map>

#include "BatchRenderContext.h"
#include "Primitive/PrimitiveVertices.h"
#include "Core/Container/Array.h"

struct BufferInfo
{
public:
	BufferInfo() = default;
	BufferInfo(ID3D11Buffer* InBuffer, uint32 BufferSize)
	{
		Buffer = InBuffer;
		Size = BufferSize;
	}

	ID3D11Buffer* GetBuffer() const { return Buffer.Get(); }
	uint32 GetSize() const { return Size; }

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Buffer = nullptr;
	uint32 Size = 0;
};


// Vertex Buffer, Index Buffer Cache
// Batch Rendering을 하지 않는 애들만 사용
class FBufferCache
{
private:
	std::unordered_map <EPrimitiveMeshType, BufferInfo> VertexBufferCache;
	std::unordered_map <EPrimitiveMeshType, BufferInfo> IndexBufferCache;

	TMap<ID3D11ShaderResourceView*, TMap<D3D11_PRIMITIVE_TOPOLOGY, BufferInfo>> BatchIndexBufferCache;
	TMap<ID3D11ShaderResourceView*, TMap<D3D11_PRIMITIVE_TOPOLOGY, BufferInfo>> BatchVertexBufferCache;
public:
	FBufferCache();
	~FBufferCache();

	BufferInfo& GetVertexBufferInfo(EPrimitiveMeshType Type);
	BufferInfo& GetIndexBufferInfo(EPrimitiveMeshType Type);

	BufferInfo& GetVertexBufferInfo(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology);
	BufferInfo& GetIndexBufferInfo(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology);

	void UpdateVertexBuffer(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology, ID3D11Buffer* Buffer);
	void UpdateIndexBuffer(ID3D11ShaderResourceView* Texture, D3D11_PRIMITIVE_TOPOLOGY Topology, ID3D11Buffer* Buffer);
	
	TArray<uint32> GetStaticIndexData(EPrimitiveMeshType MeshType);
	TArray<FVertexSimple> GetStaticVertexData(EPrimitiveMeshType MeshType);
	
	void Release();
public:
	TArray<FVertexSimple> CreateArrowVertices();
	TArray<FVertexSimple> CreateConeVertices();
	TArray<FVertexSimple> CreateCylinderVertices();

private :
	BufferInfo CreateVertexBufferInfo(EPrimitiveMeshType Type);
	BufferInfo CreateIndexBufferInfo(EPrimitiveMeshType Type);
};