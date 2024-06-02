#include "main.h"

int main(int argc, char* args[]) {
	HWND parent = NULL;
	windowType mode = ScreenSaver;
	bool waitingForHandle = false;

	for (int i = 0; i < argc; i++) {
		if (waitingForHandle) {
			parent = (HWND)atoi(args[i]);
			waitingForHandle = false;
		} else if (args[i][0] == '/' || args[i][0] == '-') {
			switch (args[i][1]) {
				case 'c': // Show the Settings dialog box, modal to the foreground window
					mode = SettingsDialog;
					parent = getForegroundWin();
					break;
				case 'p': // Preview Screen Saver as child of window <HWND>
					mode = ScreenSaver;
					waitingForHandle = true;
					break;
				case 'a': // Change password, modal to window <HWND>
					mode = PasswordDialog;
					waitingForHandle = true;
					break;
				case 's': // Run the Screen Saver
					mode = ScreenSaver;
					break;
			}
		}
	}

	if (waitingForHandle) {
		return -1;
	}
	return mode == ScreenSaver ?
		initScreenSaver(&parent) :
		initSettings(&mode, &parent);
}
