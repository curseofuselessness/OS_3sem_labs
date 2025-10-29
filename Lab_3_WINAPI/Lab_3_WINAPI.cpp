#include <iostream>
#include <windows.h>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

HANDLE* startEvents;
HANDLE* terminateEvents;
HANDLE* threadHandles;
HANDLE continueEvent;
HANDLE allSuspendedEvent;

int* arr;
int arraySize;
int threadCount;
bool* threadTerminated;
int suspendedThreadsCount;
CRITICAL_SECTION cs;

struct ThreadParams {


    int threadId;
};

DWORD WINAPI markerThread(LPVOID params) {

    ThreadParams* p = (ThreadParams*)params;
    int threadId = p->threadId;

    WaitForSingleObject(startEvents[threadId], INFINITE);

    srand(threadId);

    int markedCount = 0;
    bool working = true;

    while (working) {
        int randomNum = rand();
        int index = randomNum % arraySize;

        if (arr[index] == 0) {
            Sleep(5);
            arr[index] = threadId + 1;
            Sleep(5);
            markedCount++;
        }
        else {
            EnterCriticalSection(&cs);
            cout << "Поток " << threadId + 1 << ": "
                << "помечено элементов = " << markedCount << ", "
                << "нельзя пометить индекс = " << index << endl;
            LeaveCriticalSection(&cs);

            EnterCriticalSection(&cs);
            suspendedThreadsCount++;
            if (suspendedThreadsCount == threadCount) {
                SetEvent(allSuspendedEvent);
            }
            LeaveCriticalSection(&cs);

            HANDLE waitEvents[2] = { continueEvent, terminateEvents[threadId] };
            DWORD result = WaitForMultipleObjects(2, waitEvents, FALSE, INFINITE);

            if (result == WAIT_OBJECT_0 + 1) {

                EnterCriticalSection(&cs);

                for (int i = 0; i < arraySize; i++) {

                    if (arr[i] == threadId + 1) {

                        arr[i] = 0;

                    }

                }

                threadTerminated[threadId] = true;
                suspendedThreadsCount--;
                LeaveCriticalSection(&cs);

                working = false;
            }
            else {

                EnterCriticalSection(&cs);
                suspendedThreadsCount--;
                LeaveCriticalSection(&cs);

            }
        }
    }

    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");

    InitializeCriticalSection(&cs);

    cout << "Введите размерность массива: ";
    cin >> arraySize;

    arr = new int[arraySize];
    for (int i = 0; i < arraySize; i++) {
        arr[i] = 0;
    }

    cout << "Введите количество потоков marker: ";
    cin >> threadCount;

    if (threadCount <= 0 || threadCount > arraySize) {
        cout << "Некорректное количество потоков!" << endl;
        return 1;
    }

    startEvents = new HANDLE[threadCount];
    terminateEvents = new HANDLE[threadCount];
    threadHandles = new HANDLE[threadCount];
    threadTerminated = new bool[threadCount];

    continueEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    allSuspendedEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    for (int i = 0; i < threadCount; i++) {

        startEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        terminateEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);

        threadTerminated[i] = false;
    }

    suspendedThreadsCount = 0;

    ThreadParams* params = new ThreadParams[threadCount];

    for (int i = 0; i < threadCount; i++) {

        params[i].threadId = i;
        threadHandles[i] = CreateThread(NULL, 0, markerThread, &params[i], 0, NULL);

        if (threadHandles[i] == NULL) {

            cout << "Ошибка создания потока " << i + 1 << endl;
            return 1;

        }
    }

    cout << "Все потоки созданы. Запускаем работу..." << endl;

    for (int i = 0; i < threadCount; i++) {

        SetEvent(startEvents[i]);

    }

    while (true) {
        WaitForSingleObject(allSuspendedEvent, INFINITE);

        cout << "\nТекущее состояние массива:" << endl;

        for (int i = 0; i < arraySize; i++) {

            cout << arr[i] << " ";

        }

        cout << endl;

        bool allTerminated = true;

        for (int i = 0; i < threadCount; i++) {

            if (!threadTerminated[i]) {

                allTerminated = false;
                break;

            }
        }

        if (allTerminated) {

            break;

        }

        int threadToTerminate;
        cout << "\nВведите номер потока для завершения (1-" << threadCount << "): ";
        cin >> threadToTerminate;
        threadToTerminate--;

        if (threadToTerminate < 0 || threadToTerminate >= threadCount || threadTerminated[threadToTerminate]) {

            cout << "Некорректный номер потока!" << endl;
            continue;

        }

        SetEvent(terminateEvents[threadToTerminate]);

        WaitForSingleObject(threadHandles[threadToTerminate], INFINITE);

        cout << "\nСостояние массива после завершения потока " << threadToTerminate + 1 << ":" << endl;

        for (int i = 0; i < arraySize; i++) {

            cout << arr[i] << " ";

        }
        cout << endl;

        ResetEvent(allSuspendedEvent);

        SetEvent(continueEvent);
        Sleep(100);
        ResetEvent(continueEvent);
    }

    cout << "Все потоки завершены. Программа завершает работу." << endl;

    for (int i = 0; i < threadCount; i++) {
        CloseHandle(threadHandles[i]);
        CloseHandle(startEvents[i]);
        CloseHandle(terminateEvents[i]);
    }

    delete[] arr;
    delete[] startEvents;
    delete[] terminateEvents;
    delete[] threadHandles;
    delete[] threadTerminated;
    delete[] params;

    CloseHandle(continueEvent);
    CloseHandle(allSuspendedEvent);
    DeleteCriticalSection(&cs);

    return 0;
}