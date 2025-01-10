#include "framework.h"
#include "lab2round.h"
#include <vector>
#include <string>
#include <tuple>

using namespace std;

#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_LAB2ROUND, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2ROUND));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}

vector<tuple<float, float, wstring, int>> p;
bool needResize{ false };
const int step = 1;
const int maxSize = 360 / step;
wstring text;

const float pi = 3.14159265359f;

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2ROUND));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

HWND hWnd;

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

void onWM_CHAR(const wchar_t c) {
    if (c == '\b' && !text.empty()) {
        text.pop_back();
        needResize = true;
    }
    else if (c != '\b' && static_cast<int>(text.size()) < maxSize) {
        text.push_back(c);
        needResize = true;
    }
}

void Resize(HWND hwnd) {
    int size = text.size();
    p.resize(size);
    RECT rect;
    GetClientRect(hWnd, &rect);

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    float centerX = static_cast<float>(width) / 2;
    float centerY = static_cast<float>(height) / 2 - 10;
    float radius = static_cast<float>(min(width, height)) / 2 - 30;

    for (int i = 0; i < size; ++i) {
        float angle = (i * step - 90) * pi / 180.f;
        p[i] = { centerX + radius * cos(angle), centerY + radius * sin(angle), text.substr(i, 1),i };
    }
}

void draw(HDC hdc);
HFONT hFont;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static RECT client = {};
    hFont = CreateFontW(50, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

    switch (message)
    {
    case WM_CREATE:
    {
        GetClientRect(hWnd, &client);
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_CHAR:
    {
        onWM_CHAR(static_cast<const wchar_t>(wParam));
        needResize = true;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_SIZE:
    {
        needResize = true;
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
        POINT Min = { 300, 300 };
        POINT Max = { 1440, 900 };
        pInfo->ptMinTrackSize = Min;
        pInfo->ptMaxTrackSize = Max;
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        if (needResize) {
            Resize(hWnd);
            needResize = false;
        }
        draw(hdc);

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void draw(HDC hdc) {
    SetBkMode(hdc, TRANSPARENT);
    SelectObject(hdc, hFont);

    RECT client;
    GetClientRect(hWnd, &client); 

    XFORM xform;
    SetGraphicsMode(hdc, GM_ADVANCED); 

    for (const auto& [x, y, s, i] : p) {

        SaveDC(hdc);

        float angle = (i * step ) * pi / 180.f;
        xform.eM11 = cos(angle); 
        xform.eM12 = sin(angle); 
        xform.eM21 = -sin(angle); 
        xform.eM22 = cos(angle);  
        xform.eDx = x;            
        xform.eDy = y;            

        SetWorldTransform(hdc, &xform);

     
        RECT rect{ -30, -30, 30, 30 }; 
        DrawTextW(hdc, s.c_str(), -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

 
        RestoreDC(hdc, -1);
    }

    DeleteObject(hFont); 
}

