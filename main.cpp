#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "model.h"
#include "viewer.h"
#include "menu.h"

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[] = _T("CodeBlocksWindowsApp");


int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */
    HMENU hmenu = LoadMenu(hThisInstance, MAKEINTRESOURCE(IDC_APP1));

    if (!hmenu) {
        printf("Troubles with menu creation\n");
    }

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           400,                 /* The programs width */
           600,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           hmenu,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           lpszArgument         /* No Window Creation data */
           );


    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

void GetNextShift(const char* text, int* shift) {
    int i;
    for (i = 0; text[*shift + i]; i++) {
        if (text[*shift + i] == '\n') {
            *shift += i + 1;
            return;
        }
    }
    *shift += i;
}

void GetPrevShift(const char* text, int* shift) {
    int i;
    for (i = *shift - 2; i > 0; i--) {
        if (text[i] == '\n') {
            *shift = i + 1;
            return;
        }
    }
    *shift = 0;
}

void InitHorScroll(model_t* model, viewer_t* viewer) {
    SCROLLINFO si = {0};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nPage = viewer->lineLength;
    si.nMin = 1;
    si.nMax = GetMaxStrLen(model);
    if (si.nMax <= si.nPage) {
        si.fMask |= SIF_DISABLENOSCROLL;
    }
    SetScrollInfo(viewer->hwnd, SB_HORZ, &si, TRUE);
}

void InitVertScroll(model_t* model, viewer_t* viewer) {
    SCROLLINFO si = {0};
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_PAGE | SIF_RANGE;
    si.nPage = viewer->pageSize;
    si.nMin = 1;
    si.nMax = model->strNum;
    if (si.nMax <= si.nPage) {
        si.fMask |= SIF_DISABLENOSCROLL;
    }
    SetScrollInfo(viewer->hwnd, SB_VERT, &si, TRUE);
}

void UpdateVertScroll(model_t* mode, viewer_t* viewer) {
    SCROLLINFO si;
    int delta, i;
    si.cbSize = sizeof(SCROLLBARINFO);
    si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
    GetScrollInfo(viewer->hwnd, SB_VERT, &si);
    delta = viewer->pageSize - si.nPage;
    si.nPage = viewer->pageSize;
    if (si.nMax <= si.nPage) {
        si.fMask |= SIF_DISABLENOSCROLL;
    }
    else {
        si.fMask ^= SIF_DISABLENOSCROLL;
    }
    if (si.nPos + si.nPage > si.nMax + si.nMin) {
        si.nPos = si.nMax - (si.nPage - si.nMin);
        if (delta > 0) {
            for (i = 0; i < delta; i++) {
                ShiftUp(viewer);
            }
        }
    }
    SetScrollInfo(viewer->hwnd, SB_VERT, &si, TRUE);
}

void UpdateHorScroll(model_t* mode, viewer_t* viewer) {
    SCROLLINFO si;
    int delta, i;
    si.cbSize = sizeof(SCROLLBARINFO);
    si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
    GetScrollInfo(viewer->hwnd, SB_HORZ, &si);
    delta = viewer->lineLength - si.nPage;
    si.nPage = viewer->lineLength;
    if (si.nMax <= si.nPage) {
        si.fMask |= SIF_DISABLENOSCROLL;
    }
    else {
        si.fMask ^= SIF_DISABLENOSCROLL;
    }
    if (si.nPos + si.nPage > si.nMax + si.nMin) {
        si.nPos = si.nMax - (si.nPage - si.nMin);
        if (delta > 0) {
            for (i = 0; i < delta; i++) {
                ShiftLeft(viewer);
            }
        }
    }
    SetScrollInfo(viewer->hwnd, SB_HORZ, &si, TRUE);
}

