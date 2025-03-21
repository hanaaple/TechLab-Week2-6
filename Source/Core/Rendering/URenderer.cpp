﻿#include "URenderer.h"

#include "Core/Rendering/BufferCache.h"
#include "Core/Math/Transform.h"
#include "Object/Actor/Camera.h"

#include "DataTypes/Structs.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"
#include "Static/FEditorManager.h"

#include "Shader/UShaderManager.h"

#include "Object/Actor/BillBoardText.h"

void URenderer::Create(HWND hWindow)
{
    CreateDeviceAndSwapChain(hWindow);
    CreateFrameBuffer();
    CreateRasterizerState();
    CreateBufferCache();
    CreateDepthStencilBuffer();
    CreateDepthStencilState();

    D3D11_RASTERIZER_DESC WireframeDesc = {};
    WireframeDesc.FillMode = D3D11_FILL_WIREFRAME;  // 와이어프레임 모드
    WireframeDesc.CullMode = D3D11_CULL_BACK;
    WireframeDesc.FrontCounterClockwise = FALSE;
    Device->CreateRasterizerState(&WireframeDesc, &WireframeRasterizerState);

    D3D11_RASTERIZER_DESC SolidDesc = {};
    SolidDesc.FillMode = D3D11_FILL_SOLID;  //  일반 모드
    SolidDesc.CullMode = D3D11_CULL_BACK;
    SolidDesc.FrontCounterClockwise = FALSE;
    Device->CreateRasterizerState(&SolidDesc, &SolidRasterizerState);
    DeviceContext->RSSetState(SolidRasterizerState);
    
    UShaderManager::Get().Initialize(*this);
}

void URenderer::Release()
{
    ReleaseRasterizerState();

    // 렌더 타겟을 초기화
    DeviceContext->OMSetRenderTargets(0, nullptr, DepthStencilView);

    ReleaseFrameBuffer();
    ReleaseDepthStencilBuffer();
    ReleaseDeviceAndSwapChain();
    ReleaseBufferCache();
    UShaderManager::Get().ReleaseAll();
}

void URenderer::CreateShader()
{
    /**
         * 컴파일된 셰이더의 바이트코드를 저장할 변수 (ID3DBlob)
         *
         * 범용 메모리 버퍼를 나타내는 형식
         *   - 여기서는 shader object bytecode를 담기위해 쓰임
         * 다음 두 메서드를 제공한다.
         *   - LPVOID GetBufferPointer
         *     - 버퍼를 가리키는 void* 포인터를 돌려준다.
         *   - SIZE_T GetBufferSize
         *     - 버퍼의 크기(바이트 갯수)를 돌려준다
         */
    // UShaderManager를 통해 셰이더 로드
    UShaderManager::Get().LoadAllShaders();
    
    UShader* DefaultShader = UShaderManager::Get().GetShader(EShaderType::DefaultShader);
    if (!DefaultShader)
    {
        std::cerr << "Failed to load DefaultShader." << std::endl;
        return;
    }
    
    // 정점 하나의 크기를 설정 (바이트 단위)
    VertexStride = sizeof(FVertexSimple);
}

// 일단 비워두기만
void URenderer::CreateConstantBuffer()
{
    // D3D11_BUFFER_DESC ConstantBufferDescPicking = {};
    // ConstantBufferDescPicking.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
    // ConstantBufferDescPicking.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
    // ConstantBufferDescPicking.ByteWidth = sizeof(FPickingConstants) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    // ConstantBufferDescPicking.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정
    //
    // Device->CreateBuffer(&ConstantBufferDescPicking, nullptr, &ConstantPickingBuffer);

    // D3D11_BUFFER_DESC ConstantBufferDescDepth = {};
    // ConstantBufferDescPicking.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 CPU에서 업데이트 하기 위해
    // ConstantBufferDescPicking.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 설정
    // ConstantBufferDescPicking.ByteWidth = sizeof(FDepthConstants) + 0xf & 0xfffffff0;  // 16byte의 배수로 올림
    // ConstantBufferDescPicking.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 접근이 가능하게 설정
    //
    // Device->CreateBuffer(&ConstantBufferDescPicking, nullptr, &ConstantsDepthBuffer);

    // D3D11_BUFFER_DESC ConstantBufferDescUV = {};
    // ConstantBufferDescUV.Usage = D3D11_USAGE_DYNAMIC;                        // 매 프레임 업데이트 가능
    // ConstantBufferDescUV.BindFlags = D3D11_BIND_CONSTANT_BUFFER;             // 상수 버퍼로 사용
    // ConstantBufferDescUV.ByteWidth = sizeof(FUVConstants) + 0xf & 0xfffffff0;  // 16byte 정렬
    // ConstantBufferDescUV.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;            // CPU에서 쓰기 가능
    //
    // Device->CreateBuffer(&ConstantBufferDescUV, nullptr, &ConstantsUVBuffer);
}

