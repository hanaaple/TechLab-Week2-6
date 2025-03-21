﻿#include "UI.h"

#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_internal.h"
#include "Object/Actor/ATorus.h"

#include "Static/FEditorManager.h"

#include "Object/Actor/Sphere.h"
#include "Object/Actor/Cube.h"
#include "Object/Actor/Cone.h"
#include "Object/Actor/Cylinder.h"
#include "Object/Gizmo/EditorGizmos.h"
#include "Object/World/World.h"
#include <Object/Actor/BillBoardText.h>


void UI::Initialize(HWND hWnd, const URenderer& Renderer, UINT ScreenWidth, UINT ScreenHeight)
{
    // ImGui 초기화
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // 기본 폰트 크기 설정
    io.FontGlobalScale = 1.0f;
    io.DisplaySize = ScreenSize;
    //io.WantSetMousePos = true;
    // ImGui Backend 초기화
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(Renderer.GetDevice(), Renderer.GetDeviceContext());

    ScreenSize = ImVec2(static_cast<float>(ScreenWidth), static_cast<float>(ScreenHeight));
    bIsInitialized = true;

    io.DisplaySize = ScreenSize;
}

void UI::Update()
{
    // ImGui Frame 생성
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();


    RenderControlPanel();
    RenderPropertyWindow();
    RenderSceneManager();
    RenderSettingsPanel();
    Debug::ShowConsole();

    // ImGui 렌더링
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


void UI::Shutdown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void UI::OnUpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight)
{
    // ImGUI 리소스 다시 생성
    ImGui_ImplDX11_InvalidateDeviceObjects();
    ImGui_ImplDX11_CreateDeviceObjects();
    // ImGui 창 크기 업데이트
    ScreenSize = ImVec2(static_cast<float>(InScreenWidth), static_cast<float>(InScreenHeight));
}

void UI::RenderControlPanel()
{
    ImGui::Begin("Jungle Control Panel");

    ImGui::Text("Hello, Jungle World!");
    ImGui::Text("FPS: %.3f (what is that ms)", ImGui::GetIO().Framerate);

    RenderMemoryUsage();
    RenderPrimitiveSelection();
    RenderCameraSettings();

    ImGui::End();
}

void UI::RenderMemoryUsage()
{
    const uint64 ContainerAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Container>();
    const uint64 ContainerAllocCount = FPlatformMemory::GetAllocationCount<EAT_Container>();
    const uint64 ObjectAllocByte = FPlatformMemory::GetAllocationBytes<EAT_Object>();
    const uint64 ObjectAllocCount = FPlatformMemory::GetAllocationCount<EAT_Object>();
    ImGui::Text(
        "Container Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte,
        ContainerAllocCount
    );
    ImGui::Text(
        "Object Memory Uses: %llubyte, Count: %llu Objects",
        ObjectAllocByte,
        ObjectAllocCount
    );
    ImGui::Text(
        "Total Memory Uses: %llubyte, Count: %llu",
        ContainerAllocByte + ObjectAllocByte,
        ContainerAllocCount + ObjectAllocCount
    );

    ImGui::Separator();
}

