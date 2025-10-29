#ifndef LAB_2_H
#define LAB_2_H

#include <vector>
#include <windows.h>

extern std::vector<int> arr;
extern int minVal, maxVal;
extern int minIndex, maxIndex;
extern double averageVal;

DWORD WINAPI MinMaxThread(LPVOID lpParam);
DWORD WINAPI AverageThread(LPVOID lpParam);
void runThreadOperations();

#endif