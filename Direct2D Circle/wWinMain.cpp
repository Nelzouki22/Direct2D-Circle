#include <windows.h>
#include <d2d1.h>

#pragma comment(lib, "d2d1")

// Global variables
HINSTANCE hInst;
ID2D1Factory* pFactory = nullptr;
ID2D1HwndRenderTarget* pRenderTarget = nullptr;
ID2D1SolidColorBrush* pBrush = nullptr;

// Function prototypes
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void CreateGraphicsResources(HWND hWnd);
void DiscardGraphicsResources();
void OnRender(HWND hWnd);
void OnResize(HWND hWnd, UINT width, UINT height);

// Entry point
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    hInst = hInstance;

    // Register the window class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"CircleApp";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    RegisterClassEx(&wcex);

    // Create the window
    HWND hWnd = CreateWindow(L"CircleApp", L"Draw Circle with Direct2D", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, NULL, hInstance, NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Main message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Clean up
    DiscardGraphicsResources();
    if (pFactory) pFactory->Release();

    return (int)msg.wParam;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        // Create Direct2D factory
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
        break;

    case WM_SIZE:
        OnResize(hWnd, LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_PAINT:
        OnRender(hWnd);
        ValidateRect(hWnd, NULL);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Create graphics resources
void CreateGraphicsResources(HWND hWnd)
{
    if (!pRenderTarget)
    {
        RECT rc;
        GetClientRect(hWnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

        pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hWnd, size),
            &pRenderTarget);

        pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::SkyBlue), &pBrush);
    }
}

// Release graphics resources
void DiscardGraphicsResources()
{
    if (pBrush) pBrush->Release();
    if (pRenderTarget) pRenderTarget->Release();
}

// Render the circle
void OnRender(HWND hWnd)
{
    CreateGraphicsResources(hWnd);

    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

    // Draw a circle
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(200.0f, 200.0f), 100.0f, 100.0f);
    pRenderTarget->FillEllipse(&ellipse, pBrush);

    pRenderTarget->EndDraw();
}

// Handle window resize
void OnResize(HWND hWnd, UINT width, UINT height)
{
    if (pRenderTarget) {
        RECT rc;
        GetClientRect(hWnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
        pRenderTarget->Resize(size);
    }
}
