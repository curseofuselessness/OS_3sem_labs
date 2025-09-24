#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include "../Shared/Params.h"
#include "../Shared/Employee.h"

using namespace std;

DWORD WINAPI Creator(LPVOID lpParam) {
    Params1* params1_obj = reinterpret_cast<Params1*>(lpParam);

    ofstream binInput(params1_obj->fileName, ios::binary);

    if (!binInput.is_open()) {
        cerr << "dead";
        return 0;
    }

    for (int i = 0; i < params1_obj->n; i++) {
        cout << "Enter employee " << i + 1 << ":\n";

        cout << "Number: ";
        cin >> params1_obj->employeeArray[i].num;

        cout << "Name: ";
        cin.ignore();
        cin.getline(params1_obj->employeeArray[i].name, 50);

        cout << "Hours: ";
        cin >> params1_obj->employeeArray[i].hours;

        binInput.write(reinterpret_cast<char*>(&params1_obj->employeeArray[i].num), sizeof(int));
        binInput.write(params1_obj->employeeArray[i].name, sizeof(params1_obj->employeeArray[i].name));
        binInput.write(reinterpret_cast<char*>(&params1_obj->employeeArray[i].hours), sizeof(double));

        cout << "Success!" << endl;
    }

    binInput.close();
    return 0;
}