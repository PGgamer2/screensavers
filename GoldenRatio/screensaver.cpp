#include "init.h"

bool running = true;
bool hasParent = false;
SDL_Window* window;
SDL_Renderer* renderer;

const float GOLDEN_RATIO_CONJUGATE = 0.618034F;

void drawCircleSide(int xc, int yc, int x, int y, int side) {
	switch (side % 4) {
		case 3:
			SDL_RenderDrawPoint(renderer, xc - x, yc + y);
			SDL_RenderDrawPoint(renderer, xc - y, yc + x);
			break;
		case 2:
			SDL_RenderDrawPoint(renderer, xc + y, yc + x);
			SDL_RenderDrawPoint(renderer, xc + x, yc + y);
			break;
		case 1:
			SDL_RenderDrawPoint(renderer, xc + y, yc - x);
			SDL_RenderDrawPoint(renderer, xc + x, yc - y);
			break;
		case 0:
			SDL_RenderDrawPoint(renderer, xc - x, yc - y);
			SDL_RenderDrawPoint(renderer, xc - y, yc - x);
	}
}

void onloop() {
	SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);

	int screenW, screenH;
	SDL_GL_GetDrawableSize(window, &screenW, &screenH);
	int iterations = (int)(16.F * (float)max(screenW, screenH) / 1280.F);

	float r = (float)screenH;
	int xc = (screenW - (int)((float)screenH * (1.F + GOLDEN_RATIO_CONJUGATE))) / 2 + screenH;
	int yc = screenH;
	int x, y, d;
	SDL_Rect square;
	for (int i = 0; i < iterations; i++) {
		x = 0, y = (int)r, d = 3 - (int)(2.F * r);
		drawCircleSide(xc, yc, x, y, i);
		while (y >= x) {
			x++;
			if (d > 0) {
				y--;
				d = d + 4 * (x - y) + 10;
			} else d = d + 4 * x + 6;
			drawCircleSide(xc, yc, x, y, i);
		}

		square.w = (int)ceil(r); square.h = (int)ceil(r);
		switch (i % 4) {
			case 0:
				square.x = xc - (int)r;
				square.y = yc - (int)r;
				yc -= (int)(r - r * GOLDEN_RATIO_CONJUGATE);
				break;
			case 1:
				square.x = xc;
				square.y = yc - (int)r;
				xc += (int)(r - r * GOLDEN_RATIO_CONJUGATE);
				break;
			case 2:
				square.x = xc;
				square.y = yc;
				yc += (int)(r - r * GOLDEN_RATIO_CONJUGATE);
				break;
			case 3:
				square.x = xc - (int)r;
				square.y = yc;
				xc -= (int)(r - r * GOLDEN_RATIO_CONJUGATE);
		}
		SDL_RenderDrawRect(renderer, &square);
		r *= GOLDEN_RATIO_CONJUGATE;
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
	// Get parent window rect
	hasParent = *parent != NULL;
	RECT parentRect;
	if (hasParent) {
		GetWindowRect(*parent, &parentRect);
		int x = parentRect.left;
		int y = parentRect.top;
		GetClientRect(*parent, &parentRect);
		parentRect.left = x;
		parentRect.top = y;
	} else {
		GetWindowRect(GetDesktopWindow(), &parentRect);
	}

	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cout << SDL_GetError();
		return -1;
	}
	if (hasParent) {
		window = SDL_CreateWindowFrom(*parent);
		SDL_SetWindowBordered(window, SDL_FALSE);
		SDL_ShowWindow(window);
	} else {
		window = SDL_CreateWindow("ScreenSaver", parentRect.left, parentRect.top, parentRect.right, parentRect.bottom,
			SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_SKIP_TASKBAR | SDL_WINDOW_SHOWN);
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (window == NULL || renderer == NULL) {
		cout << SDL_GetError();
		return -1;
	}
	if (!hasParent) SDL_ShowCursor(SDL_DISABLE);

	// Get monitor refresh rate
	SDL_DisplayMode mode;
	double monitorMilliCap = 16.6667;
	if (SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(window), &mode) == 0) {
		monitorMilliCap = 1000.0 / (double)mode.refresh_rate;
	}

	/* MAIN LOOP */
	system_clock::time_point timeA = system_clock::now();
	system_clock::time_point timeB = system_clock::now();
	SDL_Event Event;
	while (running) {
		// Fix FPS at monitor's refresh rate
		timeA = system_clock::now();
		duration<double, milli> work_time = timeA - timeB;
		if (work_time.count() < monitorMilliCap) {
			duration<double, milli> delta_ms(monitorMilliCap - work_time.count());
			milliseconds delta_ms_duration = duration_cast<milliseconds>(delta_ms);
			this_thread::sleep_for(milliseconds(delta_ms_duration.count()));
		}
		timeB = system_clock::now();
		duration<double, milli> sleep_time = timeB - timeA;

		if (hasParent) {
			if (GetWindowRect(*parent, &parentRect) == 0) running = false;
			SDL_SetWindowPosition(window, parentRect.left, parentRect.top);
			if (GetClientRect(*parent, &parentRect) == 0) running = false;
			SDL_SetWindowSize(window, parentRect.right, parentRect.bottom);
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

