#pragma once

inline int countWithoutSpaces(char* str) {
    if (str == nullptr) return -1;
    int res = 0;
    while (*str != '\0') {
        if (*str != ' ') res++;
        str++;
    }
    return res;
}