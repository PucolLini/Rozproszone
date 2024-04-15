//pętla komunikatów Windows

// Aplikacja korzystuajaca z WinAPI i wyświetlająca:

// 1) okienko, a w nim checkbox'y dla podstawowych komunikatów Windows (np. WM_MOVE, WM_SIZE, WM_PAINT) oraz przycisk (button) CLEAR;

// 2) okno konsoli (podłączone pod ten sam proces!) do wyświetlania komunikatów tekstowych.

// printf za kazdym razem gdy procedura obsługi obłsuugje dany komunikat oraz gdy jest zaznaczony checkbox

// zamknięcie okna: informacja że chcemy zamknąć + opóźnienie, zamknąć konsolę potem zamknąć aplikację

#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define MOVE_CHECKBOX_ID 101
#define SIZE_CHECKBOX_ID 102
#define PAINT_CHECKBOX_ID 103
#define CLEAR_BUTTON_ID 104
#define CLOSE_BUTTON_ID 105

bool areChecked[3] = {false};

static TCHAR szWindowClass[] = _T("AplikacjaMiedzyprocesowa");
static TCHAR szTitle[] = _T("Aplikacja - komunikaty Windows");

void CreateWindowNow(HWND hwnd){
    // MOVE CHECKBOX
    CreateWindow(
        TEXT("BUTTON"), //typ
        TEXT("MOVE"), //tekst
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles 
        10,         // x position 
        10,         // y position 
        100,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)MOVE_CHECKBOX_ID,
        NULL, 
        NULL // Pointer not needed.
    );

    // SIZE CHECKBOX
    CreateWindow(
        TEXT("BUTTON"), //typ
        TEXT("SIZE"), //tekst
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles 
        110,         // x position 
        10,         // y position 
        100,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)SIZE_CHECKBOX_ID,
        NULL, 
        NULL // Pointer not needed.
    );

    //PAINT CHECKBOX
    CreateWindow(
        TEXT("BUTTON"), //typ
        TEXT("PAINT"), //tekst
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles 
        210,         // x position 
        10,         // y position 
        100,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)PAINT_CHECKBOX_ID,
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

    //CLOSE BUTTON
    CreateWindow(
        TEXT("BUTTON"), //typ
        TEXT("CLOSE"), //tekst
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,  // Styles 
        410,         // x position 
        10,         // y position 
        100,        // Button width
        50,        // Button height
        hwnd,     // Parent window
        (HMENU)CLOSE_BUTTON_ID,
        NULL, 
        NULL // Pointer not needed.
    );
}

void CloseWindowNow(HWND hwnd){
    printf("Aplikacja zamyka sie.");
    Sleep(4000);
    FreeConsole(); //zamykanie konsoli
    Sleep(4000);
    PostQuitMessage(0); // zamkniecie okna
}

void ClearWindow(HWND hwnd){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    DWORD size;
    COORD screenPosition = {0, 0};

    GetConsoleScreenBufferInfo(hConsole, &info);
    size = info.dwSize.X * info.dwSize.Y;
    FillConsoleOutputCharacter(hConsole, (TCHAR)' ', size, screenPosition, &size);
    FillConsoleOutputAttribute(hConsole, info.wAttributes, size, screenPosition, &size);
    SetConsoleCursorPosition(hConsole, screenPosition);
}

//handling messages (when events occur)
// LRESULT - int zwracany do Windowsa (reakcja okna na wiadomosc)
LRESULT CALLBACK WndProc( 
   _In_ HWND   hWnd, // wskaznik do okna
   _In_ UINT   message, // wiadomosc (np. WM_SIZE)
   _In_ WPARAM wParam, 
   _In_ LPARAM lParam
)
{
    switch(message){
        case WM_CREATE:
            CreateWindowNow(hWnd);
            AllocConsole();
            AttachConsole(GetCurrentProcessId());
            printf("Konsola zostala zainicjalizowana.\n");
            break;
        case WM_COMMAND:
            // wParam HIGH kto wysyla wiadomosc
            // wParam LOW jaka wiadomosc
            switch(LOWORD(wParam)){
                case MOVE_CHECKBOX_ID:
                    // wParam nieuzywany
                    // lParam - koordynaty x i y lewego gornego naroznika okna
                    
                    areChecked[0] = !areChecked[0];
                    if(areChecked[0]){
                        printf("MOVE_CHECKBOX zostal zaznaczony.\n");
                    }
                    else{
                        printf("MOVE_CHECKBOX zostal odznaczony.\n");
                    }
                    break;
                case SIZE_CHECKBOX_ID:
                    // wParam czy okno zostalo zminimalizowane/maksymalizowane/zmieniona wielkosc
                    // lParam nowa szerokosc i wysokosc okna (macra LOWORD i HIWORD pozwalaja na shiftowanie bitów aby dostac te wartosci)
                    //int width = LOWORD(lParam); 
                    //int height = HIWORD(lParam);
                    areChecked[1] = !areChecked[1];
                    if(areChecked[1]){
                        printf("SIZE_CHECKBOX zostal zaznaczony.\n");
                    }
                    else{
                        printf("SIZE_CHECKBOX zostal odznaczony.\n");
                    }
                    break;

                case PAINT_CHECKBOX_ID:
                    areChecked[2] = !areChecked[2];
                    if(areChecked[2]){
                        printf("PAINT_CHECKBOX zostal zaznaczony.\n");
                    }
                    else{
                        printf("PAINT_CHECKBOX zostal odznaczony.\n");
                    }
                    break;
                case CLEAR_BUTTON_ID:
                    ClearWindow(hWnd);
                    break;
                case CLOSE_BUTTON_ID:
                    printf("Wiadomosc od CLOSE_BUTTON\n");
                    CloseWindowNow(hWnd);
                default:
                    break;
            }
            break;
        case WM_CLOSE:
            printf("Wiadomosc od WM_CLOSE\n");
            CloseWindowNow(hWnd);
            break;
        case WM_DESTROY:
            printf("Wiadomosc od WM_DESTROY\n");
            CloseWindowNow(hWnd);
            break;
        default:
            if(areChecked[0] && message == WM_MOVE){
                printf("WM_MOVE\n");
            }
            if(areChecked[1] && message == WM_SIZE){
                printf("WM_SIZE\n");
            }
            if(areChecked[2] && message == WM_PAINT){
                printf("WM_PAINT\n");
            }
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

    HWND hWnd = CreateWindowEx(
        0, // WS_EX_OVERLAPPEDWINDOW
        szWindowClass,
        szTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
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
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
