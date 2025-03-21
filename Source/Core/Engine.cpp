﻿#include "Engine.h"

#include <iostream>

#include "Debug/DebugConsole.h"
#include "Input/PlayerController.h"
#include "Input/PlayerInput.h"
#include "Object/Actor/Cylinder.h"
#include "Object/Gizmo/Axis.h"
#include "Object/World/World.h"
#include "Static/FEditorManager.h"
#include <Object/Actor/Cone.h>
#include <Object/Actor/Cube.h>
#include <Object/Actor/BillBoardText.h>
#include "Object/Actor/AABBPicker.h"
#include "Object/Gizmo/WorldGrid.h"
#include "Object/Actor/Sphere.h"

// ImGui WndProc 정의
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT UEngine::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // ImGui의 메시지를 처리
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    {
        return true;
    }
    
    switch (uMsg)
    {
        // 창이 제거될 때 (창 닫기, Alt+F4 등)
    case WM_DESTROY:
        PostQuitMessage(0); // 프로그램 종료
        break;
    case WM_KEYDOWN:
        APlayerInput::Get().KeyDown(static_cast<EKeyCode>(wParam));
        if ((lParam>>30)%2 != 0)
        {
            APlayerInput::Get().KeyOnceUp(static_cast<EKeyCode>( wParam ));
        }
        break;
    case WM_KEYUP:
        APlayerInput::Get().KeyUp(static_cast<EKeyCode>( wParam ));
        break;
    case WM_LBUTTONDOWN:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, true, false);
        break;
    case WM_LBUTTONUP:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, false, false);
        break;
    case WM_RBUTTONDOWN:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, true, true);
        break;
    case WM_RBUTTONUP:
        APlayerInput::Get().HandleMouseInput(hWnd, lParam, false, true);
        break;
    case WM_SIZE:
		UEngine::Get().UpdateWindowSize(LOWORD(lParam), HIWORD(lParam));
		break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void UEngine::Initialize(
    HINSTANCE hInstance, const WCHAR* InWindowTitle, const WCHAR* InWindowClassName, int InScreenWidth,
    int InScreenHeight,
    EScreenMode InScreenMode
)
{
    WindowInstance = hInstance;
    WindowTitle = InWindowTitle;
    WindowClassName = InWindowClassName;
    ScreenMode = InScreenMode;
    ScreenWidth = InScreenWidth;
    ScreenHeight = InScreenHeight;

    InitWindow(InScreenWidth, InScreenWidth);
    InitRenderer();

    InitWorld();

    InitializedScreenWidth = ScreenWidth;
    InitializedScreenHeight = ScreenHeight;
    
    ui.Initialize(WindowHandle, *Renderer, ScreenWidth, ScreenHeight);
    InitializeShowFlags();
    UE_LOG("Engine Initialized!");
}

void UEngine::Run()
{
    // FPS 제한
    constexpr int TargetFPS = 750;
    constexpr double TargetDeltaTime = 1000.0f / TargetFPS; // 1 FPS의 목표 시간 (ms)

    // 고성능 타이머 초기화
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);

    LARGE_INTEGER StartTime;
    QueryPerformanceCounter(&StartTime);


    IsRunning = true;
    while (IsRunning)
    {
        // DeltaTime 계산 (초 단위)
        const LARGE_INTEGER EndTime = StartTime;
        QueryPerformanceCounter(&StartTime);

        const float DeltaTime =
            static_cast<float>(StartTime.QuadPart - EndTime.QuadPart) / static_cast<float>(Frequency.QuadPart);

        APlayerInput::Get().PreProcessInput();
        
        // 메시지(이벤트) 처리
        MSG Msg;
        while (PeekMessage(&Msg, nullptr, 0, 0, PM_REMOVE))
        {
            // 키 입력 메시지를 번역
            TranslateMessage(&Msg);

            // 메시지를 등록한 Proc에 전달
            DispatchMessage(&Msg);

            if (Msg.message == WM_QUIT)
            {
                IsRunning = false;
                break;
            }

        }
		// Renderer Update
        Renderer->Prepare();

		// World Update
		if (World)
		{
			World->Tick(DeltaTime);
		    World->UpdateRenderComponents();
		    World->Render();
		    World->LateTick(DeltaTime);
		}

        //각 Actor에서 TickActor() -> PlayerTick() -> TickPlayerInput() 호출하는데 지금은 Message에서 처리하고 있다.
        APlayerInput::Get().TickPlayerInput(); //잘못된 위치. 위에 달린 주석대로 처리해야 정상 플레이어 액터 내에서만 처리해야할것같다.
        
        // TickPlayerInput
        APlayerController::Get().ProcessPlayerInput(DeltaTime);
        
		// ui Update
        ui.Update();

        Renderer->SwapBuffer();

        // FPS 제한
        double ElapsedTime;
        do
        {
            Sleep(0);

            LARGE_INTEGER CurrentTime;
            QueryPerformanceCounter(&CurrentTime);

            ElapsedTime = static_cast<double>(CurrentTime.QuadPart - StartTime.QuadPart) * 1000.0 / static_cast<double>(Frequency.QuadPart);
        } while (ElapsedTime < TargetDeltaTime);
    }
}