void URenderer::ReleaseConstantBuffer()
{
    // if (ConstantPickingBuffer)
    // {
    //     ConstantPickingBuffer->Release();
    //     ConstantPickingBuffer = nullptr;
    // }

    if (ConstantsDepthBuffer)
    {
        ConstantsDepthBuffer->Release();
        ConstantsDepthBuffer = nullptr;
    }

    if (ConstantsUVBuffer) {
        ConstantsUVBuffer->Release();
        ConstantsUVBuffer = nullptr;
    }
}

void URenderer::SwapBuffer() const
{
    SwapChain->Present(1, 0); // SyncInterval: VSync 활성화 여부
}

void URenderer::Prepare() const
{
    // 화면 지우기
    DeviceContext->ClearRenderTargetView(FrameBufferRTV, ClearColor);
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Rasterization할 Viewport를 설정 
    DeviceContext->RSSetViewports(1, &ViewportInfo);
    
    /**
     * OutputMerger 설정
     * 렌더링 파이프라인의 최종 단계로써, 어디에 그릴지(렌더 타겟)와 어떻게 그릴지(블렌딩)를 지정
     */
	DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);    // DepthStencil 뷰 설정
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);
}

void URenderer::PrepareShader(EShaderType ShaderType)
{
    if (CurrentShaderType == ShaderType)
         return;
    
    CurrentShaderType = ShaderType;
    
    UShader* Shader = UShaderManager::Get().GetShader(CurrentShaderType);
    if (Shader == nullptr)
    {
        // DeviceContext->VSSetShader(nullptr, nullptr, 0);
        // DeviceContext->PSSetShader(nullptr, nullptr, 0);
    }
    else
    {
        // DeviceContext에 적용
        Shader->Apply(DeviceContext);
    }
}

void URenderer::RenderPrimitive(UPrimitiveComponent* PrimitiveComp)
{
    if (BufferCache == nullptr)
    {
        return;
    }

    BufferInfo VertexBufferInfo = BufferCache->GetVertexBufferInfo(PrimitiveComp->GetMeshType());
    
    if (VertexBufferInfo.GetBuffer() == nullptr)
    {
        return;
    }

    PrepareShader(PrimitiveComp->GetShaderType());
    
    // 특정 애들은 Render 자체를 해주면 안됨.
    // -> Shader가 없는 경우
    auto* Shader = UShaderManager::Get().GetShader(CurrentShaderType);

    if (Shader != nullptr)
    {
        BufferInfo IndexBufferInfo = BufferCache->GetIndexBufferInfo(PrimitiveComp->GetMeshType());
        
        PrepareTexture(PrimitiveComp->GetTexture());        
        PrepareTopology(PrimitiveComp->GetTopology());
        
        FConstantBufferContext ConstantBufferContext;
        ConstantBufferContext.PrimitiveComponent = PrimitiveComp;
        
        Shader->UpdateConstantBuffer(&ConstantBufferContext);
        RenderPrimitiveInternal(VertexBufferInfo, IndexBufferInfo);
    }
}

