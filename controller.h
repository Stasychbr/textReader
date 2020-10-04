#pragma once
#include "model.h"
#include "viewer.h"

void InitHorScroll(model_t* model, viewer_t* viewer);

void InitVertScroll(model_t* model, viewer_t* viewer);

void UpdateVertScroll(model_t* mode, viewer_t* viewer);

void UpdateHorScroll(model_t* mode, viewer_t* viewer);

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
