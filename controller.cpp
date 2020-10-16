#include <windows.h>
#include "controller.h"
#include "model.h"
#include "viewer.h"
#include "menu.h"
#include <stdio.h>

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

void UpdateVertScroll(model_t* model, viewer_t* viewer) {
    SCROLLINFO si;
    int delta, i;
    si.cbSize = sizeof(SCROLLBARINFO);
    si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
    GetScrollInfo(viewer->hwnd, SB_VERT, &si);
    delta = viewer->pageSize - si.nPage;
    si.nPage = viewer->pageSize;
    if (viewer->mode == CUT_MODE) {
        si.nMax = model->strNum;
    }
    else {
        si.nMax = model->strings[model->strNum] - model->strings[0];
        if (si.nMax % viewer->lineLength != 0) {
            si.nMax = si.nMax / viewer->lineLength + 1;
        }
        else {
            si.nMax = si.nMax / viewer->lineLength;
        }
    }
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

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CREATESTRUCT* cs = NULL;
    static model_t* model = NULL;
    static viewer_t* viewer = NULL;
    int i = 0, delta = 0;
    SCROLLINFO si = {0};
    HDC hdc = NULL;
    PAINTSTRUCT ps = {0};
    HMENU hmenu = NULL;
    UINT state = 0;
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
            //SelectObject(GetDC(hwnd), viewer->font);
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
            switch (LOWORD(wParam)) {
            case SB_LINERIGHT:
                delta = 1;
                if (si.nPos == si.nMax - (si.nPage - si.nMin)) {
                    break;
                }
                ShiftRight(viewer);
                break;
            case SB_LINELEFT:
                delta = -1;
                if (si.nPos == si.nMin) {
                    break;
                }
                ShiftLeft(viewer);
                break;
            case SB_THUMBTRACK:
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
                break;
            }
            SetScrollPos(hwnd, SB_HORZ, si.nPos + delta, TRUE);
            RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
            break;
        case WM_VSCROLL:
            si.cbSize = sizeof(SCROLLBARINFO);
            si.fMask = SIF_POS | SIF_RANGE | SIF_PAGE;
            GetScrollInfo(hwnd, SB_VERT, &si);
            switch (LOWORD(wParam)) {
                case SB_LINEDOWN:
                    delta = 1;
                    if (si.nPos == si.nMax - (si.nPage - si.nMin)) {
                        break;
                    }
                    ShiftDown(viewer);
                break;
                case SB_LINEUP:
                    delta = -1;
                    if (si.nPos == si.nMin) {
                        break;
                    }
                    ShiftUp(viewer);
                    break;
                case SB_THUMBTRACK:
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
                    break;
            }
            SetScrollPos(hwnd, SB_VERT, si.nPos + delta, TRUE);
            RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
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
            if (viewer->mode == CUT_MODE) {
                UpdateHorScroll(model, viewer);
            }
            UpdateVertScroll(model, viewer);
            RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
            break;
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case IDM_MODE:
                    hmenu = GetMenu(hwnd);
                    state = GetMenuState(hmenu, IDM_MODE, MF_BYCOMMAND);
                    if (state == MF_CHECKED) {
                        state = MF_UNCHECKED;
                        ShowScrollBar(hwnd, SB_HORZ, TRUE);
                        UpdateHorScroll(model, viewer);
                    }
                    else if (state == MF_UNCHECKED) {
                        state = MF_CHECKED;
                        ShowScrollBar(hwnd, SB_HORZ, FALSE);
                    }
                    ChangeMode(viewer);
                    CheckMenuItem(hmenu, IDM_MODE, MF_BYCOMMAND | state);
                    RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
                    break;
                case IDM_EXIT:
                    DestroyWindow(hwnd);
                    break;
            }
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
