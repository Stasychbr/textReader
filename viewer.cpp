#include "viewer.h"
#include <stdlib.h>
#include <windows.h>
#include "model.h"

HFONT GetFont() {
    return CreateFont(0, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
                       ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_DONTCARE, "Consolas");
}

void UpdateLineLength(viewer_t* viewer) {
    RECT rc = {0};
    GetClientRect(viewer->hwnd, &rc);
    viewer->lineLength = rc.right / viewer->tm.tmAveCharWidth;
}

viewer_t* InitViewer(HWND hwnd) {
    viewer_t* viewer = (viewer_t*)malloc(sizeof(viewer_t));
    HDC hdc = GetDC(hwnd);
    if (!viewer) {
        return NULL;
    }
    viewer->hwnd = hwnd;
    viewer->mode = CUT_MODE;
    viewer->font = GetFont();
    viewer->bcgrBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, viewer->font);
    GetTextMetrics(hdc, &(viewer->tm));
    UpdateLineLength(viewer);
    UpdatePageSize(viewer);
    viewer->curStrNum = 0;
    viewer->horShift = 0;
    return viewer;
}

void ShiftRight(viewer_t* viewer) {
    viewer->horShift++;
}

void ShiftLeft(viewer_t* viewer) {
    if (viewer->horShift > 0) {
        viewer->horShift--;
    }
}

void ShiftDown(viewer_t* viewer) {
    viewer->curStrNum++;
}

void ShiftUp(viewer_t* viewer) {
    if (viewer->curStrNum > 0) {
        viewer->curStrNum--;
    }
}

void UpdatePageSize(viewer_t* viewer) {
    RECT rc;
    GetClientRect(viewer->hwnd, &rc);
    viewer->pageSize = rc.bottom / viewer->tm.tmHeight;
}

void Draw(HDC hdc, model_t* model, viewer_t* viewer) {
    RECT rc;
    int i = 0, y;
    int lineHeight = viewer->tm.tmHeight, lineLength;
    int curStrNum = viewer->curStrNum;
    char* curStr;
    GetClientRect(viewer->hwnd, &rc);
    SelectObject(hdc, viewer->font);
    FillRect(hdc, &rc, viewer->bcgrBrush);
    for (y = 0; y < rc.bottom && i < model->strNum; y += lineHeight) {
        if (viewer->horShift >= model->strSizes[curStrNum + i]) {
            curStr = "";
            lineLength = 0;
        }
        else {
            curStr = model->strings[curStrNum + i] + viewer->horShift;
            lineLength = viewer->lineLength < model->strSizes[curStrNum + i] - viewer->horShift ?
            viewer->lineLength : model->strSizes[curStrNum + i] - viewer->horShift;
        }
        TextOut(hdc, 0, y, curStr, lineLength);
        i++;
    }
}

void DeleteViewer(viewer_t* viewer) {
    DeleteObject(viewer->font);
    DeleteObject(viewer->bcgrBrush);
    free(viewer);
}
