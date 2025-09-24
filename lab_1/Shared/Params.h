#pragma once
#include "Employee.h"

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