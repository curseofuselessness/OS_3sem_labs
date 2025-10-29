#ifndef LAB_2_H
#define LAB_2_H

#include <vector>
#include <thread>

extern std::vector<int> arr;
extern int minVal, maxVal;
extern int minIndex, maxIndex;
extern double averageVal;

void MinMaxThread();
void AverageThread();
void runThreadOperations();

#endif