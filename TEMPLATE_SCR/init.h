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

// screensaver.cpp
int initScreenSaver(HWND* parent);

// settings.cpp
int initSettings(windowType* type, HWND* parent);
