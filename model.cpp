#include <stdlib.h>
#include <stdio.h>
#include "model.h"

char* ReadText(FILE* file, UINT* strNum) {
    char* text = NULL;
    UINT size = 0, i;
    int ch = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    text = (char*)malloc(size + 1);
    if (!text) {
        return NULL;
    }
    fseek(file, 0, SEEK_SET);
    //fread(text, sizeof(char), size, file);
    for (i = 0; (ch = getc(file)) != EOF; i++) {
        text[i] = (char) ch;
        if (ch == '\n') {
            (*strNum)++;
        }
    }
    text[i] = 0;
    return text;
}

UINT GetStrNumber(const char* text) {
    UINT counter = 1;
    while (*text) {
        if (*text == '\n') {
            counter++;
        }
        text++;
    }
    return counter;
}

model_t* InitModel(const char* fileName) {
    FILE* file = fopen(fileName, "r");
    model_t* model;
    char* text;
    UINT sNum = 1;
    UINT i, j = 0;
    UINT* strSizes;
    char** strings;
    if (!file) {
        return NULL;
    }
    text = ReadText(file, &sNum);
    fclose(file);
    if (!text) {
        return NULL;
    }
    model = (model_t*)malloc(sizeof(model_t));
    strSizes = (UINT*)calloc(sNum, sizeof(UINT));
    strings = (char**)malloc(sNum * sizeof(char*));
    if (!strSizes || !model || !strings) {
        free(text);
        return NULL;
    }
    strings[0] = text;
    for (i = 0; text[i]; i++) {
        strSizes[j]++;
        if (text[i] == '\n') {
            strings[++j] = text + i + 1;
        }
    }
    model->strings = strings;
    model->strSizes = strSizes;
    model->strNum = sNum;
    return model;
}

UINT GetMaxStrLen(model_t* model) {
    UINT res = 0, i;
    for (i = 0; i < model->strNum; i++) {
        if (model->strSizes[i] > res) {
            res = model->strSizes[i];
        }
    }
    return res;
}

void DeleteModel(model_t* model) {
    free(model->strings[0]);
    free(model->strings);
    free(model->strSizes);
    free(model);
}