void UEngine::Shutdown()
{
    ShutdownWindow();
}


void UEngine::InitWindow(int InScreenWidth, int InScreenHeight)
{
    // 윈도우 클래스 등록
    WNDCLASSW wnd_class{};
    wnd_class.lpfnWndProc = WndProc;
    wnd_class.hInstance = WindowInstance;
    wnd_class.lpszClassName = WindowClassName;
    RegisterClassW(&wnd_class);

    // Window Handle 생성
    WindowHandle = CreateWindowExW(
        0, WindowClassName, WindowTitle,
        WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        InScreenWidth, InScreenHeight,
        nullptr, nullptr, WindowInstance, nullptr
    );

    // TODO: 전체화면 구현
    if (ScreenMode != EScreenMode::Windowed)
    {
        std::cout << "not implement Fullscreen and Borderless mode." << '\n';
    }

    // 윈도우 포커싱
    ShowWindow(WindowHandle, SW_SHOW);
    SetForegroundWindow(WindowHandle);
    SetFocus(WindowHandle);

    //AllocConsole(); // 콘솔 창 생성

    //// 표준 출력 및 입력을 콘솔과 연결
    //freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    //freopen_s((FILE**)stdin, "CONIN$", "r", stdin);

    //std::cout << "Debug Console Opened!" << '\n';
}

void UEngine::InitRenderer()
{
	// 렌더러 초기화
	Renderer = std::make_unique<URenderer>();
	Renderer->Create(WindowHandle);
	Renderer->CreateShader();
	Renderer->CreateConstantBuffer();
}