void UI::RenderPrimitiveSelection()
{
    const char* items[] = {"Sphere", "Cube", "Cylinder", "Cone", "Torus", "BillboardText"};

    ImGui::Combo("Primitive", &currentItem, items, IM_ARRAYSIZE(items));

    if (ImGui::Button("Spawn"))
    {
        UWorld* World = UEngine::Get().GetWorld();
        for (int i = 0; i < NumOfSpawn; i++)
        {
            if (strcmp(items[currentItem], "Sphere") == 0)
            {
                World->SpawnActor<ASphere>();
            }
            else if (strcmp(items[currentItem], "Cube") == 0)
            {
                World->SpawnActor<ACube>();
            }
            else if (strcmp(items[currentItem], "Cylinder") == 0)
            {
                World->SpawnActor<ACylinder>();
            }
            else if (strcmp(items[currentItem], "Cone") == 0)
            {
                World->SpawnActor<ACone>();
            }
            else if (strcmp(items[currentItem], "BillboardText") == 0) {
                World->SpawnActor<ABillboardText>();
            }
            else if (strcmp(items[currentItem], "Torus") == 0)
            {
                World->SpawnActor<ATorus>();
            }
            //else if (strcmp(items[currentItem], "Triangle") == 0)
            //{
            //    Actor->AddComponent<UTriangleComp>();   
            //}
        }
    }
    ImGui::SameLine();
    ImGui::InputInt("Number of spawn", &NumOfSpawn, 0);

    ImGui::Separator();

    UWorld* World = UEngine::Get().GetWorld();
    uint32 bufferSize = 100;
    char* SceneNameInput = new char[bufferSize];
    strcpy_s(SceneNameInput, bufferSize, *World->SceneName);

    if (ImGui::InputText("Scene Name", SceneNameInput, bufferSize))
    {
        World->SceneName = SceneNameInput;
    }

    if (ImGui::Button("New Scene"))
    {
        World->ClearWorld();
    }
    if (ImGui::Button("Save Scene"))
    {
        World->SaveWorld();
    }
    if (ImGui::Button("Load Scene"))
    {
        World->LoadWorld(SceneNameInput);
    }
    ImGui::Separator();
}

