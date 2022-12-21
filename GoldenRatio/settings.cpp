#include "init.h"

long RegGetDword(HKEY hKey, string subKey, string name, DWORD* valuePtr) {
	DWORD dataSize = sizeof(*valuePtr);
	return RegGetValue(hKey, subKey.c_str(), name.c_str(), RRF_RT_REG_DWORD, nullptr, valuePtr, &dataSize);
}

long RegSetDword(HKEY hKeyCat, string subKey, string name, DWORD value) {
	HKEY hKey;
	long errCode = RegCreateKeyEx(hKeyCat, subKey.c_str(), 0, NULL, NULL, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (errCode != ERROR_SUCCESS) return errCode;
	errCode = RegSetValueEx(hKey, name.c_str(), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
	RegCloseKey(hKey);
	return errCode;
}

int initSettings(windowType* type, HWND* parent) {
	if (*type == ScreenSaver) return -1;
	int clickedButton = 0;
	SDL_MessageBoxData messageBoxData;
	messageBoxData.title = "ScreenSaver Settings";
	messageBoxData.window = SDL_CreateWindowFrom(*parent);

	messageBoxData.numbuttons = 2;
	SDL_MessageBoxButtonData buttons[2];
	buttons[0].buttonid = 1;
	buttons[0].text = "Yes";
	buttons[1].buttonid = 2;
	buttons[1].text = "No";
	messageBoxData.buttons = buttons;
	messageBoxData.message = "Show squares in the spiral?";
	if (SDL_ShowMessageBox(&messageBoxData, &clickedButton) != 0) return -1;
	if (clickedButton == 1) {
		RegSetDword(HKEY_CURRENT_USER, "Software\\GoldenRatio", "Squares", (DWORD)1);
	} else if (clickedButton == 2) {
		RegSetDword(HKEY_CURRENT_USER, "Software\\GoldenRatio", "Squares", (DWORD)0);
	}
	
	return 0;
}