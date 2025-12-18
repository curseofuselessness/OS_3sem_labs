#include "ThreadSafeEmployees.h"
#include <stdio.h>
#include <windows.h>
#include <iostream>
#include <memory>

using namespace std;

std::unique_ptr<ThreadSafeEmployees> employees_db;
int empsNum;

DWORD WINAPI InstanceThread(LPVOID param) {
    HANDLE hPipe = (HANDLE)param;
    int empNum;
    char command[20] = "";
    DWORD dwBytesRead, dwBytesWrote;
    BOOL fSuccess = FALSE;

    while (true) {
        fSuccess = ReadFile(hPipe, &command, sizeof(command), &dwBytesRead, NULL);
        if (!fSuccess || dwBytesRead == 0) {
            if (GetLastError() == ERROR_BROKEN_PIPE)
                printf("InstanceThread: client disconnected.\n");
            else
                printf("InstanceThread: ReadFile failed, GLE: %d.\n", GetLastError());
            break;
        }

        if (strcmp(command, "read") == 0) {
           
            fSuccess = ReadFile(hPipe, &empNum, sizeof(empNum), &dwBytesRead, NULL);
            if (!fSuccess || dwBytesRead == 0) {
                printf("InstanceThread: ReadFile failed, GLE: %d.\n", GetLastError());
                break;
            }

            Employee emp;
            bool found = employees_db->read_employee(empNum, emp);

            fSuccess = WriteFile(hPipe, &found, sizeof(bool), &dwBytesWrote, NULL);
            if (!fSuccess || dwBytesWrote == 0) {
                printf("InstanceThread: WriteFile failed, GLE: %d.\n", GetLastError());
                break;
            }

            if (found) {
                fSuccess = WriteFile(hPipe, &emp, sizeof(Employee), &dwBytesWrote, NULL);
                if (!fSuccess || dwBytesWrote == 0) {
                    printf("InstanceThread: WriteFile failed, GLE: %d.\n", GetLastError());
                    break;
                }
            }
        }
        else if (strcmp(command, "write") == 0) {
   
            fSuccess = ReadFile(hPipe, &empNum, sizeof(empNum), &dwBytesRead, NULL);
            if (!fSuccess || dwBytesRead == 0) {
                printf("InstanceThread: ReadFile failed, GLE: %d.\n", GetLastError());
                break;
            }

            Employee current_emp;
            bool found = employees_db->read_employee(empNum, current_emp);

            fSuccess = WriteFile(hPipe, &found, sizeof(bool), &dwBytesWrote, NULL);
            if (!fSuccess || dwBytesWrote == 0) {
                printf("InstanceThread: WriteFile failed, GLE: %d.\n", GetLastError());
                break;
            }

            if (found) {
  
                fSuccess = WriteFile(hPipe, &current_emp, sizeof(Employee), &dwBytesWrote, NULL);
                if (!fSuccess || dwBytesWrote == 0) {
                    printf("InstanceThread: WriteFile failed, GLE: %d.\n", GetLastError());
                    break;
                }

                Employee modified_emp;
                fSuccess = ReadFile(hPipe, &modified_emp, sizeof(Employee), &dwBytesRead, NULL);
                if (!fSuccess || dwBytesRead == 0) {
                    printf("InstanceThread: ReadFile failed, GLE: %d.\n", GetLastError());
                    break;
                }

                employees_db->write_employee(empNum, modified_emp);
            }
        }
        else if (strcmp(command, "exit") == 0) {
            break;
        }
    }

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    printf("Thread ended his work.\n");
    return 1;
}

int main()
{
    PROCESS_INFORMATION pi;
    STARTUPINFO si = { sizeof(si) };
    wchar_t commandLine[] = L"Client.exe";
    int clientsNum;

    printf("Type number of employees: ");
    scanf_s("%d", &empsNum);

    employees_db = std::make_unique<ThreadSafeEmployees>();

    printf("Type your employees (num, name, hours):\n");
    for (int i = 0; i < empsNum; i++) {
        Employee emp;
        printf("%d) ", i + 1);

        scanf_s("%d", &emp.num);
        scanf_s("%9s", emp.name, (unsigned)_countof(emp.name));
        scanf_s("%lf", &emp.hours);

        employees_db->add_employee(emp);
    }

    printf("\nCreated file contents:\n");
    auto all_emps = employees_db->get_all();
    for (const auto& emp : all_emps) {
        printf("%d %s %.2lf\n", emp.num, emp.name, emp.hours);
    }

    printf("\nType number of clients: ");
    scanf_s("%d", &clientsNum);

    HANDLE hPipe;
    HANDLE* hThreads = new HANDLE[clientsNum];
    DWORD dwThreadId;

    for (int i = 0; i < clientsNum; i++) {
        hPipe = CreateNamedPipe(
            L"\\\\.\\pipe\\server_pipe",
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            clientsNum,
            1024,  
            1024,  
            0,     
            NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            printf("Pipe %d creation failed, GLE: %d.\n", i + 1, GetLastError());
            delete[] hThreads;
            return -1;
        }

        if (!CreateProcess(NULL, commandLine, NULL, NULL, FALSE,
            CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
            printf("Process %d creation failed.\n", i + 1);
            CloseHandle(hPipe);
            continue;
        }
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);

        if (ConnectNamedPipe(hPipe, NULL)) {
            printf("Client %d connected.\n", i + 1);
            hThreads[i] = CreateThread(NULL, 0, InstanceThread, (LPVOID)hPipe, 0, &dwThreadId);
            if (hThreads[i] == NULL) {
                printf("Thread %d creation failed.\n", i + 1);
                CloseHandle(hPipe);
            }
        }
        else {
            CloseHandle(hPipe);
        }
    }


    WaitForMultipleObjects(clientsNum, hThreads, TRUE, INFINITE);


    for (int i = 0; i < clientsNum; i++) {
        if (hThreads[i] != NULL) {
            CloseHandle(hThreads[i]);
        }
    }
    delete[] hThreads;


    printf("\nModified file contents:\n");
    all_emps = employees_db->get_all();
    for (const auto& emp : all_emps) {
        printf("%d %s %.2lf\n", emp.num, emp.name, emp.hours);
    }


    printf("\nType 'exit' to finish server: ");
    char cmd[10];
    scanf_s("%s", cmd, (unsigned)_countof(cmd));

    return 0;
}