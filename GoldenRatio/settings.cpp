#include "main.h"

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
		setSetting("Software\\GoldenRatio", "Squares", 1UL);
	} else if (clickedButton == 2) {
		setSetting("Software\\GoldenRatio", "Squares", 0UL);
	}
	
	return 0;
}