void URenderer::RenderBatch(FBatchRenderContext& BatchContext)
{
    if (BufferCache == nullptr)
    {
        return;
    }
    
    if (BatchContext.bIsDirty)
    {
        TArray<FVertexSimple> VertexData;
        if (BatchContext.bUseIndexBuffer)
        {
            uint32 VertexOffset = 0;
            TArray<uint32> IndexData;
            for (const auto& [MeshType, RenderComponents] : BatchContext.RenderComponentMap)
            {
                for (auto* RenderComponent : RenderComponents)
                {
                    TArray<FVertexSimple> Vertecies;
                    if (RenderComponent->TryGetVertexData(&Vertecies))
                    {
                        VertexData.Append(Vertecies);

                        TArray<uint32> Indecies = *MeshResourceCache::Get().GetIndexData(MeshType);
                        
                        for (auto Index : Indecies)
                        {
                            IndexData.Add(Index + VertexOffset);
                        }

                        VertexOffset += Vertecies.Num();
                    }
                }   
            }
            uint32 IndexDataSize = sizeof(uint32) * IndexData.Num();
            
            ID3D11Buffer* IndexBuffer = CreateBuffer(IndexData.GetData(), IndexDataSize, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);

            BufferCache->UpdateIndexBuffer(BatchContext.TextureType, BatchContext.Topology, IndexBuffer, IndexData.Num());
        }
        else
        {
            for (const auto& [MeshType, RenderComponents] : BatchContext.RenderComponentMap)
            {
                // 같은 메시인 컴포넌트
                for (auto* RenderComponent : RenderComponents)
                {
                    TArray<FVertexSimple> Vertecies;
                    if (RenderComponent->TryGetVertexData(&Vertecies))
                    {
                        VertexData.Append(Vertecies);
                    }
                }
            }
        }
        uint32 VertexDataSize = sizeof(FVertexSimple) * VertexData.Num();
        
        ID3D11Buffer* VertexBuffer = CreateBuffer(VertexData.GetData(), VertexDataSize, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
        BufferCache->UpdateVertexBuffer(BatchContext.TextureType, BatchContext.Topology, VertexBuffer, VertexData.Num());
        BatchContext.bIsDirty = false;
    }

    PrepareShader(BatchContext.ShaderType);

    auto* Shader = UShaderManager::Get().GetShader(CurrentShaderType);

    if (Shader != nullptr)
    {
        PrepareTexture(BatchContext.TextureType);
        PrepareTopology(BatchContext.Topology);

        BufferInfo VertexBufferInfo = BufferCache->GetVertexBufferInfo(BatchContext.TextureType, BatchContext.Topology);
        BufferInfo IndexBufferInfo = BufferCache->GetIndexBufferInfo(BatchContext.TextureType, BatchContext.Topology);

        FConstantBufferContext ConstantBufferContext;
        ConstantBufferContext.BatchContext = &BatchContext;
        Shader->UpdateConstantBuffer(&ConstantBufferContext);
        
        RenderPrimitiveInternal(VertexBufferInfo, IndexBufferInfo);
    }
}

void URenderer::RenderPrimitiveInternal(const BufferInfo& VertexBufferInfo, const BufferInfo& IndexBufferInfo) const
{
    UINT VertexBufferOffset = 0;
    ID3D11Buffer* VertexBuffer = VertexBufferInfo.GetBuffer();
    ID3D11Buffer* IndexBuffer = IndexBufferInfo.GetBuffer();
    DeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexStride, &VertexBufferOffset);
    
    if (IndexBuffer == nullptr)
    {
        DeviceContext->Draw(VertexBufferInfo.GetSize(), 0);
    }
    else
    {
        UINT IndexBufferOffset = 0;
        DeviceContext->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, IndexBufferOffset);
        DeviceContext->DrawIndexed(IndexBufferInfo.GetSize(), IndexBufferOffset, 0);
    }
}

ID3D11Buffer* URenderer::CreateBuffer(const void* Data, UINT ByteWidth, D3D11_BIND_FLAG BindFlag, D3D11_USAGE D3d11Usage) const
{
    if (ByteWidth <= 0)
        return nullptr;
    
    D3D11_BUFFER_DESC BufferDesc = {};
    BufferDesc.ByteWidth = ByteWidth;
    BufferDesc.Usage = D3d11Usage;
    BufferDesc.BindFlags = BindFlag;

    D3D11_SUBRESOURCE_DATA BufferSRD = {};
    BufferSRD.pSysMem = Data;

    ID3D11Buffer* Buffer = nullptr;
    const HRESULT Result = Device->CreateBuffer(&BufferDesc, &BufferSRD, &Buffer);
    if (FAILED(Result))
    {
        return nullptr;
    }
    return Buffer;
}

// void URenderer::UpdateConstantPrimitive(const ConstantUpdateInfo& UpdateInfo) const
// {
//     UShader* DefaultShader = UShaderManager::Get().GetShader(EShaderType::DefaultShader);
//     if (!DefaultShader)
//     {
//         std::cerr << "Failed to retrieve DefaultShader." << std::endl;
//         return;
//     }
//     FMatrix MVP = FMatrix::Transpose(UpdateInfo.Transform.GetMatrix() * ViewMatrix * ProjectionMatrix);
//     DefaultShader->UpdateConstantBuffer(DeviceContext, 0, &MVP, sizeof(MVP));
//     struct PSConstants { FVector4 Color; uint32 bUseVertexColor; };
//     PSConstants PSData = { UpdateInfo.Color, UpdateInfo.bUseVertexColor };
//     DefaultShader->UpdateConstantBuffer(DeviceContext, 1, &PSData, sizeof(PSData));
// }


