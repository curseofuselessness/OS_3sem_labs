#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>

using namespace std;

int countWithoutSpaces(char* str) {
    if (str == nullptr) return -1;
    int res = 0;
    while (*str != '\0'){
        if(*str != ' ')res++;
        str++;
    }
    return res;
}


struct employee {
    int num;
    char name[100] = "";
    double hours;
};

struct Params1 {
    char fileName[100] = "";
    int n;
    employee* employeeArray;
};

struct Params2 {
    int n;
    char fileNameBin[100];
    char fileNameReport[100];
    int hourPayment;
    employee* employeeArray;
};

Params2* params2_obj = new Params2();
Params1* params1_obj = new Params1();

DWORD WINAPI Creator(LPVOID lpParam) {
    params1_obj = reinterpret_cast<Params1*>(lpParam);

    ofstream binInput(params1_obj->fileName, ios::binary);

    if (!binInput.is_open()) {
        cerr << "dead";
        return 0;
    }

    for (int i = 0; i < params1_obj->n; i++) {
        cout << "Enter employee " << i + 1 << ":\n";

        cout << "Number: ";
        cin >> params1_obj->employeeArray[i].num;
        params2_obj->employeeArray[i].num = params1_obj->employeeArray[i].num;

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

DWORD WINAPI Reporter(LPVOID lpParam) {
    params2_obj = reinterpret_cast<Params2*>(lpParam);

    ifstream binInput(params2_obj->fileNameBin, ios::binary);
    
    if (!binInput.is_open()) {
        cout << "Error opening file! " << params2_obj->fileNameBin  << endl;
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


int main() {


    cout << "Enter amount of employees: ";
    char tempN[100] = "";
    cin.getline(tempN, 100);
    params1_obj->n = atoi(tempN);
    params2_obj->n = atoi(tempN);

    employee* employees = new employee[params1_obj->n];

    cout << "Enter binary file name: ";
    cin.getline(params1_obj->fileName, 100);

    strcpy(params2_obj->fileNameBin, params1_obj->fileName);

    params1_obj->employeeArray = employees;
    params2_obj->employeeArray = employees;

    HANDLE hThread = CreateThread(
        NULL,
        0,
        Creator,    
        params1_obj,
        0,
        NULL
    );

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    HANDLE hThread2 = CreateThread(
        NULL,
        0,
        Reporter,
        params2_obj,
        0,
        NULL
    );

    WaitForSingleObject(hThread2, INFINITE);
    CloseHandle(hThread2);

    return 0;
}