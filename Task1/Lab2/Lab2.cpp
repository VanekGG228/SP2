#include "framework.h"
#include "Lab2.h"
#include "cell.h"
#include <vector>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <fstream>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#ifndef UNICODE
#define UNICODE
#endif

#define ID_TRACKBAR_ROWS 1001
#define ID_TRACKBAR_COLS 1002

using namespace std;

#define MAX_LOADSTRING 100


const int maxRowsCols = 10;

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
    LoadStringW(hInstance, IDC_LAB2, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB2));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return static_cast<int>(msg.wParam);
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB2));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = nullptr;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HWND hTrRows, hTrCols;
Matrix matrix;




std::vector<std::wstring> loadSentencesFromFile(const std::wstring& filename) {
    std::wifstream file(filename); 
    std::vector<std::wstring> sentences;
    std::wstring line;

    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (!line.empty()) {
                sentences.push_back(line); 
            }
        }
        file.close();
    }
    else {
        MessageBox(NULL, L"Не удалось открыть файл.", L"Ошибка", MB_OK | MB_ICONERROR);
    }

    return sentences; 
}


void redrawMatrix(HWND hWnd) {
    HDC hdc = GetDC(hWnd);
    RECT rect;
    GetClientRect(hWnd, &rect);

    matrix.setSize({ rect.right - rect.left + 10, rect.bottom - rect.top - 95 });
    matrix.draw(hdc);
    ReleaseDC(hWnd, hdc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {


    case WM_CREATE:
    {
        HWND hButtonFill = CreateWindow(
            L"BUTTON",  
            L"Заполнить ячейки", 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  
            700,         
            50,         
            150,        
            30,         
            hWnd,       
            (HMENU)ID_BUTTON_FILL,  
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), 
            NULL);     

        CreateWindowEx(0, L"EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            10, 50, 100, 30, hWnd, (HMENU)ID_EDIT_ROWS, NULL, NULL);

        CreateWindowEx(0, L"EDIT", NULL,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_NUMBER,
            400, 50, 100, 30, hWnd, (HMENU)ID_EDIT_COLS, NULL, NULL);

        SetWindowText(GetDlgItem(hWnd, ID_EDIT_COLS), L"5");
        SetWindowText(GetDlgItem(hWnd, ID_EDIT_ROWS), L"5");
        RECT rect;
        GetClientRect(hWnd, &rect);
        matrix.setSize({ rect.right - rect.left, rect.bottom - rect.top - 100 });
        matrix.setRowsCols(5, 5);
        matrix.setPos({ 0, 100 });
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam); 

        if (HIWORD(wParam) == EN_CHANGE)
        {
            char buffer[10] = { 0 };
            int value1 = 0;

            GetWindowTextA(GetDlgItem(hWnd, ID_EDIT_ROWS), buffer, sizeof(buffer));
            value1 = atoi(buffer);

            if (value1 > maxRowsCols){
                    value1 = maxRowsCols;
                    SetWindowText(GetDlgItem(hWnd, ID_EDIT_ROWS), to_wstring(maxRowsCols).c_str());
            }
            int value2 = 0;

            GetWindowTextA(GetDlgItem(hWnd, ID_EDIT_COLS), buffer, sizeof(buffer));
            value2 = atoi(buffer);

            if (value2 > maxRowsCols)
                {
                    value2 = maxRowsCols;
                    SetWindowText(GetDlgItem(hWnd, ID_EDIT_COLS), to_wstring(maxRowsCols).c_str());
                }

            matrix.setRowsCols(value1, value2);
            redrawMatrix(hWnd);
            SetFocus(hWnd);

        }
        switch (wmId)
        {
        case ID_BUTTON_FILL:  
        {
            std::vector<std::wstring> sentences = loadSentencesFromFile(L"E:\\WORK\\SysP\\SPLab2\\Task1\\x64\\Debug\\sentences.txt"); //here is the path to ur file with sentences

            if (!sentences.empty()) {
                matrix.fillWithSentences(sentences); 
                redrawMatrix(hWnd);
            }
            else {
                MessageBox(NULL, L"Файл пуст или не удалось загрузить предложения.", L"Ошибка", MB_OK | MB_ICONERROR);
            }
            break;
        }
        case IDM_EXIT:  
            DestroyWindow(hWnd);
            break;  

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);  
        }
        break;
    }
 
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        redrawMatrix(hWnd);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}
