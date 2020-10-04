#pragma once
#include <windows.h>

typedef struct model_t {
    char** strings;
    UINT* strSizes;
    UINT strNum;
} model_t;

model_t* InitModel(const char* fileName);

void DeleteModel(model_t* model);

UINT GetMaxStrLen(model_t* model);