/*  This function is called by the Windows function DispatchMessage()  */

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CREATESTRUCT* cs = NULL;
    static model_t* model = NULL;
    static viewer_t* viewer = NULL;
    int i = 0, delta = 0;
    SCROLLINFO si = {0};
    HDC hdc = NULL;
    PAINTSTRUCT ps = {0};
    switch (message)                  /* handle the messages */
    {
        case WM_CREATE:
            cs = (CREATESTRUCT*) lParam;
            model = InitModel((LPSTR)cs->lpCreateParams);
            if (!model) {
                printf("Troubles with model Initialisation!\n");
                break;
            }
            viewer = InitViewer(hwnd);
            if (!viewer) {
                printf("Troubles with viewer Initialisation!\n");
                DeleteModel(model);
                break;
            }
            InitHorScroll(model, viewer);
            InitVertScroll(model, viewer);
            break;
        case WM_PAINT:
            hdc = BeginPaint(hwnd, &ps);
            Draw(hdc, model, viewer);
            EndPaint(hwnd, &ps);
            break;
        case WM_HSCROLL:
            si.cbSize = sizeof(SCROLLBARINFO);
            si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
            GetScrollInfo(hwnd, SB_HORZ, &si);
            if (LOWORD(wParam) == SB_LINERIGHT) {
                if (si.nPos == si.nMax - (si.nPage - si.nMin)) {
                    break;
                }
                ShiftRight(viewer);
                SetScrollPos(hwnd, SB_HORZ, si.nPos + 1, TRUE);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            }
            else if (LOWORD(wParam) == SB_LINELEFT) {
                if (si.nPos == si.nMin) {
                    break;
                }
                ShiftLeft(viewer);
                SetScrollPos(hwnd, SB_HORZ, si.nPos - 1, TRUE);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            }
            else if (LOWORD(wParam) == SB_THUMBTRACK) {
                delta = (HIWORD(wParam) - si.nPos);
                if (delta > 0) {
                    for (i = 0; i < delta; i++) {
                        ShiftRight(viewer);
                    }
                }
                else {
                    for (i = 0; i < -delta; i++) {
                        ShiftLeft(viewer);
                    }
                }
                SetScrollPos(hwnd, SB_HORZ, si.nPos + delta, TRUE);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            }
            break;
        case WM_VSCROLL:
            si.cbSize = sizeof(SCROLLBARINFO);
            si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
            GetScrollInfo(hwnd, SB_VERT, &si);
            if (LOWORD(wParam) == SB_LINEDOWN) {
                if (si.nPos == si.nMax - (si.nPage - si.nMin)) {
                    break;
                }
                ShiftDown(viewer);
                SetScrollPos(hwnd, SB_VERT, si.nPos + 1, TRUE);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            }
            else if (LOWORD(wParam) == SB_LINEUP) {
                if (si.nPos == si.nMin) {
                    break;
                }
                ShiftUp(viewer);
                SetScrollPos(hwnd, SB_VERT, si.nPos - 1, TRUE);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            }
            else if (LOWORD(wParam) == SB_THUMBTRACK) {
                delta = (HIWORD(wParam) - GetScrollPos(hwnd, SB_VERT));
                if (delta > 0) {
                    for (i = 0; i < delta; i++) {
                        ShiftDown(viewer);
                    }
                }
                else {
                    for (i = 0; i < -delta; i++) {
                        ShiftUp(viewer);
                    }
                }
                SetScrollPos(hwnd, SB_VERT, si.nPos + delta, TRUE);
                RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            }
            break;
        case WM_MOUSEWHEEL:
            delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) {
                SendMessage(hwnd, WM_VSCROLL, SB_LINEUP, NULL);
            }
            else {
                SendMessage(hwnd, WM_VSCROLL, SB_LINEDOWN, NULL);
            }
            break;
        case WM_SIZE:
            UpdateLineLength(viewer);
            UpdatePageSize(viewer);
            UpdateHorScroll(model, viewer);
            UpdateVertScroll(model, viewer);
            RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE);
            break;
        case WM_DESTROY:
            DeleteModel(model);
            DeleteViewer(viewer);
            PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