void UEngine::InitWorld()
{
    World = FObjectFactory::ConstructObject<UWorld>();

    FEditorManager::Get().SetCamera(World->SpawnActor<ACamera>());

    FEditorManager::Get().SetWorldGrid(World->SpawnActor<AWorldGrid>());
    World->SpawnActor<AAxis>();
    World->SpawnActor<AAABBPicker>();

    //World->SpawnActor<ASphere>();
    //World->SpawnActor<ACube>();
    float OffsetY = 0;
    
    {
        ACube* Actor = World->SpawnActor<ACube>();
        UCubeComp* Cube = static_cast<UCubeComp*>(Actor->GetRootComponent());
        Actor->SetRootComponent(Cube);
        Cube->SetTexture(ETextureType::None);
        Cube->SetShaderType(EShaderType::PrimitiveShader);

        FTransform Transform;
        Transform.SetPosition(OffsetY, OffsetY, 0);
        Cube->SetRelativeTransform(Transform);
        OffsetY += 2;
    }

    {
        ACube* Actor = World->SpawnActor<ACube>();
        UCubeComp* Cube = static_cast<UCubeComp*>(Actor->GetRootComponent());
        Actor->SetRootComponent(Cube);
        Cube->SetTexture(ETextureType::CatTexture);
        Cube->SetShaderType(EShaderType::TextureShader);

        FTransform Transform;
        Transform.SetPosition(OffsetY, OffsetY, 0);
        Cube->SetRelativeTransform(Transform);
        OffsetY += 2;
    }

    {
        ACube* Actor = World->SpawnActor<ACube>();
        UCubeComp* Cube = static_cast<UCubeComp*>(Actor->GetRootComponent());
        Actor->SetRootComponent(Cube);
        Cube->SetTexture(ETextureType::Lenna);
        Cube->SetShaderType(EShaderType::TextureShader);

        FTransform Transform;
        Transform.SetPosition(OffsetY, OffsetY, 0);
        Cube->SetRelativeTransform(Transform);
        OffsetY += 2;
    }

    {
        ACube* Actor = World->SpawnActor<ACube>();
        UCubeComp* Cube = static_cast<UCubeComp*>(Actor->GetRootComponent());
        Actor->SetRootComponent(Cube);
        Cube->SetTexture(ETextureType::Cement);
        Cube->SetShaderType(EShaderType::TextureShader);

        FTransform Transform;
        Transform.SetPosition(OffsetY, OffsetY, 0);
        Cube->SetRelativeTransform(Transform);
        OffsetY += 2;
    }
    
    //// Test
    //World->SpawnActor<AArrow>();
    //World->SpawnActor<ABillboardText>();
    
    /* 빌보드 테스트 코드*/
    //World->SpawnActor<ABillboardText>();

    // for (int i =0 ; i < 1000; i++)
    // {
    //     World->SpawnActor<ACube>();
    // }

    // auto* Acube = World->SpawnActor<ACube>();
    // USceneComponent* Child = Acube->GetRootComponent(); 
    // for (int i = 0; i < 1; i++)
    // {
    //     auto* cube = Acube->AddComponent<UCubeComp>();
    //     FTransform Transform = FTransform();
    //     Transform.SetPosition(0, 1, 0);
    //     cube->SetRelativeTransform(Transform);
    //     cube->SetupAttachment(Child, EEndPlayReason::EAttachmentRule::KeepRelative);
    //     Child = cube;
    // }

    //World->SpawnActor<ASphere>();
    //World->SpawnActor<APicker>();
    /*auto* Actor = World->SpawnActor<ACylinder>();
    UConeComp* ConeComp = Actor->AddComponent<UConeComp>();
    ConeComp->SetRelativeTransform(FTransform(FVector(0.0f, 0.0f, 1.0f), FVector(), FVector(1.2f, 1.2f, 0.5f)));

    ConeComp->SetupAttachment(Actor->GetRootComponent(), EEndPlayReason::EAttachmentRule::KeepRelative);
	
    for (int i=0;i<10;i++)
    {
        UConeComp* ConeComp1 =  Actor->AddComponent<UConeComp>();
        
        ConeComp1->SetRelativeTransform(FTransform(FVector(0.0f, i, 1.0f), FVector(), FVector(1.2f, 1.2f, 0.5f)));
        ConeComp1->SetupAttachment(Actor->GetRootComponent(), EEndPlayReason::EAttachmentRule::KeepRelative);
    
        for (int j=0;j<10;j++)
        {
            UConeComp* ConeComp2 =  Actor->AddComponent<UConeComp>();
            ConeComp2->SetRelativeTransform(FTransform(FVector(0.0f, j * i, 1.0f), FVector(), FVector(1.2f, 1.2f, 0.5f)));
            ConeComp2->SetupAttachment(ConeComp1, EEndPlayReason::EAttachmentRule::KeepRelative);
        }
    }*/
    
    

	World->BeginPlay();
}

void UEngine::ShutdownWindow()
{
    DestroyWindow(WindowHandle);
    WindowHandle = nullptr;

    UnregisterClassW(WindowClassName, WindowInstance);
    WindowInstance = nullptr;

	ui.Shutdown();
}

void UEngine::UpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
	ScreenWidth = InScreenWidth;
	ScreenHeight = InScreenHeight;

    if(Renderer)
    {
        Renderer->OnUpdateWindowSize(ScreenWidth, ScreenHeight);
    }

	if (ui.bIsInitialized)
	{
		ui.OnUpdateWindowSize(ScreenWidth, ScreenHeight);
	}
}

UObject* UEngine::GetObjectByUUID(uint32 InUUID) const
{
    if (const auto Obj = GObjects.Find(InUUID))
    {
        return Obj->get();
    }
    return nullptr;
}

TMap<EEngineShowFlags, bool> UEngine::ShowFlagStates;

void UEngine::InitializeShowFlags()
{
    ShowFlagStates.Add(EEngineShowFlags::SF_Primitives, true);
    ShowFlagStates.Add(EEngineShowFlags::SF_Gizmo, true);
    ShowFlagStates.Add(EEngineShowFlags::SF_BillboardText, true);
}

//  Show Flag 설정
void UEngine::SetShowFlag(EEngineShowFlags Flag, bool bEnable)
{
    ShowFlagStates[Flag] = bEnable;
    //bShowPrimitives = bEnable;
    //Renderer->SetShowPrimitives(bShowPrimitives); //  렌더러에 전달
}

//  Show Flag 상태 확인
bool UEngine::IsShowFlagEnabled(EEngineShowFlags Flag) const
{
    return ShowFlagStates[Flag];
    /*
    if (Flag == EEngineShowFlags::SF_Primitives)
    {
        return ShowFlagStates[Flag];
        //return bShowPrimitives;
    }
    return false;*/
}

const TMap<EEngineShowFlags, bool>& UEngine::GetShowFlagStates() const
{
    return ShowFlagStates;
}
