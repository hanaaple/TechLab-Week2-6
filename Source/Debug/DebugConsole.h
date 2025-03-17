#pragma once
#include <vector>

#include "Core/Container/String.h"
#include "ImGui/imgui.h"

#define UE_LOG(format, ...) Debug::Log(format, ##__VA_ARGS__)


class FName;
class FString;

class Debug
{
    static std::vector<FString> items; // 출력 로그

public:
    static void ShowConsole();
    static void ProcessCommand(const FString& command, std::vector<FString>& log);
    static void Log(const char* format, ...);
    static void Log(const FName& name);
    //UE_LOG({FName("Apple"),FName("Is")})
    static void Log(std::initializer_list<FName> names, const FString* separator=nullptr);
    static ImVec2 ResizeToScreen(const ImVec2& vec2, ImVec2 PreRatio, ImVec2 CurRatio);
};
