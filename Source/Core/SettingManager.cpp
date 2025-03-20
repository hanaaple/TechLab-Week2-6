#include "SettingManager.h"

bool SettingManager::FileExists(const char* filename) {
	DWORD fileAttr = GetFileAttributesA(filename);
	return (fileAttr != INVALID_FILE_ATTRIBUTES && !(fileAttr & FILE_ATTRIBUTE_DIRECTORY));

}

float SettingManager::LoadCamSensitivty() {
	//if (!FileExists(INI_FILE)) {
	//	// ini 파일이 없으면 새로 저장.
	//	SaveCamSensitivty(initCamSensitivity);
	//}
	return GetPrivateProfileIntA(SECTION, KEY_CAMSENSITIVITY, initCamSensitivity, INI_FILE) / 100.0f;
}

void SettingManager::SaveCamSensitivty(float val) {
	char value[32];
	sprintf_s(value, sizeof(value), "%d", static_cast<int>(val * 100));
	WritePrivateProfileStringA(SECTION, KEY_CAMSENSITIVITY, value, INI_FILE);
}

float SettingManager:: LoadGridSpacing(){
	//if (!FileExists(INI_FILE)) {
	//	// ini 파일이 없으면 새로 저장.
	//	SaveGridSpacing(initGridSpacing);
	//}
	return GetPrivateProfileIntA(SECTION, KEY_GRIDSPACING, initGridSpacing, INI_FILE) / 100.0f;
}

void SettingManager::SaveGridSpacing(float val) {
	char value[32];
	sprintf_s(value, sizeof(value), "%d", static_cast<int>(val * 100));
	WritePrivateProfileStringA(SECTION, KEY_GRIDSPACING, value, INI_FILE);
}