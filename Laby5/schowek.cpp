// komunikacja przez schowek
// program z polem edycyjnym
// i przyciski CUT COPY, PASTE, CLEAR
// wysylanie i odbior tekstu przez schowek

// dwie instancje programu, skomunikowac je ze soba przez schowek
// wlasny zdefiniowany przez siebie format danych

#include <windows.h>
#include <tchar.h> 
#include <stdio.h>

#define EDIT_SPACE_ID 1
#define CUT_BUTTON_ID 2
#define COPY_BUTTON_ID 3
#define PASTE_BUTTON_ID 4
#define CLEAR_BUTTON_ID 5

#define MAX_CHARACTERS 100

static TCHAR szWindowClass[] = _T("AplikacjaMiedzyprocesowa");
static TCHAR szTitle[] = _T("Aplikacja - komunikacja przez schowek");

HWND editBar;

UINT SOME_TEXT = RegisterClipboardFormat(TEXT("worldwide"));

void CreateWindowNow(HWND hwnd)
{
    // pole edycyjne
    editBar = CreateWindowEx(
        0, 
        TEXT("EDIT"), 
        TEXT(""), 
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE,
        20, 
        100, 
        300, 
        50, 
        hwnd, 
        (HMENU)EDIT_SPACE_ID, 
        NULL, 
        NULL
    );

    // CUT BUTTON
    CreateWindow(
        TEXT("BUTTON"), //typ
        TEXT("CUT"), //tekst
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
        10,         // x position 
        10,         // y position 
        100,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)CUT_BUTTON_ID,
        NULL, 
        NULL // Pointer not needed.
    );

    // COPY BUTTON
    CreateWindow(
        TEXT("BUTTON"), //typ
        TEXT("COPY"), //tekst
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
        110,         // x position 
        10,         // y position 
        100,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)COPY_BUTTON_ID,
        NULL, 
        NULL // Pointer not needed.
    );

    //PASTE BUTTON
    CreateWindow(
        TEXT("BUTTON"), //typ
        TEXT("PASTE"), //tekst
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
        210,         // x position 
        10,         // y position 
        100,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)PASTE_BUTTON_ID,
        NULL, 
        NULL // Pointer not needed.
    );

    //CLEAR BUTTON
    CreateWindow(
        TEXT("BUTTON"), //typ
        TEXT("CLEAR"), //tekst
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
        310,         // x position 
        10,         // y position 
        100,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)CLEAR_BUTTON_ID,
        NULL, 
        NULL // Pointer not needed.
    );
}

void copy(HWND hwnd)
{
    HWND tekst = GetDlgItem(hwnd, EDIT_SPACE_ID);
    if (tekst != NULL)
    {
        int nTextLength = GetWindowTextLength(tekst);
        if (nTextLength > 0)
        {
            char* pszText = (char*)GlobalAlloc(GPTR, nTextLength + 1);
            GetWindowTextA(tekst, pszText, nTextLength + 1);

            if (OpenClipboard(hwnd))
            {
                EmptyClipboard();
                HGLOBAL hClipboardData = GlobalAlloc(GMEM_DDESHARE, nTextLength + 1);
                if (hClipboardData != NULL)
                {
                    char* pClipboardData = (char*)GlobalLock(hClipboardData);
                    if (pClipboardData != NULL)
                    {
                        strcpy_s(pClipboardData, nTextLength + 1, pszText);
                        GlobalUnlock(hClipboardData);
                        SetClipboardData(SOME_TEXT, hClipboardData);
                    }
                }
                CloseClipboard();
            }

            GlobalFree(pszText);
        }
    }
}

void cut(HWND hwnd)
{
    copy(hwnd);
    HWND text = GetDlgItem(hwnd, EDIT_SPACE_ID);
    if (text != NULL)
    {
        SendMessage(text, WM_CUT, 0, 0);
        TCHAR buffer[MAX_CHARACTERS];
        GetWindowText(hwnd, buffer, MAX_CHARACTERS);
        SetWindowText(text, TEXT(""));
    }
}

void paste(HWND hwnd)
{
    HWND text = GetDlgItem(hwnd, EDIT_SPACE_ID);
    if (text != NULL)
    {
        if(OpenClipboard(hwnd)){
            HANDLE hClipboardData = GetClipboardData(SOME_TEXT);
            if(hClipboardData != NULL){
                char* pClipboardData = (char*)GlobalLock(hClipboardData);
                if(pClipboardData != NULL){
                    SetWindowTextA(text, pClipboardData);
                    GlobalUnlock(hClipboardData);
                }
                
            }
            CloseClipboard();
        }
    }
}

void clear(HWND hwnd)
{
    HWND text = GetDlgItem(hwnd, EDIT_SPACE_ID);
    if (text != NULL)
    {
        SetWindowText(text, TEXT(""));
    }
}

// handling messages (when events occur)
// LRESULT - int zwracany do Windowsa (reakcja okna na wiadomosc)
LRESULT CALLBACK WndProc( 
   _In_ HWND   hWnd,
   _In_ UINT   message,
   _In_ WPARAM wParam, 
   _In_ LPARAM lParam
)
{
    switch(message){
        case WM_CREATE:
            CreateWindowNow(hWnd);
            break;
        case WM_COMMAND:
            switch(LOWORD(wParam)){
                case CUT_BUTTON_ID:
                    cut(hWnd);
                    break;
                case COPY_BUTTON_ID:
                    copy(hWnd);
                    break;
                case PASTE_BUTTON_ID:
                    paste(hWnd);
                    break;
                case CLEAR_BUTTON_ID:
                    clear(hWnd);
                    break;
                default:
                    break;
            }
            break;
        case WM_CLOSE:
            PostQuitMessage(0);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            //defaultowa akcja dla wiadomosci
            return DefWindowProc(hWnd, message, wParam, lParam);
            break;
    }
    return 0;
}

//main
int WINAPI WinMain( 
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nCmdShow
)
{
   // podstawowe informacje o glownym okienku
   WNDCLASSEX wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);
   wcex.style          = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc    = WndProc;
   wcex.cbClsExtra     = 0;
   wcex.cbWndExtra     = 0;
   wcex.hInstance      = hInstance;
   wcex.hIcon          = LoadIcon(wcex.hInstance, IDI_APPLICATION);
   wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
   wcex.lpszMenuName   = NULL;
   wcex.lpszClassName  = szWindowClass;
   wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    // rejestracja WINDCLASSEX aby wiedzial jak wysylac do okienka wiadomosci
    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL,
            _T("Call to RegisterClassEx failed!"),
            _T(":("),
            NULL);

        return 1;
    }

    // The parameters to CreateWindowEx explained:
    // WS_EX_OVERLAPPEDWINDOW : An optional extended window style.
    // szWindowClass: the name of the application
    // szTitle: the text that appears in the title bar
    // WS_OVERLAPPEDWINDOW: the type of window to create
    // CW_USEDEFAULT, CW_USEDEFAULT: initial position (x, y)
    // 500, 100: initial size (width, length)
    // NULL: the parent of this window
    // NULL: this application does not have a menu bar
    // hInstance: the first parameter from WinMain
    // NULL: not used in this application

    HWND hWnd = CreateWindow(
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        500, 250,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    // w hWnd jest wskaznik do okna (jezeli zostal poprawnie stworzone okno)
    if (!hWnd)
    {
        return 0;
    }

    //zeby mozna bylo widziec okno

    // hWnd: the value returned from CreateWindow
    // nCmdShow: the fourth parameter from WinMain
    ShowWindow(hWnd, nCmdShow); //SW_SHOWDEFAULT
    UpdateWindow(hWnd);


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
