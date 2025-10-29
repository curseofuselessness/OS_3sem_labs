#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std;

int Size = 0;
int* arr = nullptr;

mutex arrMutex;
mutex coutMutex;

condition_variable startCV;
mutex startMutex;
bool startSignal = false;

vector<unique_ptr<mutex>> threadMutexes;
vector<unique_ptr<condition_variable>> threadCVs;

vector<bool> threadSignals;
vector<bool> threadTerminate;
vector<bool> threadsFinished;

void marker(int threadId) {
    unique_lock<mutex> startLock(startMutex);
    startCV.wait(startLock, [] { return startSignal; });
    startLock.unlock();

    srand(time(nullptr) + threadId + 1);

    int markedCount = 0;
    bool running = true;

    while (running) {
        int index = rand() % Size;

        {
            lock_guard<mutex> arrLock(arrMutex);
            if (arr[index] == 0) {
                this_thread::sleep_for(chrono::milliseconds(5));
                arr[index] = threadId + 1;
                this_thread::sleep_for(chrono::milliseconds(5));
                markedCount++;
                continue;
            }
        }

        {
            lock_guard<mutex> coutLock(coutMutex);
            cout << "Thread " << threadId + 1 << ": marked " << markedCount
                << " elements, cannot mark index " << index << endl;
        }

        {
            unique_lock<mutex> threadLock(*threadMutexes[threadId]);
            threadSignals[threadId] = true;
            threadCVs[threadId]->wait(threadLock, [&] {
                return threadTerminate[threadId] || !threadSignals[threadId];
                });

            if (threadTerminate[threadId]) {
                lock_guard<mutex> arrLock(arrMutex);
                for (int i = 0; i < Size; i++) {
                    if (arr[i] == threadId + 1) {
                        arr[i] = 0;
                    }
                }
                running = false;
            }
            else {
                threadSignals[threadId] = false;
            }
        }
    }
}

int main() {
    cout << "Enter array Size: ";
    cin >> Size;
    arr = new int[Size]();

    int countOfThreads = 0;
    cout << "Enter number of marker threads: ";
    cin >> countOfThreads;

    threadSignals.resize(countOfThreads, false);
    threadTerminate.resize(countOfThreads, false);
    threadsFinished.resize(countOfThreads, false);

    threadCVs.resize(countOfThreads);
    threadMutexes.resize(countOfThreads);
    for (int i = 0; i < countOfThreads; i++) {
        threadCVs[i] = make_unique<condition_variable>();
        threadMutexes[i] = make_unique<mutex>();
    }

    vector<thread> threads;
    for (int i = 0; i < countOfThreads; i++) {
        threads.emplace_back(marker, i);
    }

    {
        lock_guard<mutex> lock(startMutex);
        startSignal = true;
    }
    startCV.notify_all();

    while (true) {
        bool allSignaled = false;
        while (!allSignaled) {
            allSignaled = true;
            for (int i = 0; i < countOfThreads; i++) {
                if (!threadsFinished[i] && !threadSignals[i]) {
                    allSignaled = false;
                    break;
                }
            }
            if (!allSignaled) {
                this_thread::sleep_for(chrono::milliseconds(50));
            }
        }

        {
            lock_guard<mutex> lock(arrMutex);
            cout << "Current array: ";
            for (int i = 0; i < Size; i++) {
                cout << arr[i] << " ";
            }
            cout << "\n";
        }

        int stopIndex;
        cout << "Enter marker ID to terminate: ";
        cin >> stopIndex;

        if (stopIndex < 1 || stopIndex > countOfThreads || threadsFinished[stopIndex - 1]) {
            cout << "Invalid marker ID!\n";

            for (int i = 0; i < countOfThreads; i++) {
                if (!threadsFinished[i]) {
                    lock_guard<mutex> lock(*threadMutexes[i]);
                    threadSignals[i] = false;
                    threadCVs[i]->notify_one();
                }
            }
            continue;
        }

        int stopActualIndex = stopIndex - 1;

        {
            lock_guard<mutex> lock(*threadMutexes[stopActualIndex]);
            threadTerminate[stopActualIndex] = true;
        }
        threadCVs[stopActualIndex]->notify_one();

        threads[stopActualIndex].join();
        threadsFinished[stopActualIndex] = true;

        {
            lock_guard<mutex> lock(arrMutex);
            cout << "Array after termination of marker " << stopIndex << ": ";
            for (int i = 0; i < Size; i++) {
                cout << arr[i] << " ";
            }
            cout << "\n";
        }

        bool allDone = true;
        for (int i = 0; i < countOfThreads; i++) {
            if (!threadsFinished[i]) {
                allDone = false;
                break;
            }
        }

        if (allDone) {
            break;
        }

        for (int i = 0; i < countOfThreads; i++) {
            if (!threadsFinished[i]) {
                lock_guard<mutex> lock(*threadMutexes[i]);
                threadSignals[i] = false;
                threadCVs[i]->notify_one();
            }
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    delete[] arr;
    cout << "All threads finished. Program completed." << endl;
    return 0;
}