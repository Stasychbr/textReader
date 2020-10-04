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
    //viewer->bcgrBrush = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, viewer->font);
    GetTextMetrics(hdc, &(viewer->tm));
    UpdateLineLength(viewer);
    UpdatePageSize(viewer);
    viewer->curStrNum = 0;
    viewer->horShift = 0;
    ReleaseDC(hwnd, hdc);
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
    int i = 0, y, j = 0; // UINT
    int lineHeight = viewer->tm.tmHeight, lineLength;
    int curStrNum = viewer->curStrNum;
    char* curStr;
    BOOL nextLine;
    GetClientRect(viewer->hwnd, &rc);
    SelectObject(hdc, viewer->font); // watch docs!
    if (viewer->mode == CUT_MODE) {
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
    else if (viewer->mode == WRAP_MODE) {
        for (y = 0; y < rc.bottom && i < model->strNum; y += lineHeight) {
            curStr = model->strings[curStrNum + i];
                if (viewer->lineLength < model->strSizes[curStrNum + i] - j * viewer->lineLength) {
                    lineLength = viewer->lineLength;
                }
                else {
                    lineLength = model->strSizes[curStrNum + i] - j * viewer->lineLength;
                    nextLine = TRUE;
                }
            TextOut(hdc, 0, y, curStr + j * viewer->lineLength, lineLength);
            if (nextLine) {
                nextLine = FALSE;
                j = 0;
                i++;
            }
            else {
                j++;
            }
        }
    }
}

void ChangeMode(viewer_t* viewer) {
    if (viewer->mode == CUT_MODE) {
        viewer->mode = WRAP_MODE;
    }
    else if (viewer->mode == WRAP_MODE) {
        viewer->mode = CUT_MODE;
    }
}

void DeleteViewer(viewer_t* viewer) {
    DeleteObject(viewer->font);
    free(viewer);
}
