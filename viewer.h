#pragma once
#include "model.h"
#include <windows.h>

typedef enum viewMode {
    CUT_MODE,
    WRAP_MODE,
} viewMode;

typedef struct viewer_t {
    HWND hwnd;
    HFONT font;
    //HBRUSH bcgrBrush;
    TEXTMETRIC tm;
    int curStrNum;
    int horShift;
    int lineLength;
    int pageSize;
    viewMode mode;
} viewer_t;

viewer_t* InitViewer(HWND hwnd);

void Draw(HDC hdc, model_t* model, viewer_t* viewer);

void UpdateLineLength(viewer_t* viewer);

void DeleteViewer(viewer_t* viewer);

void ShiftRight(viewer_t* viewer);

void ShiftLeft(viewer_t* viewer);

void ShiftUp(viewer_t* viewer);

void ShiftDown(viewer_t* viewer);

void UpdatePageSize(viewer_t* viewer);

void ChangeMode(viewer_t* viewer);
