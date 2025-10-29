#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include "Lab_2.h"

using namespace std;

vector<int> arr;
int minVal, maxVal;
int minIndex, maxIndex;
double averageVal;

void MinMaxThread() {
    minVal = arr[0];
    maxVal = arr[0];
    minIndex = 0;
    maxIndex = 0;

    for (int i = 1; i < arr.size(); i++) {
        if (arr[i] < minVal) {
            minVal = arr[i];
            minIndex = i;
        }
        this_thread::sleep_for(chrono::milliseconds(7));

        if (arr[i] > maxVal) {
            maxVal = arr[i];
            maxIndex = i;
        }
        this_thread::sleep_for(chrono::milliseconds(7));
    }

    cout << "Min value: " << minVal << " at index " << minIndex << endl;
    cout << "Max value: " << maxVal << " at index " << maxIndex << endl;
}

void AverageThread() {
    long long sum = 0;

    for (int i = 0; i < arr.size(); i++) {
        sum += arr[i];
        this_thread::sleep_for(chrono::milliseconds(12));
    }

    averageVal = static_cast<double>(sum) / arr.size();
    cout << "Average value: " << averageVal << endl;
}

void runThreadOperations() {
    thread minMaxThread(MinMaxThread);
    thread averageThread(AverageThread);

    minMaxThread.join();
    averageThread.join();

    arr[minIndex] = static_cast<int>(averageVal);
    arr[maxIndex] = static_cast<int>(averageVal);
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