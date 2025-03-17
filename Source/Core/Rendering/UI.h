#pragma once
#define _TCHAR_DEFINED
#include <Windows.h>

#include "ImGui/imgui.h"

#include "URenderer.h"

class USceneComponent;

class UI
{
public:
    int currentItem = 0;
    int NumOfSpawn = 0;
    bool bIsInitialized = false;

    void Initialize(HWND hWnd, const class URenderer& Renderer, UINT ScreenWidth, UINT ScreenHeight);
    void Update();
    void Shutdown();

    void OnUpdateWindowSize(UINT InScreenWidth, UINT InScreenHeight);

public: // UIWindows
    void RenderControlPanel();
    void RenderMemoryUsage();
    void RenderPrimitiveSelection();
    void RenderCameraSettings();
    void RenderPropertyWindow();
    void RenderSceneManager();
    void RenderSettingsPanel();

private:

    void RenderComponentTree(USceneComponent* Component, bool bShowTransform,
                             bool bShowUUID, ImGuiTreeNodeFlags nodeFlags);


    //bool bWasWindowSizeUpdated = true;

    ImVec2 ScreenSize;
    //ImVec2 InitialScreenSize;

    //ImVec2 PreRatio;
    //ImVec2 CurRatio;
};
