#include "init.h"

bool running = true;
bool hasParent = false;
SDL_Window* window;
SDL_Renderer* renderer;

bool shouldRenderSquares;
float zoomFactor = 1.0F;
const float GOLDEN_RATIO_CONJUGATE = 0.618034F;
constexpr float ZOOM_END = 1.618034F * 1.618034F * 1.618034F * 1.618034F;

void drawCircleSide(int xc, int yc, int x, int y, int side) {
	switch (side % 4) {
		case 0:
			SDL_RenderDrawPoint(renderer, xc - x, yc + y);
			SDL_RenderDrawPoint(renderer, xc - y, yc + x);
			break;
		case 1:
			SDL_RenderDrawPoint(renderer, xc + y, yc + x);
			SDL_RenderDrawPoint(renderer, xc + x, yc + y);
			break;
		case 2:
			SDL_RenderDrawPoint(renderer, xc + y, yc - x);
			SDL_RenderDrawPoint(renderer, xc + x, yc - y);
			break;
		case 3:
			SDL_RenderDrawPoint(renderer, xc - x, yc - y);
			SDL_RenderDrawPoint(renderer, xc - y, yc - x);
	}
}

void onloop() {
	SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);

	int screenW, screenH;
	SDL_GL_GetDrawableSize(window, &screenW, &screenH);
	int iterations = (int)(16.F * (float)max(screenW, screenH) / 1280.F);

	float r = zoomFactor;
	int xc = screenW / 2;
	int yc = screenH / 2;
	int x, y, d;
	SDL_Rect square;
	for (int i = 0; i < iterations; i++) {
		square.w = (int)ceil(r); square.h = (int)ceil(r);
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
		switch (i % 4) {
			case 0:
				square.x = xc - (int)r;
				square.y = yc;
				yc -= (int)(r * GOLDEN_RATIO_CONJUGATE);
				break;
			case 1:
				square.x = xc;
				square.y = yc;
				xc -= (int)(r * GOLDEN_RATIO_CONJUGATE);
				break;
			case 2:
				square.x = xc;
				square.y = yc - (int)r;
				yc += (int)(r * GOLDEN_RATIO_CONJUGATE);
				break;
			case 3:
				square.x = xc - (int)r;
				square.y = yc - (int)r;
				xc += (int)(r * GOLDEN_RATIO_CONJUGATE);
		}
		if (shouldRenderSquares) {
			SDL_RenderDrawRect(renderer, &square);
		}
		r *= 1.F + GOLDEN_RATIO_CONJUGATE;
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
	// Check if we should render squares
	DWORD value = 0;
	if (RegGetDword(HKEY_CURRENT_USER, "Software\\GoldenRatio", "Squares", &value) == ERROR_SUCCESS) {
		shouldRenderSquares = value != 0L;
	}

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

		zoomFactor += zoomFactor * ((float)duration_cast<milliseconds>(deltaDuration).count() / 1000.F);
		if (zoomFactor > ZOOM_END) {
			zoomFactor = 1.F + (zoomFactor - ZOOM_END) * ((float)duration_cast<milliseconds>(deltaDuration).count() / 1000.F);
		}

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

