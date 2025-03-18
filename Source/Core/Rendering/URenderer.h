#pragma once

#define _TCHAR_DEFINED
#include <d3d11.h>
#include <memory>

#include "BatchRenderContext.h"
#include "Core/Math/Vector.h"

#include "Core/Math/Matrix.h"
#include "Core/Math/Plane.h"
#include "Core/Math/Transform.h"
#include "Core/Rendering/BufferCache.h"
#include "Texture/TextureLoader.h"

class AActor;
struct FVertexSimple;
struct FVector4;

class ACamera;

class URenderer
{
private:
    struct FConstants
    {
        FMatrix MVP;
        FVector4 Color;
		// true인 경우 Vertex Color를 사용하고, false인 경우 Color를 사용합니다.
        uint32 bUseVertexColor;
        uint32 bUseUV;
    };
	
	struct alignas(16) FPickingConstants
	{
		FVector4 UUIDColor;
	};

	struct alignas(16) FDepthConstants{
		unsigned int DepthOffset;
		int nearPlane;
		int farPlane;
	};

    struct alignas(16) FUVConstants {
        float U;
        float V;
        float Width;
        float Height;
    };
	
    struct ConstantUpdateInfo
    {
        const FTransform& Transform;
		const FVector4& Color;
        bool bUseVertexColor;
        bool bUseUV;
    };

public:
    /** Renderer를 초기화 합니다. */
    void Create(HWND hWindow);
	
    /** Renderer에 사용된 모든 리소스를 해제합니다. */
    void Release();

    void CreateShader();

    void CreateConstantBuffer();

    void ReleaseConstantBuffer();

    /** 스왑 체인의 백 버퍼와 프론트 버퍼를 교체하여 화면에 출력 */
    void SwapBuffer() const;

    /** 렌더링 파이프라인을 준비 합니다. */
    void Prepare() const;

    /** 셰이더를 준비 합니다. */
	void PrepareShader(EShaderType ShaderType);

	void RenderPrimitive(class UPrimitiveComponent* PrimitiveComp);
	void RenderBatch(FBatchRenderContext& BatchContext);

    /**
     * Buffer에 있는 Vertex를 그립니다.
     * @param pBuffer 렌더링에 사용할 버텍스 버퍼에 대한 포인터
     * @param numVertices 버텍스 버퍼에 저장된 버텍스의 총 개수
     */
    void RenderPrimitiveInternal(const BufferInfo& VertexBufferInfo, const BufferInfo& IndexBufferInfo) const;
	
    /**
     * 정점 데이터로 Vertex Buffer를 생성합니다.
     * @param Data 버퍼로 변환할 정점 데이터 배열의 포인터
     * @param ByteWidth 버퍼의 총 크기 (바이트 단위)
     * @param BindFlag
     * @param D3d11Usage
     * @return 생성된 버텍스 버퍼에 대한 ID3D11Buffer 포인터, 실패 시 nullptr
     *
     * @note 이 함수는 D3D11_USAGE_IMMUTABLE 사용법으로 버퍼를 생성합니다.
     */
	ID3D11Buffer* CreateBuffer(const void* Data, UINT ByteWidth, D3D11_BIND_FLAG BindFlag, D3D11_USAGE D3d11Usage) const;

    /** Constant Data를 업데이트 합니다. */
    //void UpdateConstantPrimitive(const ConstantUpdateInfo& UpdateInfo) const;
    //void UpdateConstantBatch(const FBatchRenderContext& BatchRenderContext) const;

    ID3D11Device* GetDevice() const { return Device; }
    ID3D11DeviceContext* GetDeviceContext() const { return DeviceContext; }

    /** View 변환 Matrix를 업데이트 합니다. */
    void UpdateViewMatrix(const FTransform& CameraTransform);

    /** Projection 변환 Matrix를 업데이트 합니다. */
    void UpdateProjectionMatrix(ACamera* Camera);

	void OnUpdateWindowSize(int Width, int Height);
    void PrepareTexture(ETextureType TextureType);
    FBufferCache* GetBufferCache() const { return BufferCache.get(); }

protected:
    /** Direct3D Device 및 SwapChain을 생성합니다. */
    void CreateDeviceAndSwapChain(HWND hWindow);

    /** Direct3D Device 및 SwapChain을 해제합니다.  */
    void ReleaseDeviceAndSwapChain();

	void ReleaseBufferCache();
	
    /** 프레임 버퍼를 생성합니다. */
    void CreateFrameBuffer();

    /** 뎁스 스텐실 버퍼를 생성합니다. */
	void CreateDepthStencilBuffer();

    /** 뎁스 스텐실 상태를 생성합니다. */
	void CreateDepthStencilState();
	
    /** 프레임 버퍼를 해제합니다. */
    void ReleaseFrameBuffer();

	/** 뎁스 스텐실 버퍼를 해제합니다. */
	void ReleaseDepthStencilBuffer();
	
    /** 레스터라이즈 상태를 생성합니다. */
    void CreateRasterizerState();

    /** 레스터라이저 상태를 해제합니다. */
    void ReleaseRasterizerState();

    void CreateBufferCache();

	void CreateSamplerState();

	
	
	void ReleaseSamplerState();
	
	void ReleaseTextureSRVs();
	
