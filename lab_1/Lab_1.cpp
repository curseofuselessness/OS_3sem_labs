#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <Windows.h>
#include <string>

using namespace std;

int main() {
    char tempN[100] = "";
    char fileName[100] = "";

    cout << "Enter amount of employees: ";
    cin.getline(tempN, 100);

    cout << "Enter binary file name: ";
    cin.getline(fileName, 100);

    string creatorCmd = "Creator.exe ";
    creatorCmd += tempN;
    creatorCmd += " ";
    creatorCmd += fileName;

    STARTUPINFOA si1;
    PROCESS_INFORMATION pi1;

    ZeroMemory(&si1, sizeof(si1));
    si1.cb = sizeof(si1);
    ZeroMemory(&pi1, sizeof(pi1));

    BOOL success1 = CreateProcessA(
        NULL,
        (LPSTR)creatorCmd.c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si1,
        &pi1
    );

    if (!success1) {
        cout << "Failed to start Creator.exe. Error: " << GetLastError() << endl;
        return 1;
    }

    WaitForSingleObject(pi1.hProcess, INFINITE);
    CloseHandle(pi1.hProcess);
    CloseHandle(pi1.hThread);

    string reporterCmd = "Reporter.exe ";
    reporterCmd += tempN;
    reporterCmd += " ";
    reporterCmd += fileName;

    STARTUPINFOA si2;
    PROCESS_INFORMATION pi2;

    ZeroMemory(&si2, sizeof(si2));
    si2.cb = sizeof(si2);
    ZeroMemory(&pi2, sizeof(pi2));

    BOOL success2 = CreateProcessA(
        NULL,
        (LPSTR)reporterCmd.c_str(),
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si2,
        &pi2
    );

    if (!success2) {
        cout << "Failed to start Reporter.exe. Error: " << GetLastError() << endl;
        return 1;
    }

    WaitForSingleObject(pi2.hProcess, INFINITE);
    CloseHandle(pi2.hProcess);
    CloseHandle(pi2.hThread);

    return 0;
}
