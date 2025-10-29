#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include "../Shared/Params.h"
#include "../Shared/Employee.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: Creator.exe <n> <binary_filename>" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    const char* fileName = argv[2];

    employee* employees = new employee[n];

    ofstream binOutput(fileName, ios::binary);
    if (!binOutput.is_open()) {
        cerr << "Error opening file: " << fileName << endl;
        delete[] employees;
        return 1;
    }

    for (int i = 0; i < n; i++) {
        cout << "Enter employee " << i + 1 << ":\n";

        cout << "Number: ";
        cin >> employees[i].num;

        cout << "Name: ";
        cin.ignore();
        cin.getline(employees[i].name, 50);

        cout << "Hours: ";
        cin >> employees[i].hours;

        binOutput.write(reinterpret_cast<char*>(&employees[i].num), sizeof(int));
        binOutput.write(employees[i].name, sizeof(employees[i].name));
        binOutput.write(reinterpret_cast<char*>(&employees[i].hours), sizeof(double));

        cout << "Success!\n";
    }

    binOutput.close();
    delete[] employees;

    return 0;
}