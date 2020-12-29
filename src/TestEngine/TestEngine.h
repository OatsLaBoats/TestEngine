#ifndef DEFINED_TEST_ENGINE_H
#define DEFINED_TEST_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

/*
In order to compile you need to create these functions:
    bool Init(int argc, char **argv) : This function is called before the window initialization.
    bool Update(void) : This function is called before the draw operation.
    void Destory(void) : This function is called before program exit.

For all functions returning a bool returning true means that the engine can continue. While false means the engine should stop.
*/

#define COLOR_WHITE ((Color){.rgb = 0xFFFFFFFF})
#define COLOR_BLACK ((Color){.rgb = 0xFF000000})
#define COLOR_RED ((Color){.rgb = 0xFFFF0000})
#define COLOR_GREEN ((Color){.rgb = 0xFF00FF00})
#define COLOR_BLUE ((Color){.rgb = 0xFF0000FF})
#define COLOR_YELLOW ((Color){.rgb = 0xFFFFFF00})
#define COLOR_VIOLET ((Color){.rgb = 0xFFFF00FF})

typedef enum KeyCode
{
	KEY_BACKSPACE = 0x08,
	KEY_TAB = 0x09,
	KEY_ENTER = 0x0D,
	KEY_SHIFT = 0x10,
	KEY_CTRL = 0x11,
	KEY_ALT = 0x12,
	KEY_CAPSLOCK = 0x14,
	KEY_ESCAPE = 0x1B,
	KEY_SPACE = 0x20,
	KEY_PAGEUP = 0x21,
	KEY_PAGEDOWN = 0x22,
	KEY_END = 0x23,
	KEY_HOME = 0x24,
	KEY_LEFT = 0x25,
	KEY_UP = 0x26,
	KEY_RIGHT = 0x27,
	KEY_DOWN = 0x28,
	KEY_DELETE = 0x2E,
	KEY_NUMPAD0 = 0x60,
	KEY_NUMPAD1 = 0x61,
	KEY_NUMPAD2 = 0x62,
	KEY_NUMPAD3 = 0x63,
	KEY_NUMPAD4 = 0x64,
	KEY_NUMPAD5 = 0x65,
	KEY_NUMPAD6 = 0x66,
	KEY_NUMPAD7 = 0x67,
	KEY_NUMPAD8 = 0x68,
	KEY_NUMPAD9 = 0x69,
	KEY_MULTIPLY = 0x6A,
	KEY_ADD = 0x6B,
	KEY_SUBTRACT = 0x6D,
	KEY_DIVIDE = 0x6F,
	KEY_F1 = 0x70,
	KEY_F2 = 0x71,
	KEY_F3 = 0x72,
	KEY_F4 = 0x73,
	KEY_F5 = 0x74,
	KEY_F6 = 0x75,
	KEY_F7 = 0x76,
	KEY_F8 = 0x77,
	KEY_F9 = 0x78,
	KEY_F10 = 0x79,
	KEY_F11 = 0x7A,
	KEY_F12 = 0x7B,
	KEY_NUMLOCK = 0x90,
	KEY_LSHIFT = 0xA0,
	KEY_RSHIFT = 0xA1,
	KEY_LCTRL = 0xA2,
	KEY_RCTRL = 0xA3,
	KEY_LALT = 0xA4,
	KEY_RALT = 0xA5
} KeyCode;

typedef struct MouseState
{
	int x;
    int y;
	bool lButton;
	bool mButton;
	bool rButton;
	bool xButton1;
	bool xButton2;
} MouseState;

typedef struct Canvas
{
    uint32_t *pixels;
    int width;
    int height;
} Canvas;

typedef union Color
{
	uint32_t rgb;

	struct
	{
		uint8_t a;
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};
} Color;

void SetWindowSize(int width, int height);
void SetWindowTitle(char *title);
void SetFrameRate(int desiredFPS);

void StopEngine(void);
bool IsKeyDown(uint32_t keyCode);
void GetMouseState(MouseState *mouseState);
void SetCursorPosition(int x, int y);
void LockCursor(void);
void UnlockCursor(void);
Canvas *GetCanvas(void);

void DrawPixel(uint32_t x, uint32_t y, Color color);
void Clear(Color color);

static inline Color FColor(float a, float r, float g, float b)
{
	Color color;
	color.a = (uint8_t)(a * 255.0f);
	color.r = (uint8_t)(r * 255.0f);
	color.g = (uint8_t)(g * 255.0f);
	color.b = (uint8_t)(b * 255.0f);
	
	return color;
}

static inline Color IColor(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
	Color color;
	color.a = a;
	color.r = r;
	color.g = g;
	color.b = b;

	return color;
}

#endif