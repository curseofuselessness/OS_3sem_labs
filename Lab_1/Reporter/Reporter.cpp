#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include "../Shared/Params.h"
#include "../Shared/Employee.h"
#include "../Shared/Utils.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: Reporter.exe <n> <binary_filename>" << endl;
        return 1;
    }

    int n = atoi(argv[1]);
    const char* fileNameBin = argv[2];

    employee* employees = new employee[n];

    ifstream binInput(fileNameBin, ios::binary);
    if (!binInput.is_open()) {
        cout << "Error opening file: " << fileNameBin << endl;
        delete[] employees;
        return 1;
    }

    cout << "Text filename: ";
    char fileNameReport[100];
    cin.getline(fileNameReport, 100);

    ofstream Output(fileNameReport);
    if (!Output.is_open()) {
        cout << "Error opening file: " << fileNameReport << endl;
        delete[] employees;
        return 1;
    }

    cout << "Payment for hour: ";
    float hourPayment;
    cin >> hourPayment;

    Output << "Number\tName\tHours\tBill\n";

    for (int i = 0; i < n; ++i) {
        binInput.read(reinterpret_cast<char*>(&employees[i]), sizeof(employee));

        Output << employees[i].num << "\t";
        Output.write(employees[i].name, countWithoutSpaces(employees[i].name));
        Output << "\t" << employees[i].hours << "\t" << employees[i].hours * hourPayment << "\n";
    }

    binInput.close();
    Output.close();
    delete[] employees;

    return 0;
}