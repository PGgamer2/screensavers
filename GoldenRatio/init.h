#pragma once

#include <iostream>
#include <chrono>
#include <thread>
#include <cstdint>
#include <windows.h>
using namespace std;
using namespace std::chrono;

#include <SDL.h>

enum windowType {
	ScreenSaver, SettingsDialog, PasswordDialog
};

// settings.cpp
long RegGetDword(HKEY hKey, string subKey, string name, DWORD* valuePtr);
long RegSetDword(HKEY hKey, string subKey, string name, DWORD value);
int initSettings(windowType* type, HWND* parent);

// screensaver.cpp
int initScreenSaver(HWND* parent);