    void PrepareTopology(D3D_PRIMITIVE_TOPOLOGY Topology);

protected:
    // Direct3D 11 장치(Device)와 장치 컨텍스트(Device Context) 및 스왑 체인(Swap Chain)을 관리하기 위한 포인터들
    ID3D11Device* Device = nullptr;                         // GPU와 통신하기 위한 Direct3D 장치
    ID3D11DeviceContext* DeviceContext = nullptr;           // GPU 명령 실행을 담당하는 컨텍스트
    IDXGISwapChain* SwapChain = nullptr;                    // 프레임 버퍼를 교체하는 데 사용되는 스왑 체인

    // 렌더링에 필요한 리소스 및 상태를 관리하기 위한 변수들
    ID3D11Texture2D* FrameBuffer = nullptr;                 // 화면 출력용 텍스처
    ID3D11RenderTargetView* FrameBufferRTV = nullptr;       // 텍스처를 렌더 타겟으로 사용하는 뷰
    ID3D11RasterizerState* RasterizerState = nullptr;       // 래스터라이저 상태(컬링, 채우기 모드 등 정의)
    //ID3D11Buffer* ConstantBuffer = nullptr;                 // 쉐이더에 데이터를 전달하기 위한 상수 버퍼

    FLOAT ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f }; // 화면을 초기화(clear)할 때 사용할 색상 (RGBA)
    D3D11_VIEWPORT ViewportInfo = {};                       // 렌더링 영역을 정의하는 뷰포트 정보

    // Shader를 렌더링할 때 사용되는 변수들
    //ID3D11VertexShader* SimpleVertexShader = nullptr;       // Vertex 데이터를 처리하는 Vertex 셰이더
    //ID3D11PixelShader* SimplePixelShader = nullptr;         // Pixel의 색상을 결정하는 Pixel 셰이더

    //ID3D11InputLayout* SimpleInputLayout = nullptr;         // Vertex 셰이더 입력 레이아웃 정의
    unsigned int VertexStride = 0;                                // Vertex 버퍼의 각 요소 크기

    // Depth Stenil Buffer
	ID3D11Texture2D* DepthStencilBuffer = nullptr;          // DepthStencil버퍼 역할을 하는 텍스쳐
	ID3D11DepthStencilView* DepthStencilView = nullptr;     // DepthStencil버퍼를 렌더 타겟으로 사용하는 뷰
	ID3D11DepthStencilState* DepthStencilState = nullptr;   // DepthStencil 상태(깊이 테스트, 스텐실 테스트 등 정의)
    ID3D11DepthStencilState* GizmoDepthStencilState = nullptr; // 기즈모용 스텐실 스테이트. Z버퍼 테스트 하지않고 항상 앞에렌더
	
	// Buffer Cache
	std::unique_ptr<FBufferCache> BufferCache;
	//UTextureLoader TextureLoader;

	ID3D11SamplerState* SamplerState = nullptr;
	ETextureType CurrentTextureType = ETextureType::None;
	EShaderType CurrentShaderType = EShaderType::None;
	
    FMatrix ViewMatrix = FMatrix::Identity();
	FMatrix ProjectionMatrix = FMatrix::Identity();

	D3D_PRIMITIVE_TOPOLOGY CurrentTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;

	
protected:
	// 피킹용 버퍼들
	//ID3D11Texture2D* PickingFrameBuffer = nullptr;                 // 화면 출력용 텍스처
	//ID3D11RenderTargetView* PickingFrameBufferRTV = nullptr;       // 텍스처를 렌더 타겟으로 사용하는 뷰
	//ID3D11Buffer* ConstantPickingBuffer = nullptr;                 // 뷰 상수 버퍼
	//FLOAT PickingClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //
	//ID3D11PixelShader* PickingPixelShader = nullptr;         // Pixel의 색상을 결정하는 Pixel 셰이더
	ID3D11Buffer* ConstantsDepthBuffer = nullptr;
    
    ID3D11Buffer* ConstantsUVBuffer = nullptr;           // UV 상수 버퍼

	ID3D11DepthStencilState* IgnoreDepthStencilState = nullptr;   // DepthStencil 상태(깊이 테스트, 스텐실 테스트 등 정의)

public:
	//피킹용 함수들	
    //void ReleasePickingFrameBuffer();
    void PrepareZIgnore();
    //void PreparePicking();
	//void PreparePickingShader() const;
    //void UpdateConstantDepth(int Depth) const;
    //void UpdateConstantFontUV(char c) const;
    void PrepareMain();

	FMatrix GetProjectionMatrix() const { return ProjectionMatrix; }
	FMatrix GetViewMatrix() const { return ViewMatrix; }
	ETextureType GetCurrentTextureType() const { return CurrentTextureType; }
public:
	//View Mode 변경 함수
	void SetFillMode(D3D11_FILL_MODE FillMode);
	void EnableWireframeMode();
	void EnableLitMode();
	void EnableUnlitMode();
	const void ApplyCurrentRasterizerState() const;

	//렌더링 여부 반환
	static bool ShouldRenderActor(const AActor* OwnerActor);

private:
	ID3D11RasterizerState* WireframeRasterizerState = nullptr;
	ID3D11RasterizerState* SolidRasterizerState = nullptr;

};
