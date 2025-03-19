#pragma once
#include "Core/AbstractClass//Singleton.h"
#include <windows.h>
#include <stdio.h>

// INI 파일 경로
#define INI_FILE ".\\editor.ini"
#define SECTION "Settings"
#define KEY_CAMSENSITIVITY "CamSensitivity"
#define KEY_GRIDSPACING "GridSpacing"

class SettingManager : public TSingleton<SettingManager>{
public:
	float initCamSensitivity = 100.0f;			// 초기 감도
	float initGridSpacing = 20.0f;				// 초기 그리드 스페이싱.

public:
	float LoadCamSensitivty();
	void SaveCamSensitivty(float val);

	float LoadGridSpacing();
	void SaveGridSpacing(float val);
private:
	bool FileExists(const char* filename);
};