// TODOOOOOOOO
// void URenderer::UpdateConstantBatch(const FBatchRenderContext& BatchRenderContext) const
// {    
//     UShader* DefaultShader = UShaderManager::Get().GetShader(EShaderType::DefaultShader);
//     if (!DefaultShader)
//     {
//         std::cerr << "Failed to retrieve DefaultShader." << std::endl;
//         return;
//     }
//     DefaultShader->UpdateConstantBuffer(DeviceContext, 0, &BatchRenderContext, sizeof(BatchRenderContext));
//     //DefaultShader->VertexConstants.MVP = FMatrix::Transpose(ViewMatrix * ProjectionMatrix);
//     //DefaultShader->UpdateVertexConstantBuffer(DeviceContext);
// }

void URenderer::CreateDeviceAndSwapChain(HWND hWindow)
{
    // 지원하는 Direct3D 기능 레벨을 정의
    D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
    
    // SwapChain 구조체 초기화
    DXGI_SWAP_CHAIN_DESC SwapChainDesc = {};
    SwapChainDesc.BufferDesc.Width = 0;                            // 창 크기에 맞게 자동으로 설정
    SwapChainDesc.BufferDesc.Height = 0;                           // 창 크기에 맞게 자동으로 설정
    SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;  // 색상 포멧
    SwapChainDesc.SampleDesc.Count = 1;                            // 멀티 샘플링 비활성화
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;   // 렌더 타겟으로 설정
    SwapChainDesc.BufferCount = 2;                                 // 더블 버퍼링
    SwapChainDesc.OutputWindow = hWindow;                          // 렌더링할 창 핸들
    SwapChainDesc.Windowed = TRUE;                                 // 창 모드
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      // 스왑 방식
    
    // Direct3D Device와 SwapChain을 생성
    D3D11CreateDeviceAndSwapChain(
        // 입력 매개변수
        nullptr,                                                       // 디바이스를 만들 때 사용할 비디오 어댑터에 대한 포인터
        D3D_DRIVER_TYPE_HARDWARE,                                      // 만들 드라이버 유형을 나타내는 D3D_DRIVER_TYPE 열거형 값
        nullptr,                                                       // 소프트웨어 래스터라이저를 구현하는 DLL에 대한 핸들
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_DEBUG,  // 사용할 런타임 계층을 지정하는 D3D11_CREATE_DEVICE_FLAG 열거형 값들의 조합
        FeatureLevels,                                                 // 만들려는 기능 수준의 순서를 결정하는 D3D_FEATURE_LEVEL 배열에 대한 포인터
        ARRAYSIZE(FeatureLevels),                                      // pFeatureLevels 배열의 요소 수
        D3D11_SDK_VERSION,                                             // SDK 버전. 주로 D3D11_SDK_VERSION을 사용
        &SwapChainDesc,                                                // SwapChain 설정과 관련된 DXGI_SWAP_CHAIN_DESC 구조체에 대한 포인터
    
        // 출력 매개변수
        &SwapChain,                                                    // 생성된 IDXGISwapChain 인터페이스에 대한 포인터
        &Device,                                                       // 생성된 ID3D11Device 인터페이스에 대한 포인터
        nullptr,                                                       // 선택된 기능 수준을 나타내는 D3D_FEATURE_LEVEL 값을 반환
        &DeviceContext                                                 // 생성된 ID3D11DeviceContext 인터페이스에 대한 포인터
    );
    
    // 생성된 SwapChain의 정보 가져오기
    SwapChain->GetDesc(&SwapChainDesc);
    
    // 뷰포트 정보 설정
    ViewportInfo = {
        0.0f, 0.0f,
        static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height),
        0.0f, 1.0f
    };
}

