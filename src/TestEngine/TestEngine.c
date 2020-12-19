#include "TestEngine.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "gdi32.lib")

bool Init(int argc, char **argv);
bool Update(float elapsedTime);
void Destroy(void);

static WNDCLASSEX windowClass;
static HWND window;
static uint64_t perfCountFreq;
static BITMAPINFO bmi;

static bool engineIsRunning;
static int frameRate;
static Canvas canvas;
static char *windowTitle;
static char changingTitle[1024];

static inline uint32_t LimitMaxU32(uint32_t a, uint32_t max)
{
    uint32_t result = a - ((a > max) * (a - max));
    return result;
}

static LRESULT WindowCallback(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_CLOSE:
        {
            engineIsRunning = false;
        } break;

        case WM_DESTROY:
        {
            DestroyWindow(hwnd);
        } break;

        default:
        {
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
    }

    return 0;
}

static void InitWindowClass(void)
{
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowCallback;
    windowClass.hInstance = GetModuleHandle(NULL);
    windowClass.lpszClassName = "TestEngine";
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
}

static void InitWindow(void)
{
    RECT dim = {0};
    dim.bottom = canvas.height;
    dim.right = canvas.width;

    uint32_t style = WS_OVERLAPPEDWINDOW;

    AdjustWindowRect(&dim, style, FALSE);

    int width = dim.right - dim.left;
    int height = dim.bottom - dim.top;

    window = CreateWindowEx(0,
                            windowClass.lpszClassName,
                            windowTitle,
                            style,
                            CW_USEDEFAULT, CW_USEDEFAULT,
                            width, height,
                            0, 0,
                            GetModuleHandle(NULL),
                            NULL);
}

static void InitCanvas(void)
{
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = canvas.width;
    bmi.bmiHeader.biHeight = -canvas.height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    canvas.pixels = calloc(1, canvas.width * canvas.height * sizeof(*canvas.pixels));
}

static void DrawFrame(void)
{
    HDC dc = GetDC(window);

    SetDIBitsToDevice(dc, 0, 0, canvas.width, canvas.height, 0, 0, 0, canvas.height, canvas.pixels, &bmi, DIB_RGB_COLORS);

    ReleaseDC(window, dc);
}

static void LimitFrameRate(float msElapsed)
{
    float msPerFrame = 1000.0f / frameRate;

    if(msElapsed < msPerFrame)
    {
        uint32_t sleepMS = (uint32_t)(msPerFrame - msElapsed);
        Sleep(sleepMS);
    }
}

static float GetElapsedTime(void)
{
    LARGE_INTEGER unitInSecond;
    QueryPerformanceCounter(&unitInSecond);

    return (1000.0f * (float)((double)unitInSecond.QuadPart / (double)perfCountFreq));
}

int main(int argc, char **argv)
{
    engineIsRunning = true;
    frameRate = 60;
    windowTitle = "TestEngine";

    canvas.height = 500;
    canvas.width = 750;

    if(!Init(argc, argv))
    {
        fprintf(stderr, "Failed user initialization.\n");
        return 1;
    }

    LARGE_INTEGER qpf;
    if(!QueryPerformanceFrequency(&qpf))
    {
        fprintf(stderr, "Failed to get the clock tick rate.\n");
        return 1;
    }

    perfCountFreq = qpf.QuadPart;

    if(timeBeginPeriod(1) != TIMERR_NOERROR)
    {
        fprintf(stderr, "Failed to set the scheduler MS.\n");
        return 1;
    }

    InitWindowClass();
    if(!RegisterClassEx(&windowClass))
    {
        fprintf(stderr, "Failed to register window class.\n");
        return 1;
    }

    InitWindow();
    if(window == NULL)
    {
        fprintf(stderr, "Failed to open the window.\n");
        return 1;
    }

    ShowWindow(window, SW_SHOW);

    InitCanvas();
    if(canvas.pixels == NULL)
    {
        fprintf(stderr, "Failed to allocate canvas.\n");
        return 1;
    }

    int frameCount = 0;
    float totalFrames = 0.0f;
    float elapsedTime = 0.0f;

    float frameStart = GetElapsedTime();
    while(engineIsRunning)
    {
        MSG message;
        while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }

        if(!Update(elapsedTime))
        {
            engineIsRunning = false;
        }

        DrawFrame();

        float frameEnd = GetElapsedTime();
        LimitFrameRate(frameEnd - frameStart);

        float afterFrame = GetElapsedTime();
        float elapsedSeconds = (afterFrame - frameStart) / 1000.0f;
        float elapsedTime = afterFrame - frameStart;
        
        frameStart = afterFrame; //updating frame start for next frame/
        
        ++frameCount;
        totalFrames += 1.0f / elapsedSeconds;

        if(frameCount == (frameRate / 2))
        {
            sprintf_s(changingTitle, sizeof(changingTitle), "%s   %df/s", windowTitle, (int)(totalFrames / frameCount));
            SetWindowText(window, changingTitle);

            frameCount = 0;
            totalFrames = 0.0f;
        }
    }

    Destroy();

    return 0;
}

void SetWindowSize(int width, int height)
{
    if(window == 0)
    {
        canvas.height = height;
        canvas.width = width;
        return;
    }

    fprintf(stderr, "SetWindowSize can only be called during the Init() function.\n");
}

void SetWindowTitle(char *title)
{
    windowTitle = title;
}

void SetFrameRate(int desiredFPS)
{
    frameRate = desiredFPS;
}

void ExitEngine(void)
{
    engineIsRunning = false;
}

void DrawPixel(uint32_t x, uint32_t y, uint32_t color)
{
    x = LimitMaxU32(x, canvas.width - 1);
    y = LimitMaxU32(y, canvas.height - 1);

    canvas.pixels[y * canvas.width + x] = color;
}

bool IsKeyDown(uint32_t keyCode)
{
    return (GetAsyncKeyState(keyCode) & (1 << 15)) != 0;
}

void GetMouseState(MouseState *mouseState)
{
    POINT mouseP;
    GetCursorPos(&mouseP);
    ScreenToClient(window, &mouseP);

    mouseState->x = mouseP.x;
    mouseState->y = mouseP.y;
    mouseState->lButton = (GetAsyncKeyState(VK_LBUTTON) & (1 << 15)) != 0;
    mouseState->mButton = (GetAsyncKeyState(VK_MBUTTON) & (1 << 15)) != 0;
    mouseState->rButton = (GetAsyncKeyState(VK_RBUTTON) & (1 << 15)) != 0;
    mouseState->xButton1 = (GetAsyncKeyState(VK_XBUTTON1) & (1 << 15)) != 0;
    mouseState->xButton2 = (GetAsyncKeyState(VK_XBUTTON2) & (1 << 15)) != 0;
}

void SetCursorPosition(int x, int y)
{
    RECT coords;

    if(!GetWindowRect(window, &coords))
    {
        fprintf(stderr, "Failed to set cursor position.");
        return;
    }
    
    if(!SetCursorPos(coords.left + x, coords.top + y))
    {
        fprintf(stderr, "Failed to set cursor position.");
        return;
    }
}

void LockCursor(void)
{
    RECT coords;

    if(!GetWindowRect(window, &coords))
    {
        fprintf(stderr, "Failed to lock cursor.\n");
        return;
    }

    if(!ClipCursor(&coords))
    {
        fprintf(stderr, "Failed to lock cursor.\n");
        return;
    }
}

void UnlockCursor(void)
{
    if(!ClipCursor(0))
    {
        fprintf(stderr, "Failed to unclip cursor.\n");
        return;
    }
}

Canvas *GetCanvas(void)
{
    return &canvas;
}