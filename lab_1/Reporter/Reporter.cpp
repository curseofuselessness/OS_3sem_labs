#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include "../Shared/Params.h"
#include "../Shared/Employee.h"
#include "../Shared/Utils.h"

using namespace std;

DWORD WINAPI Reporter(LPVOID lpParam) {
    Params2* params2_obj = reinterpret_cast<Params2*>(lpParam);

    ifstream binInput(params2_obj->fileNameBin, ios::binary);

    if (!binInput.is_open()) {
        cout << "Error opening file! " << params2_obj->fileNameBin << endl;
        return 1;
    }

    cout << "Text filname: ";
    cin.ignore();
    cin.getline(params2_obj->fileNameReport, 100);

    ofstream Output(params2_obj->fileNameReport);

    if (!Output.is_open()) {
        cout << "Error opening file! " << params2_obj->fileNameReport << endl;
        return 1;
    }

    cout << "Payment for hour: ";
    cin >> params2_obj->hourPayment;

    Output << "Number" << "\t" << "Name" << "\t" << "Hours" << "\t" << "Bill" << "\n";

    for (int i = 0; i < params2_obj->n; i++) {
        binInput.read(reinterpret_cast<char*>(&params2_obj->employeeArray[i]), sizeof(employee));

        Output << params2_obj->employeeArray[i].num << "\t";
        Output.write(params2_obj->employeeArray[i].name, countWithoutSpaces(params2_obj->employeeArray[i].name));
        Output << "\t";
        Output << params2_obj->employeeArray[i].hours << "\t";
        Output << params2_obj->employeeArray[i].hours * params2_obj->hourPayment << "\n";
    }

    binInput.close();
    Output.close();

    return 0;
}