void UI::RenderCameraSettings()
{
    ImGui::Text("Camera");

    ACamera* Camera = FEditorManager::Get().GetCamera();

    bool IsOrthogonal;
    if (Camera->ProjectionMode == ECameraProjectionMode::Orthographic)
    {
        IsOrthogonal = true;
    }
    else if (Camera->ProjectionMode == ECameraProjectionMode::Perspective)
    {
        IsOrthogonal = false;
    }

    if (ImGui::Checkbox("Orthogonal", &IsOrthogonal))
    {
        if (IsOrthogonal)
        {
            Camera->ProjectionMode = ECameraProjectionMode::Orthographic;
        }
        else
        {
            Camera->ProjectionMode = ECameraProjectionMode::Perspective;
        }
    }

    float FOV = Camera->GetFieldOfView();
    if (ImGui::DragFloat("FOV", &FOV, 0.1f))
    {
        FOV = std::clamp(FOV, 0.01f, 179.99f);
        Camera->SetFieldOfVew(FOV);
    }

    float NearFar[2] = {Camera->GetNear(), Camera->GetFar()};
    if (ImGui::DragFloat2("Near, Far", NearFar, 0.1f))
    {
        NearFar[0] = FMath::Max(0.01f, NearFar[0]);
        NearFar[1] = FMath::Max(0.01f, NearFar[1]);

        if (NearFar[0] < NearFar[1])
        {
            Camera->SetNear(NearFar[0]);
            Camera->SetFar(NearFar[1]);
        }
        else
        {
            if (abs(NearFar[0] - Camera->GetNear()) < 0.00001f)
            {
                Camera->SetFar(NearFar[0] + 0.01f);
            }
            else if (abs(NearFar[1] - Camera->GetFar()) < 0.00001f)
            {
                Camera->SetNear(NearFar[1] - 0.01f);
            }
        }
    }

    FVector CameraPosition = Camera->GetActorTransform().GetPosition();
    if (ImGui::DragFloat3("Camera Location", reinterpret_cast<float*>(&CameraPosition), 0.1f))
    {
        FTransform Trans = Camera->GetActorTransform();
        Trans.SetPosition(CameraPosition);
        Camera->SetActorTransform(Trans);
    }

    FVector PrevEulerAngle = Camera->GetActorTransform().GetEulerRotation();
    FVector UIEulerAngle = {PrevEulerAngle.X, PrevEulerAngle.Y, PrevEulerAngle.Z};
    if (ImGui::DragFloat3("Camera Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
    {
        FTransform Transform = Camera->GetActorTransform();

        //FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;
        //Transform.Rotate(DeltaEulerAngle);

        UIEulerAngle.Y = FMath::Clamp(UIEulerAngle.Y, -Camera->MaxYDegree, Camera->MaxYDegree);
        Transform.SetRotation(UIEulerAngle);
        Camera->SetActorTransform(Transform);
    }
    if (ImGui::DragFloat("Camera Speed", &Camera->CameraSpeed, 0.1f))
    {
        SettingManager::Get().SaveCamSensitivty(Camera->CameraSpeed);
    }

    FVector Forward = Camera->GetActorTransform().GetForward();
    FVector Up = Camera->GetActorTransform().GetUp();
    FVector Right = Camera->GetActorTransform().GetRight();

    ImGui::Text("Camera GetForward(): (%.2f %.2f %.2f)", Forward.X, Forward.Y, Forward.Z);
    ImGui::Text("Camera GetUp(): (%.2f %.2f %.2f)", Up.X, Up.Y, Up.Z);
    ImGui::Text("Camera GetRight(): (%.2f %.2f %.2f)", Right.X, Right.Y, Right.Z);
}

void UI::RenderSettingsPanel()
{
    ImGui::Begin("Render Settings");

    // 2개의 컬럼 생성 (가운데 세로줄 포함)
    ImGui::Columns(2, nullptr, true);

    //첫 번째 컬럼 (View Mode 및 Grid Spacing)
    ImGui::Text("View Mode");
    ImGui::NextColumn(); // 다음 컬럼 이동

    //두 번째 컬럼 (Show Flags)
    ImGui::Text("Show Flags");
    ImGui::NextColumn(); // 다음 컬럼 이동

    //View Mode 드롭다운 (좌측 컬럼)
    ImGui::PushItemWidth(-1);
    static const char* ViewModeNames[] = {"Lit", "Unlit", "Wireframe"};
    static int CurrentViewMode = (UEngine::Get().GetRenderer()->CurrentViewMode);

    if (ImGui::Combo("##ViewMode", &CurrentViewMode, ViewModeNames, IM_ARRAYSIZE(ViewModeNames)))
    {
        UEngine::Get().GetRenderer()->SetFillMode(static_cast<D3D11_FILL_MODE>(CurrentViewMode));
    }
    ImGui::PopItemWidth();
    ImGui::NextColumn();

    //Show Flag 체크박스 (우측 컬럼)
    const auto& ShowFlagStates = UEngine::Get().GetShowFlagStates();
    for (auto& [Flag, bEnabled] : ShowFlagStates)
    {
        const char* FlagName = nullptr;
        bool bChecked = bEnabled;

        switch (Flag)
        {
        case EEngineShowFlags::SF_Primitives:
            FlagName = "Primitives";
            break;
        case EEngineShowFlags::SF_Gizmo:
            FlagName = "Gizmo";
            break;
        case EEngineShowFlags::SF_BillboardText:
            FlagName = "Billboard Text";
            break;
        }

        if (ImGui::Checkbox(FlagName, &bChecked))
        {
            UEngine::Get().SetShowFlag(Flag, bChecked);
            //Primitives 비활성화 + Gizmo가 아닌 경우 렌더링 X
            //Gizmo 비활성화 + Gizmo인 경우 렌더링 X
            //BillboardText 비활성화 + BillboardText인 경우 렌더링 X
            if (strcmp(FlagName, "Primitives") == 0)
            {
                UWorld* world = UEngine::Get().GetWorld();
                for (auto actor : world->GetActors())
                {
                    for (auto* component : actor->GetComponents())
                    {
                        if (component->IsA<UPrimitiveComponent>() && !actor->IsGizmoActor())
                        {
                            UPrimitiveComponent* Comp = static_cast<UPrimitiveComponent*>(component);
                            Comp->SetVisibility(bChecked);
                        }
                    } 
                }
            }

            if (strcmp(FlagName, "Gizmo") == 0)
            {
                UWorld* world = UEngine::Get().GetWorld();
                for (auto actor : world->GetActors())
                {
                    for (auto* component : actor->GetComponents())
                    {
                        if (component->IsA<UPrimitiveComponent>() && actor->IsGizmoActor())
                        {
                            UPrimitiveComponent* Comp = static_cast<UPrimitiveComponent*>(component);
                            Comp->SetVisibility(bChecked);
                        }
                    } 
                }
            }

            if (strcmp(FlagName, "Billboard Text") == 0)
            {
                UWorld* world = UEngine::Get().GetWorld();
                for (auto actor : world->GetActors())
                {
                    if (actor->IsA<ABillboardText>())
                    {
                        for (auto* component : actor->GetComponents())
                        {
                            if (component->IsA<UPrimitiveComponent>())
                            {
                                UPrimitiveComponent* Comp = static_cast<UPrimitiveComponent*>(component);
                                Comp->SetVisibility(bChecked);
                            }
                        }
                    }
                }
            }
        }
    }

    //체크박스 목록 아래 가로 구분선 추가
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    //Grid Spacing UI (양쪽 컬럼을 합쳐 넓게 표시)
    ImGui::Columns(1); // 컬럼 분할 해제 → 전체 너비 사용

    static float GridSpacing = SettingManager::Get().LoadGridSpacing();
    
    ImGui::Text("Grid Spacing");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-1);
    if (ImGui::SliderFloat("##GridSpacing", &GridSpacing, 1.f, 50.0f, "%.2f"))
    {
        FEditorManager::Get().GetWorldGrid()->SetSpacing(GridSpacing);
        SettingManager::Get().SaveGridSpacing(GridSpacing);
    }

    ImGui::End();
}

void UI::RenderPropertyWindow()
{
    ImGui::Begin("Properties");

    AActor* selectedActor = FEditorManager::Get().GetSelectedActor();
    if (selectedActor != nullptr)
    {
        FTransform selectedTransform = selectedActor->GetActorTransform();
 
        /*UPrimitiveComponent* component = dynamic_cast<UPrimitiveComponent*>(selectedActor->GetRootComponent());
        float xAxis[] = {
            component->obb.axis[0].X,
            component->obb.axis[0].Y,
            component->obb.axis[0].Z
        };
        float yAxis[] = {
            component->obb.axis[1].X,
            component->obb.axis[1].Y,
            component->obb.axis[1].Z
        };
        float zAxis[] = {
            component->obb.axis[2].X,
            component->obb.axis[2].Y,
            component->obb.axis[2].Z
        };
        float center[] = {
            component->obb.Center.X,
            component->obb.Center.Y,
            component->obb.Center.Z
        };
        float halfSize[] = {
            component->obb.halfSize[0],
            component->obb.halfSize[1],
            component->obb.halfSize[2]
        };

        ImGui::InputFloat3("x axis", xAxis, "%.3f");
        ImGui::InputFloat3("y axis", yAxis, "%.3f");
        ImGui::InputFloat3("z axis", zAxis, "%.3f");
        ImGui::InputFloat3("center", center, "%.3f");
        ImGui::InputFloat3("half", halfSize, "%.3f");*/
        // 선택된 오브젝트의 이름을 표시하고 변경 가능하도록 함
        uint32 bufferSize = 100;
        char SceneNameInput[100];
        strcpy_s(SceneNameInput, bufferSize, selectedActor->GetName().ToString().ToStdString().c_str());

        if (ImGui::InputText("Object Name", SceneNameInput, bufferSize))
        {
            selectedActor->SetName(FName(SceneNameInput));
        }

        float position[] = {
            selectedTransform.GetPosition().X, selectedTransform.GetPosition().Y, selectedTransform.GetPosition().Z
        };
        float scale[] = {
            selectedTransform.GetScale().X, selectedTransform.GetScale().Y, selectedTransform.GetScale().Z
        };

        if (ImGui::DragFloat3("Translation", position, 0.1f))
        {
            selectedTransform.SetPosition(position[0], position[1], position[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }

        FVector PrevEulerAngle = selectedTransform.GetEulerRotation();
        FVector UIEulerAngle = PrevEulerAngle;
        if (ImGui::DragFloat3("Rotation", reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
        {
            FVector DeltaEulerAngle = UIEulerAngle - PrevEulerAngle;
            selectedTransform.Rotate(DeltaEulerAngle);
            selectedActor->SetActorTransform(selectedTransform);
        }

        if (ImGui::DragFloat3("Scale", scale, 0.1f))
        {
            selectedTransform.SetScale(scale[0], scale[1], scale[2]);
            selectedActor->SetActorTransform(selectedTransform);
        }

        if (FEditorManager::Get().GetGizmoHandle() != nullptr)
        {
            const char* GizmoTypeStr = (FEditorManager::Get().GetGizmoType() == EGizmoType::Translate)
                                           ? "Translate"
                                           : (FEditorManager::Get().GetGizmoType() == EGizmoType::Rotate)
                                           ? "Rotate"
                                           : "Scale";
            ImGui::Text("GizmoType: %s", GizmoTypeStr);
        }
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        //  계층 구조로 자식 트리 출력
        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Children");

        if (USceneComponent* RootComponent = selectedActor->GetRootComponent())
        {
            RenderComponentTree(RootComponent, true,
                                true, ImGuiTreeNodeFlags_OpenOnArrow);
            //RenderComponentTree(RootComponent, true);  // Transform UI 표시 활성화
        }
    }

    ImGui::End();
}

void UI::RenderSceneManager()
{
    ImGui::Begin("Scene Manager");

    UWorld* World = UEngine::Get().GetWorld();
    if (!World)
    {
        ImGui::Text("World is NULL");
        ImGui::End();
        return;
    }

    //  씬의 모든 액터 가져오기
    const TArray<AActor*>& Actors = World->GetActors();
    AActor* selectedActor = FEditorManager::Get().GetSelectedActor();

    if (ImGui::CollapsingHeader("Primitives", ImGuiTreeNodeFlags_DefaultOpen))
    {
        for (AActor* Actor : Actors)
        {
            if (Actor->IsGizmoActor()) continue;
            ImGui::PushID(Actor->GetUUID());

            ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;
            if (Actor == selectedActor)
                nodeFlags |= ImGuiTreeNodeFlags_Selected;

            bool nodeOpen = ImGui::TreeNodeEx(*Actor->GetName().ToString(), nodeFlags);

            if (ImGui::IsItemClicked())
            {
                selectedActor = Actor;
            }

            if (nodeOpen)
            {
                RenderComponentTree(Actor->GetRootComponent(), false,
                                    true, ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen);
                // Transform UI 표시 X
                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        FEditorManager::Get().SelectActor(selectedActor);
    }

    ImGui::End();
}

void UI::RenderComponentTree(USceneComponent* Component, bool bShowTransform, bool bShowUUID, ImGuiTreeNodeFlags nodeFlags)
{
    if (!Component) return;

    //ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow;
    FString ChildLabel = Component->GetName().ToString();
    if (bShowUUID) ChildLabel += ("/" + FString::FromInt(Component->GetUUID()));

    ImGui::PushID(Component->GetUUID());
    bool bNodeOpen = ImGui::TreeNodeEx(*ChildLabel, nodeFlags);

    if (bNodeOpen)
    {
        if (bShowTransform)
        {
            {
                FTransform ComponentTransform = Component->GetRelativeTransform();
                float position[] = {
                    ComponentTransform.GetPosition().X, ComponentTransform.GetPosition().Y,
                    ComponentTransform.GetPosition().Z
                };
                float scale[] = {
                    ComponentTransform.GetScale().X, ComponentTransform.GetScale().Y, ComponentTransform.GetScale().Z
                };

                
                uint32 bufferSize = 100;
                char ComponentNameInput[100];
                strcpy_s(ComponentNameInput, bufferSize, Component->GetName().ToString().ToStdString().c_str());

                if (ImGui::InputText("Component Name", ComponentNameInput, bufferSize))
                {
                    Component->SetName(FName(ComponentNameInput));
                }
                
                ImGui::Text("Local", Component->GetUUID());
                if (ImGui::DragFloat3(("Translation##" + std::to_string(Component->GetUUID())).c_str(), position, 0.1f))
                {
                    ComponentTransform.SetPosition(position[0], position[1], position[2]);
                    Component->SetRelativeTransform(ComponentTransform);
                }

                FVector PrevEulerAngle = ComponentTransform.GetEulerRotation();
                FVector UIEulerAngle = PrevEulerAngle;
                if (ImGui::DragFloat3(("Rotation##" + std::to_string(Component->GetUUID())).c_str(),
                                      reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
                {
                    ComponentTransform.SetRotation(UIEulerAngle);
                    Component->SetRelativeTransform(ComponentTransform);
                }

                if (ImGui::DragFloat3(("Scale##" + std::to_string(Component->GetUUID())).c_str(), scale, 0.1f))
                {
                    ComponentTransform.SetScale(scale[0], scale[1], scale[2]);
                    Component->SetRelativeTransform(ComponentTransform);
                }
            }
            ImGui::Separator();
            {
                FTransform ComponentTransform = Component->GetComponentTransform();
                float position[] = {
                    ComponentTransform.GetPosition().X, ComponentTransform.GetPosition().Y,
                    ComponentTransform.GetPosition().Z
                };
                float scale[] = {
                    ComponentTransform.GetScale().X, ComponentTransform.GetScale().Y, ComponentTransform.GetScale().Z
                };
                
                
                ImGui::Text("World", Component->GetUUID());
                

                if (ImGui::DragFloat3(("World Translation##" + std::to_string(Component->GetUUID())).c_str(), position, 0.1f))
                {
                    ComponentTransform.SetPosition(position[0], position[1], position[2]);
                    Component->SetWorldTransform(ComponentTransform);
                }

                FVector PrevEulerAngle = ComponentTransform.GetEulerRotation();
                FVector UIEulerAngle = PrevEulerAngle;
                if (ImGui::DragFloat3(("World Rotation##" + std::to_string(Component->GetUUID())).c_str(),
                                      reinterpret_cast<float*>(&UIEulerAngle), 0.1f))
                {
                    ComponentTransform.SetRotation(UIEulerAngle);
                    Component->SetWorldTransform(ComponentTransform);
                }

                if (ImGui::DragFloat3(("World Scale##" + std::to_string(Component->GetUUID())).c_str(), scale, 0.1f))
                {
                    ComponentTransform.SetScale(scale[0], scale[1], scale[2]);
                    Component->SetWorldTransform(ComponentTransform);
                }
                
            
                // ImGui::Text("World Position %f, %f, %f",  ComponentTransform.GetPosition().X, ComponentTransform.GetPosition().Y, ComponentTransform.GetPosition().Z);
                // ImGui::Text("World Rotation %f, %f, %f", ComponentTransform.GetEulerRotation().X, ComponentTransform.GetEulerRotation().Y, ComponentTransform.GetEulerRotation().Z);
                // ImGui::Text("World Scale %f, %f, %f", ComponentTransform.GetScale().X, ComponentTransform.GetScale().Y, ComponentTransform.GetScale().Z);

            }
            if (Component->IsA<UPrimitiveComponent>())
            {
                auto* PrimitiveComponent = dynamic_cast<UPrimitiveComponent*>(Component);
                bool bisBatch = false;
                if (PrimitiveComponent->GetRenderMode() == ERenderMode::Batch)
                {
                    bisBatch = true;
                }
                if (ImGui::Checkbox("IsBatch", &bisBatch))
                {
                    if (bisBatch)
                        PrimitiveComponent->SetRenderMode(Batch);
                    else
                        PrimitiveComponent->SetRenderMode(Individual);
                }
                
                static const char* TopologyItems[] = {"LineList", "Triangle", "POINTLIST"};
                int curItem = -1;
                if (PrimitiveComponent->GetTopology() == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
                    curItem = 0;
                else if (PrimitiveComponent->GetTopology() == D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
                    curItem = 1;
                else if (PrimitiveComponent->GetTopology() == D3D11_PRIMITIVE_TOPOLOGY_POINTLIST)
                    curItem = 2;
                
                if (ImGui::Combo("##Topology", &curItem, TopologyItems, IM_ARRAYSIZE(TopologyItems)))
                {
                    if (FName(TopologyItems[curItem]) == FName("LineList"))
                    {
                        PrimitiveComponent->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
                    } else if (FName(TopologyItems[curItem]) == FName("Triangle"))
                    {
                        PrimitiveComponent->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    }else if (FName(TopologyItems[curItem]) == FName("POINTLIST"))
                    {
                        PrimitiveComponent->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
                    }
                }
                // Mesh
                {
                    
                    static const char* MeshItem[] = {"EPT_Triangle", "EPT_Cube (UV)", "EPT_Sphere", "EPT_Line", "EPT_BoundingBox", "EPT_GridLine", "EPT_Cylinder", "EPT_Quad (UV)", "EPT_Cone", "EPT_Torus"};
                    int curMeshItem = -1;
                    if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_Triangle)
                        curMeshItem = 0;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_Cube)
                        curMeshItem = 1;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_Sphere)
                        curMeshItem = 2;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_Line)
                        curMeshItem = 3;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_BoundingBox)
                        curMeshItem = 4;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_GridLine)
                        curMeshItem = 5;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_Cylinder)
                        curMeshItem = 6;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_Quad)
                        curMeshItem = 7;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_Cone)
                        curMeshItem = 8;
                    else if (PrimitiveComponent->GetMeshType() == EPrimitiveMeshType::EPT_Torus)
                        curMeshItem = 9;
                
                    if (ImGui::Combo("##Mesh", &curMeshItem, MeshItem, IM_ARRAYSIZE(MeshItem)))
                    {
                        if (FName(MeshItem[curMeshItem]) == FName("EPT_Triangle"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_Triangle);
                        } else if (FName(MeshItem[curMeshItem]) == FName("EPT_Cube (UV)"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_Cube);
                        }else if (FName(MeshItem[curMeshItem]) == FName("EPT_Sphere"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_Sphere);
                        }
                        else if (FName(MeshItem[curMeshItem]) == FName("EPT_Line"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_Line);
                        }
                        else if (FName(MeshItem[curMeshItem]) == FName("EPT_BoundingBox"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_BoundingBox);
                        }
                        else if (FName(MeshItem[curMeshItem]) == FName("EPT_GridLine"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_GridLine);
                        }
                        else if (FName(MeshItem[curMeshItem]) == FName("EPT_Cylinder"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_Cylinder);
                        }
                        else if (FName(MeshItem[curMeshItem]) == FName("EPT_Quad (UV)"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_Quad);
                        }
                        else if (FName(MeshItem[curMeshItem]) == FName("EPT_Cone"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_Cone);
                        }
                        else if (FName(MeshItem[curMeshItem]) == FName("EPT_Torus"))
                        {
                            PrimitiveComponent->SetMesh(EPrimitiveMeshType::EPT_Torus);
                        }
                    
                    }
                }

                {
                    static const char* TextureItems[] = {"None", "FontATexture", "AtlasFontTexture", "CatTexture", "Lenna", "Cement"};
                    int curTextureItem = -1;
                    if (PrimitiveComponent->GetTexture() == ETextureType::None)
                        curTextureItem = 0;
                    else if (PrimitiveComponent->GetTexture() == ETextureType::FontATexture)
                        curTextureItem = 1;
                    else if (PrimitiveComponent->GetTexture() == ETextureType::AtlasFontTexture)
                        curTextureItem = 2;
                    else if (PrimitiveComponent->GetTexture() == ETextureType::CatTexture)
                        curTextureItem = 3;
                    else if (PrimitiveComponent->GetTexture() == ETextureType::Lenna)
                        curTextureItem = 4;
                    else if (PrimitiveComponent->GetTexture() == ETextureType::Cement)
                        curTextureItem = 5;
                    
                    if (ImGui::Combo("##Texture", &curTextureItem, TextureItems, IM_ARRAYSIZE(TextureItems)))
                    {
                        if (FName(TextureItems[curTextureItem]) == FName("None"))
                        {
                            PrimitiveComponent->SetTexture(ETextureType::None);
                        } else if (FName(TextureItems[curTextureItem]) == FName("FontATexture"))
                        {
                            PrimitiveComponent->SetTexture(ETextureType::FontATexture);
                        }else if (FName(TextureItems[curTextureItem]) == FName("AtlasFontTexture"))
                        {
                            PrimitiveComponent->SetTexture(ETextureType::AtlasFontTexture);
                        }
                        else if (FName(TextureItems[curTextureItem]) == FName("CatTexture"))
                        {
                            PrimitiveComponent->SetTexture(ETextureType::CatTexture);
                        }
                        else if (FName(TextureItems[curTextureItem]) == FName("Lenna"))
                        {
                            PrimitiveComponent->SetTexture(ETextureType::Lenna);
                        }
                        else if (FName(TextureItems[curTextureItem]) == FName("Cement"))
                        {
                            PrimitiveComponent->SetTexture(ETextureType::Cement);
                        }
                    }
                }

                {
                    static const char* ShaderItems[] = {"DefaultShader", "TextureShader", "PrimitiveShader"};
                    int curShaderIndex = -1;
                    if (PrimitiveComponent->GetShaderType() == EShaderType::DefaultShader)
                        curShaderIndex = 0;
                    else if (PrimitiveComponent->GetShaderType() == EShaderType::TextureShader)
                        curShaderIndex = 1;
                    else if (PrimitiveComponent->GetShaderType() == EShaderType::PrimitiveShader)
                        curShaderIndex = 2;
                    
                    if (ImGui::Combo("##Shader", &curShaderIndex, ShaderItems, IM_ARRAYSIZE(ShaderItems)))
                    {
                        if (FName(ShaderItems[curShaderIndex]) == FName("DefaultShader"))
                        {
                            PrimitiveComponent->SetShaderType(EShaderType::DefaultShader);
                        } else if (FName(ShaderItems[curShaderIndex]) == FName("TextureShader"))
                        {
                            PrimitiveComponent->SetShaderType(EShaderType::TextureShader);
                        }else if (FName(ShaderItems[curShaderIndex]) == FName("PrimitiveShader"))
                        {
                            PrimitiveComponent->SetShaderType(EShaderType::PrimitiveShader);
                        }
                    }
                }
            }
        }
        ImGui::Separator();
        for (auto* Child : Component->GetAttachChildren())
        {
            if (!bShowTransform && Child->GetAttachChildren().Num() == 0)nodeFlags |= ImGuiTreeNodeFlags_Leaf;
            else nodeFlags &= ~ImGuiTreeNodeFlags_Leaf;
            if (Child)
                RenderComponentTree(Child, bShowTransform, bShowUUID, nodeFlags);
        }

        ImGui::TreePop();
    }

    ImGui::PopID();
}
