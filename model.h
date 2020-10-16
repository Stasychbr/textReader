#pragma once
#include <windows.h>

typedef struct model_t {
    //char* text;
    char** strings;
    //int* strSizes;
    int strNum;
} model_t;

model_t* InitModel(const char* fileName);

void DeleteModel(model_t* model);

int GetMaxStrLen(model_t* model);

UINT GetStrLen(model_t* model, int ind);
