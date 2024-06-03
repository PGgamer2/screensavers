#include "main.h"

bool running = true;
bool hasParent = false;
SDL_Window* window;
SDL_Renderer* renderer;

typedef long double ld; // extra precision

ld zoomPointR = -0.10109636384562L;
ld zoomPointI = 0.95628651080914L;
ld zoomFactor = 1.0L;
SDL_Texture* texBuf;
uint8_t* pixelBuf = nullptr;

void renderPart(uint8_t* t_buff, int initPos, ld t_zoomPointR, ld t_zoomPointI, ld t_zoomFactor, int t_screenW, int t_screenH) {
	ld zDen = t_zoomFactor * min(t_screenH, t_screenW) / 2.0L;
	int iterations = 50 + pow(log10(4.0L / ((ld)t_screenW / zDen)), 5.0L);
	int r, g, b, i;
	float hue, h;
	ld real, imag, zReal, zImag, r2, i2;
	for (int p = t_screenW * t_screenH * initPos; p < t_screenW * t_screenH * (initPos + 1); p += 4) {
		// MANDELBROT
		real = ((ld)((p / 4) % t_screenW) - ((ld)t_screenW / 2.0L)) / zDen + t_zoomPointR;
		imag = ((ld)((p / 4) / t_screenW) - ((ld)t_screenH / 2.0L)) / zDen + t_zoomPointI;
		zReal = real; zImag = imag;
		if ((pow(real - .25L, 2.0L) + pow(imag, 2.0L)) * (pow(real, 2.0L) + (real / 2.0L) + pow(imag, 2.0L) - .1875L) < pow(imag, 2.0L) / 4.0L ||
			pow(real + 1.0L, 2.0L) + pow(imag, 2.0L) < .0625L) {
			i = iterations;
		} else {
			r2 = 0.0L, i2 = 0.0L;
			for (i = 0; i < iterations; ++i) {
				r2 = zReal * zReal;
				i2 = zImag * zImag;
				if (r2 + i2 > 4.0L) break;
				zImag = 2.0L * zReal * zImag + imag;
				zReal = r2 - i2 + real;
			}
		}

		// Iterations to RGB
		r = 0, g = 0, b = 0;
		hue = (1.0F - (float)i / (float)iterations) * 0.7F;
		h = (hue - (float)((int)hue)) * 6.0f;
		switch ((int)h) {
		case 0:
			r = 255;
			g = (int)((h - (float)((int)h)) * 255.0f + 0.5f);
			break;
		case 1:
			r = (int)((1.0f - (h - (float)((int)h))) * 255.0f + 0.5f);
			g = 255;
			break;
		case 2:
			g = 255;
			b = (int)((h - (float)((int)h)) * 255.0f + 0.5f);
			break;
		case 3:
			g = (int)((1.0f - (h - (float)((int)h))) * 255.0f + 0.5f);
			b = 255;
			break;
		case 4:
			r = (int)((h - (float)((int)h)) * 255.0f + 0.5f);
			b = 255;
			break;
		case 5:
			r = 255;
			b = (int)((1.0f - (h - (float)((int)h))) * 255.0f + 0.5f);
			break;
		}
		if (hue < 0.15F) {
			r = (int)((float)r * (hue / 0.15F));
			g = (int)((float)g * (hue / 0.15F));
			b = (int)((float)b * (hue / 0.15F));
		}

		t_buff[p] = r;
		t_buff[p + 1] = g;
		t_buff[p + 2] = b;
	}
}

void onloop() {
	int screenW, screenH;
	SDL_GetRendererOutputSize(renderer, &screenW, &screenH);

	int pitch = 0;
	if (!SDL_LockTexture(texBuf, NULL, (void**)&pixelBuf, &pitch)) {
		thread t1(renderPart, pixelBuf, 0, zoomPointR, zoomPointI, zoomFactor, screenW, screenH);
		thread t2(renderPart, pixelBuf, 1, zoomPointR, zoomPointI, zoomFactor, screenW, screenH);
		thread t3(renderPart, pixelBuf, 2, zoomPointR, zoomPointI, zoomFactor, screenW, screenH);
		renderPart(pixelBuf, 3, zoomPointR, zoomPointI, zoomFactor, screenW, screenH);
		t1.join();
		t2.join();
		t3.join();

		SDL_UnlockTexture(texBuf);
		SDL_RenderCopy(renderer, texBuf, NULL, NULL);
		SDL_RenderPresent(renderer);
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
	unsigned long ZOOM_END = 2000UL;
	if (getSetting("Software\\MandelbrotScr", "ZoomEnd", &ZOOM_END) != 0UL) {
		ZOOM_END = 2000UL;
	}

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
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		cout << SDL_GetError();
		return -1;
	}

	texBuf = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBX8888, SDL_TEXTUREACCESS_STREAMING, parentRect.w, parentRect.h);

	// Get monitor refresh rate
	SDL_DisplayMode mode;
	double monitorMilliCap = 16.6667;
	if (SDL_GetCurrentDisplayMode(SDL_GetWindowDisplayIndex(window), &mode) == 0) {
		monitorMilliCap = 1000.0 / (double)mode.refresh_rate;
	}

	// Decide mandelbrot zoom point
	srand(time(NULL));
	ld theta = ((ld)rand() / (ld)RAND_MAX) * (ld)M_PI * 2.0L;
	if (rand() % 2) {
		ld r = (1 - cos(theta)) / 2.0L;
		zoomPointR = r * cos(theta) + 0.25L;
		zoomPointI = r * sin(theta);
	} else {
		zoomPointR = 0.25L * cos(theta) - 1;
		zoomPointI = 0.25L * sin(theta);
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
		// Zoom
		zoomFactor += zoomFactor * ((ld)duration_cast<milliseconds>(deltaDuration).count() / 100000.0L);
		if (zoomFactor > ZOOM_END) zoomFactor = ZOOM_END;

		if (hasParent) {
			if (getWindowSize(*parent, &parentRect) == 0) running = false;
			SDL_SetWindowPosition(window, parentRect.x, parentRect.y);
			SDL_SetWindowSize(window, parentRect.w, parentRect.h);
		}

		// Rendering and other code
		while (SDL_PollEvent(&Event)) {
			onevent(&Event);
		}
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		//SDL_RenderClear(renderer);
		onloop();
		SDL_RenderPresent(renderer);
	}

	// Free resources
	SDL_DestroyTexture(texBuf);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
