#include "main.h"

int initSettings(windowType* type, HWND* parent) {
	if (*type == ScreenSaver) return -1;
	int clickedButton = 0;
	SDL_MessageBoxData messageBoxData;
	messageBoxData.title = "ScreenSaver Settings";
	messageBoxData.window = SDL_CreateWindowFrom(*parent);

	messageBoxData.numbuttons = 1;
	SDL_MessageBoxButtonData button;
	button.buttonid = 0;
	button.text = "Ok";
	button.flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
	messageBoxData.buttons = &button;
	messageBoxData.message = "There are no settings for this screensaver";
	if (SDL_ShowMessageBox(&messageBoxData, &clickedButton) != 0) return -1;

	return 0;
}