void URenderer::ReleaseDeviceAndSwapChain()
{
    if (DeviceContext)
    {
        DeviceContext->Flush(); // 남이있는 GPU 명령 실행
    }

    if (SwapChain)
    {
        SwapChain->Release();
        SwapChain = nullptr;
    }

    if (Device)
    {
        Device->Release();
        Device = nullptr;
    }

    if (DeviceContext)
    {
        DeviceContext->Release();
        DeviceContext = nullptr;
    }
}

void URenderer::ReleaseBufferCache()
{
    if (BufferCache)
    {
        BufferCache->Release();
        BufferCache.reset();
    }
}

void URenderer::CreateFrameBuffer()
{
    // 스왑 체인으로부터 백 버퍼 텍스처 가져오기
    SwapChain->GetBuffer(0, IID_PPV_ARGS(&FrameBuffer));

    // 렌더 타겟 뷰 생성
    D3D11_RENDER_TARGET_VIEW_DESC FrameBufferRTVDesc = {};
    FrameBufferRTVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;      // 색상 포맷
    FrameBufferRTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D; // 2D 텍스처

    Device->CreateRenderTargetView(FrameBuffer, &FrameBufferRTVDesc, &FrameBufferRTV);
}

void URenderer::CreateDepthStencilBuffer()
{
    D3D11_TEXTURE2D_DESC DepthBufferDesc = {};
    DepthBufferDesc.Width = static_cast<UINT>(ViewportInfo.Width);
    DepthBufferDesc.Height = static_cast<UINT>(ViewportInfo.Height);
    DepthBufferDesc.MipLevels = 1;
    DepthBufferDesc.ArraySize = 1;
	DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // 32비트 중 24비트는 깊이, 8비트는 스텐실
    DepthBufferDesc.SampleDesc.Count = 1;
    DepthBufferDesc.SampleDesc.Quality = 0;
    DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;              // 텍스쳐 바인딩 플래그를 DepthStencil로 설정
    DepthBufferDesc.CPUAccessFlags = 0;
    DepthBufferDesc.MiscFlags = 0;

    HRESULT result = Device->CreateTexture2D(&DepthBufferDesc, nullptr, &DepthStencilBuffer);

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DepthBufferDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    
    result = Device->CreateDepthStencilView(DepthStencilBuffer, &dsvDesc, &DepthStencilView);
}

void URenderer::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = TRUE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;                     // 더 작은 깊이값이 왔을 때 픽셀을 갱신함
    // DepthStencilDesc.StencilEnable = FALSE;                                 // 스텐실 테스트는 하지 않는다.
    // DepthStencilDesc.StencilReadMask = 0xFF;
    // DepthStencilDesc.StencilWriteMask = 0xFF;
    // DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    // DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    // DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    // DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    // DepthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    // DepthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    // DepthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    // DepthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState);
    
    D3D11_DEPTH_STENCIL_DESC IgnoreDepthStencilDesc = {};
    DepthStencilDesc.DepthEnable = TRUE;
    DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;                     
    Device->CreateDepthStencilState(&IgnoreDepthStencilDesc ,&IgnoreDepthStencilState);
}

void URenderer::ReleaseFrameBuffer()
{
    if (FrameBuffer)
    {
        FrameBuffer->Release();
        FrameBuffer = nullptr;
    }

    if (FrameBufferRTV)
    {
        FrameBufferRTV->Release();
        FrameBufferRTV = nullptr;
    }

  //   if (PickingFrameBuffer)
  //   {
		// PickingFrameBuffer->Release();
		// PickingFrameBuffer = nullptr;
  //   }
  //
  //   if (PickingFrameBufferRTV)
  //   {
		// PickingFrameBufferRTV->Release();
		// PickingFrameBufferRTV = nullptr;
  //   }
}

void URenderer::ReleaseDepthStencilBuffer()
{
    if (DepthStencilBuffer)
    {
        DepthStencilBuffer->Release();
        DepthStencilBuffer = nullptr;
    }
    if (DepthStencilView)
    {
        DepthStencilView->Release();
        DepthStencilView = nullptr;
    }
    if (DepthStencilState)
    {
        DepthStencilState->Release();
        DepthStencilState = nullptr;
    }
    if (IgnoreDepthStencilState)
    {
        IgnoreDepthStencilState->Release();
        IgnoreDepthStencilState = nullptr;
    }
}

void URenderer::CreateRasterizerState()
{
    D3D11_RASTERIZER_DESC RasterizerDesc = {};
    RasterizerDesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
    RasterizerDesc.CullMode = D3D11_CULL_BACK;  // 백 페이스 컬링
    RasterizerDesc.FrontCounterClockwise = FALSE;

    Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
}

