#include <iostream>
#include <windows.h>
#include <vector>
#include "Lab_2.h"

using namespace std;

// Define the global variables
vector<int> arr;
int minVal, maxVal;
int minIndex, maxIndex;
double averageVal;

DWORD WINAPI MinMaxThread(LPVOID lpParam) {
    minVal = arr[0];
    maxVal = arr[0];
    minIndex = 0;
    maxIndex = 0;

    for (int i = 1; i < arr.size(); i++) {
        if (arr[i] < minVal) {
            minVal = arr[i];
            minIndex = i;
        }
        Sleep(7);

        if (arr[i] > maxVal) {
            maxVal = arr[i];
            maxIndex = i;
        }
        Sleep(7);
    }

    cout << "Min value: " << minVal << " at index " << minIndex << endl;
    cout << "Max value: " << maxVal << " at index " << maxIndex << endl;

    return 0;
}

DWORD WINAPI AverageThread(LPVOID lpParam) {
    long long sum = 0;

    for (int i = 0; i < arr.size(); i++) {
        sum += arr[i];
        Sleep(12);
    }

    averageVal = static_cast<double>(sum) / arr.size();
    cout << "Average value: " << averageVal << endl;

    return 0;
}

void runThreadOperations() {
    HANDLE hMinMax = CreateThread(NULL, 0, MinMaxThread, NULL, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, AverageThread, NULL, 0, NULL);

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    arr[minIndex] = static_cast<int>(averageVal);
    arr[maxIndex] = static_cast<int>(averageVal);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);
}

int main() {
    int n;
    cout << "Enter array size: ";
    cin >> n;

    arr.resize(n);
    cout << "Enter " << n << " integers:\n";
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    runThreadOperations();

    cout << "\nModified array:\n";
    for (int val : arr) {
        cout << val << " ";
    }
    cout << endl;

    return 0;
}