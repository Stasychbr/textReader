#include <stdlib.h>
#include <stdio.h>
#include "model.h"

char* ReadText(FILE* file, int* strNum) {
    char* text = NULL;
    int size = 0, i, ch = 0;
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    text = (char*)malloc(size + 1);
    if (!text) {
        return NULL;
    }
    fseek(file, 0, SEEK_SET);
    fread(text, sizeof(char), size, file);
    text[size] = 0;
    return text;
}

int GetStrNumber(const char* text) {
    int counter = 1;
    while (*text) {
        if (*text == '\n') {
            counter++;
        }
        text++;
    }
    return counter;
}

model_t* InitModel(const char* fileName) {
    FILE* file = fopen(fileName, "rb");
    model_t* model;
    char* text;
    int sNum = 1, i = 0, j = 0;
    int* strSizes;
    char** strings;
    if (!file) {
        return NULL;
    }
    text = ReadText(file, &sNum);
    printf("%s", text);
    fclose(file);
    if (!text) {
        return NULL;
    }
    while (text[i]) {
        if (text[i++] == '\n') {
            sNum++;
        }
    }
    model = (model_t*)malloc(sizeof(model_t));
    strings = (char**)malloc((sNum + 1)* sizeof(char*));
    if (!model || !strings) {
        free(text);
        free(model);
        free(strings);
        return NULL;
    }
    strings[0] = text;
    strings[sNum] = text + i;
    for (i = 0; text[i]; i++) {
        if (text[i] == '\n') {
            strings[++j] = text + i + 1;
        }
    }
    model->strings = strings;
    model->strNum = sNum;
    return model;
}

int GetMaxStrLen(model_t* model) {
    int res = 0, i, curLen;
    for (i = 0; i < model->strNum; i++) {
        curLen = GetStrLen(model, i);
        if (curLen > res) {
            res = curLen;
        }
    }
    return res;
}

UINT GetStrLen(model_t* model, int ind) {
    UINT len =  model->strings[ind + 1] - model->strings[ind];
    if (ind < model->strNum - 1) {
        return len - 1;
    }
    else if (ind == model->strNum - 1) {
        return len;
    }
    return 0;
}

void DeleteModel(model_t* model) {
    free(model->strings[0]);
    free(model->strings);
    free(model);
}