void URenderer::ReleaseRasterizerState()
{
    if (RasterizerState)
    {
        RasterizerState->Release();
        RasterizerState = nullptr;
    }
}

void URenderer::CreateBufferCache()
{
    BufferCache = std::make_unique<FBufferCache>();
}

void URenderer::CreateSamplerState()
{
    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    Device->CreateSamplerState(&samplerDesc, &SamplerState);
}

void URenderer::ReleaseSamplerState()
{
    if (SamplerState)
    {
        SamplerState->Release();
        SamplerState = nullptr;
    }
}

void URenderer::ReleaseTextureSRVs()
{
    // if (TextureSRV)
    // {
    //     TextureSRV->Release();
    //     TextureSRV = nullptr;
    // }
    //
    // // 텍스처 객체 해제
    // if (texture)
    // {
    //     texture->Release();
    //     texture = nullptr;
    // }
}

void URenderer::PrepareTopology(D3D_PRIMITIVE_TOPOLOGY Topology)
{
    if (CurrentTopology != Topology)
    {
        DeviceContext->IASetPrimitiveTopology(Topology);
        CurrentTopology = Topology;
    }
}

// void URenderer::ReleasePickingFrameBuffer()
// {
// 	if (PickingFrameBuffer)
// 	{
// 		PickingFrameBuffer->Release();
// 		PickingFrameBuffer = nullptr;
// 	}
// 	if (PickingFrameBufferRTV)
// 	{
// 		PickingFrameBufferRTV->Release();
// 		PickingFrameBufferRTV = nullptr;
// 	}
// }

void URenderer::PrepareZIgnore()
{
    DeviceContext->OMSetDepthStencilState(IgnoreDepthStencilState, 0);
}

// void URenderer::PreparePicking()
// {
//     // 렌더 타겟 바인딩
//     //DeviceContext->OMSetRenderTargets(1, &PickingFrameBufferRTV, DepthStencilView);
//     DeviceContext->OMSetRenderTargets(1, &PickingFrameBufferRTV, nullptr);
//     DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
//     DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil 상태 설정. StencilRef: 스텐실 테스트 결과의 레퍼런스
//
//     DeviceContext->ClearRenderTargetView(PickingFrameBufferRTV, PickingClearColor);
// }

// void URenderer::PreparePickingShader() const
// {
//     DeviceContext->PSSetShader(PickingPixelShader, nullptr, 0);
//
//     if (ConstantPickingBuffer)
//     {
//         DeviceContext->PSSetConstantBuffers(1, 1, &ConstantPickingBuffer);
//     }
// }

// void URenderer::UpdateConstantDepth(int Depth) const
// {
//     if (!ConstantsDepthBuffer) return;
//
//     ACamera* Cam = FEditorManager::Get().GetCamera();
//     
//     D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
//     
//     DeviceContext->Map(ConstantsDepthBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
//     {
//         FDepthConstants* Constants = static_cast<FDepthConstants*>(ConstantBufferMSR.pData);
//         Constants->DepthOffset = Depth;
//         Constants->nearPlane = Cam->GetNear();
//         Constants->farPlane = Cam->GetFar();
//     }
//     DeviceContext->Unmap(ConstantsDepthBuffer, 0);
// }

// void URenderer::UpdateConstantFontUV(char c) const {
//     int correct_c = c - 32;
//     if (!ConstantsUVBuffer) return;
//     D3D11_MAPPED_SUBRESOURCE ConstantBufferMSR;
//     DeviceContext->Map(ConstantsUVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ConstantBufferMSR);
//     {
//         FUVConstants* Constants = static_cast<FUVConstants*>(ConstantBufferMSR.pData);
//         
//         CharacterInfo curCharInfo = UTextureLoader::Get().GetTexture()..AtlasInfoMap[correct_c];
//         
//         Constants->U = curCharInfo.u;
//         Constants->V = curCharInfo.v;
//         Constants->Width = curCharInfo.width;
//         Constants->Height = curCharInfo.height;
//     }
//     DeviceContext->Unmap(ConstantsUVBuffer, 0);
// }

