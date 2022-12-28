#include "main.h"

bool running = true;
bool hasParent = false;
SDL_Window* window;
SDL_Renderer* renderer;

void onloop() {
	int w, h, grayscale;
	SDL_Rect rect;
	rect.w = 5;
	rect.h = 5;
	SDL_GetWindowSizeInPixels(window, &w, &h);
	for (int x = 0; x <= w; x += 5) {
		for (int y = 0; y <= h; y += 5) {
			grayscale = (int)((float)rand() / (float)RAND_MAX * 255.F);
			SDL_SetRenderDrawColor(renderer, grayscale, grayscale, grayscale, 255);
			rect.x = x;
			rect.y = y;
			SDL_RenderFillRect(renderer, &rect);
		}
	}
}

void onevent(SDL_Event* Event) {
	if (Event->type == SDL_QUIT) {
		running = false;
		return;
	}

	if (hasParent) return;
	if (Event->type == SDL_KEYDOWN) {
		running = false;
		return;
	}
	if (Event->type == SDL_MOUSEMOTION) {
		SDL_MouseMotionEvent* MouseEvent = (SDL_MouseMotionEvent*)Event;
		if (MouseEvent->xrel > 2 || MouseEvent->yrel > 2) {
			running = false;
		}
	}
}

int initScreenSaver(HWND* parent) {
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cout << SDL_GetError();
		return -1;
	}

	// Get parent window rect
	hasParent = *parent != NULL;
	SDL_Rect parentRect;
	if (hasParent) {
		getWindowSize(*parent, &parentRect);
	} else {
		SDL_DisplayMode mode;
		SDL_GetCurrentDisplayMode(0, &mode);
		parentRect.x = 0;
		parentRect.y = 0;
		parentRect.w = mode.w;
		parentRect.h = mode.h;
	}

	// Create window
	if (hasParent) {
		window = SDL_CreateWindowFrom(*parent);
		SDL_SetWindowBordered(window, SDL_FALSE);
		SDL_ShowWindow(window);
	} else {
		window = SDL_CreateWindow("ScreenSaver", parentRect.x, parentRect.y, parentRect.w, parentRect.h,
			SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_SHOWN);
		SDL_ShowCursor(SDL_DISABLE);
	}
	if (window == NULL) {
		cout << SDL_GetError();
		return -1;
	}

	// Create renderer
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		cout << SDL_GetError();
		return -1;
	}

	// Get monitor refresh rate
	SDL_DisplayMode mode;
	double monitorMilliCap = 16.6667;
	if (SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(window), &mode) == 0) {
		monitorMilliCap = 1000.0 / (double)mode.refresh_rate;
	}

	/* MAIN LOOP */
	system_clock::time_point currentFrame = system_clock::now();
	system_clock::time_point lastFrame = system_clock::now();
	SDL_Event Event;
	while (running) {
		// Fix FPS at monitor's refresh rate
		currentFrame = system_clock::now();
		system_clock::duration deltaDuration = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaDuration.count() < monitorMilliCap) {
			duration<double, milli> delta_ms(monitorMilliCap - deltaDuration.count());
			milliseconds delta_ms_duration = duration_cast<milliseconds>(delta_ms);
			this_thread::sleep_for(milliseconds(delta_ms_duration.count()));
		}

		if (hasParent) {
			if (getWindowSize(*parent, &parentRect) == 0) running = false;
			SDL_SetWindowPosition(window, parentRect.x, parentRect.y);
			SDL_SetWindowSize(window, parentRect.w, parentRect.h);
		}

		// Rendering and other code
		while (SDL_PollEvent(&Event)) {
			onevent(&Event);
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		onloop();
		SDL_RenderPresent(renderer);
	}

	// Free resources
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

