#include "main.h"

unsigned long getSetting(string subKey, string name, unsigned long* valuePtr) {
	unsigned long errCode = 1UL;
#ifdef _WIN32
	DWORD dataSize = sizeof(*valuePtr);
	errCode = RegGetValue(HKEY_CURRENT_USER, subKey.c_str(), name.c_str(), RRF_RT_REG_DWORD, nullptr, valuePtr, &dataSize);
#endif
	return errCode;
}

unsigned long setSetting(string subKey, string name, unsigned long value) {
	unsigned long errCode = 1UL;
#ifdef _WIN32
	HKEY hKey;
	errCode = RegCreateKeyEx(HKEY_CURRENT_USER, subKey.c_str(), 0, NULL, NULL, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	if (errCode != ERROR_SUCCESS) return errCode;
	errCode = RegSetValueEx(hKey, name.c_str(), 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
	RegCloseKey(hKey);
#endif
	return errCode;
}

int getWindowSize(HWND handle, SDL_Rect* rect) {
	int success = 1;
#ifdef _WIN32
	RECT winRect;
	success = GetWindowRect(handle, &winRect);
	rect->x = winRect.left;
	rect->y = winRect.top;
	success = GetClientRect(handle, &winRect);
	rect->w = winRect.right;
	rect->h = winRect.bottom;
#endif
	return success;
}

int getDesktopSize(SDL_Rect* rect) {
	int success = 1;
	rect->x = 0;
	rect->y = 0;
#ifdef _WIN32
	RECT winRect;
	success = GetWindowRect(GetDesktopWindow(), &winRect);
	rect->w = winRect.right;
	rect->h = winRect.bottom;
#elif defined(__linux__)
	struct fb_var_screeninfo fb_var;
	int fd = open("/dev/graphics/fb0", O_RDONLY);
	ioctl(fd, FBIOGET_VSCREENINFO, &fb_var);
	close(fd);
	rect->w = fb_var.xres;
	rect->h = fb_var.yres;
#endif
	return success;
}

HWND getForegroundWin() {
	HWND win = NULL;
#ifdef _WIN32
	win = GetForegroundWindow();
#endif
	return win;
}