void URenderer::PrepareMain()
{
	DeviceContext->OMSetDepthStencilState(DepthStencilState, 0);                // DepthStencil 상태 설정. StencilRef: 스텐실 테스트 결과의 레퍼런스
    DeviceContext->OMSetRenderTargets(1, &FrameBufferRTV, DepthStencilView);
    DeviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void URenderer::UpdateViewMatrix(const FTransform& CameraTransform)
{
    ViewMatrix = CameraTransform.GetViewMatrix();
}

void URenderer::UpdateProjectionMatrix(ACamera* Camera)
{
    float AspectRatio = UEngine::Get().GetScreenRatio();

    float FOV = FMath::DegreesToRadians(Camera->GetFieldOfView());
    float Near = Camera->GetNear();
    float Far = Camera->GetFar();

    if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        ProjectionMatrix = FMatrix::PerspectiveFovLH(FOV, AspectRatio, Near, Far);
    }
    else if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        ProjectionMatrix = FMatrix::PerspectiveFovLH(FOV, AspectRatio, Near, Far);

        // TODO: 추가 필요.
        // ProjectionMatrix = FMatrix::OrthoForLH(FOV, AspectRatio, Near, Far);
    }
}

void URenderer::OnUpdateWindowSize(int Width, int Height)
{
    if (SwapChain)
    {
        ReleaseFrameBuffer();
        //ReleasePickingFrameBuffer();
        ReleaseDepthStencilBuffer();
        
        SwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);

        DXGI_SWAP_CHAIN_DESC SwapChainDesc;
        SwapChain->GetDesc(&SwapChainDesc);
        // 뷰포트 정보 갱신
        ViewportInfo = {
            0.0f, 0.0f,
            static_cast<float>(SwapChainDesc.BufferDesc.Width), static_cast<float>(SwapChainDesc.BufferDesc.Height),
            0.0f, 1.0f
        };
        
        CreateFrameBuffer();

		//CreatePickingTexture(UEngine::Get().GetWindowHandle());

        CreateDepthStencilBuffer();
    }
}

// 체크
void URenderer::PrepareTexture(ETextureType TextureType)
{
    // 여기서 ResourceView를 들고 있고 매핑된 거로 가져오기,샘플러 유지
    if (TextureType == CurrentTextureType)
    {
        return;
    }

    CurrentTextureType = TextureType;

    Texture* Texture = UTextureLoader::Get().GetTexture(TextureType, Device, DeviceContext);

    if (Texture != nullptr)
    {
        ID3D11ShaderResourceView* TextureSRV = Texture->GetTextureSRV();
        if (TextureSRV != nullptr)
        {
            DeviceContext->PSSetShaderResources(0, 1, &TextureSRV);
            DeviceContext->PSSetSamplers(0, 1, &SamplerState);   
        }else
        {
            ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
            ID3D11SamplerState* nullSampler[1] = { nullptr };
            DeviceContext->PSSetShaderResources(0, 1, nullSRV);
            DeviceContext->PSSetSamplers(0, 1, nullSampler);
        }
    }
    else
    {
        ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
        ID3D11SamplerState* nullSampler[1] = { nullptr };
        DeviceContext->PSSetShaderResources(0, 1, nullSRV);
        DeviceContext->PSSetSamplers(0, 1, nullSampler);
    }
}

void URenderer::SetFillMode(D3D11_FILL_MODE FillMode)
{
    if (CurrentViewMode == FillMode)
        return;
    
    CurrentViewMode = FillMode;
    
    if (CurrentViewMode == D3D11_FILL_WIREFRAME)
    {
        DeviceContext->RSSetState(WireframeRasterizerState);
    }
    else
    {
        DeviceContext->RSSetState(SolidRasterizerState);
    }
}

bool URenderer::ShouldRenderActor(const AActor* OwnerActor)
{
    if (!OwnerActor) return false;
    const auto& ShowFlags = UEngine::Get().GetShowFlagStates();

    //Primitives 비활성화 + Gizmo가 아닌 경우 렌더링 X
    if (!ShowFlags[EEngineShowFlags::SF_Primitives] && !OwnerActor->IsGizmoActor()) 
        return false;

    //Gizmo 비활성화 + Gizmo인 경우 렌더링 X
    if (!ShowFlags[EEngineShowFlags::SF_Gizmo] && OwnerActor->IsGizmoActor()) 
        return false;
    
    //BillboardText 비활성화 + BillboardText인 경우 렌더링 X
    if (!ShowFlags[EEngineShowFlags::SF_BillboardText] && OwnerActor->IsA<ABillboardText>()) 
        return false;
    

    return true;
}