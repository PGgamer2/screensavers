#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#else
#ifdef __linux__
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif
typedef void* HWND;
#endif

using namespace std;
using namespace std::chrono;

#include <SDL.h>

enum windowType {
	ScreenSaver, SettingsDialog, PasswordDialog
};

// win_compat.cpp
unsigned long getSetting(string subKey, string name, unsigned long* valuePtr);
unsigned long setSetting(string subKey, string name, unsigned long value);
int getWindowSize(HWND handle, SDL_Rect* rect);
int getDesktopSize(SDL_Rect* rect);
HWND getForegroundWin();

// settings.cpp
int initSettings(windowType* type, HWND* parent);

// screensaver.cpp
int initScreenSaver(HWND* parent);
