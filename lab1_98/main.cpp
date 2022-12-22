#include <iostream>
#include <windows.h>
#include <sstream>
#include <fstream>
#include "employee.h"

using std::string;
using std::stringstream;
using std::cout;
using std::endl;
using std::ifstream;
using std::cin;

void startCreator(const string &binaryFileName, int numberOfRecords) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    string numberOfRecordsStr;
    stringstream out;
    out << numberOfRecords;

    numberOfRecordsStr = out.str();
    string cmdArgs = binaryFileName + " " + numberOfRecordsStr;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(".\\creator.exe",
                       &cmdArgs[0],
                       NULL,
                       NULL,
                       FALSE,
                       CREATE_NEW_CONSOLE,
                       NULL,
                       NULL,
                       &si,
                       &pi)
            ) {
        cout << "Launching creator failed " << GetLastError();
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    cout << "Binary file " << binaryFileName << " contains:" << endl;

    ifstream binaryIn(binaryFileName.c_str(), std::ios::binary | std::ios::in);
    Employee e = {};
    for (int i = 0; i < numberOfRecords; ++i) {
        binaryIn.read((char *) &e, sizeof(Employee));
        cout << e.num << " " << e.name << " " << e.hours << endl;
    }
}

void startReporter(const string &binaryFileName, const string &reportName, int salary) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    string salaryStr;
    stringstream out;
    out << salary;

    salaryStr = out.str();
    string cmdArgs = binaryFileName + " " + reportName + " " + salaryStr;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(".\\reporter.exe",
                       &cmdArgs[0],
                       NULL,
                       NULL,
                       FALSE,
                       CREATE_NEW_CONSOLE,
                       NULL,
                       NULL,
                       &si,
                       &pi)
            ) {
        cout << "Launching reporter failed " << GetLastError();
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    ifstream in(reportName.c_str());

    string in_s;

    cout << "Отчет по файлу " << binaryFileName << endl;
    cout << "Номер сотрудника, имя сотрудника, часы, зарплата" << endl;

    int i = 0;
    while(getline(in, in_s)){
        if (i < 2) {
            i++;
            continue;
        }
        cout << in_s << endl;
        i++;
    }
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    string binaryFileName;
    string reportName;
    int numberOfRecords;
    int salary;

    cout << "Enter binary file name:" << endl;
    cin >> binaryFileName;
    cout << "Enter number of records: " << endl;
    cin >> numberOfRecords;

    startCreator(binaryFileName, numberOfRecords);


    cout << "Enter report name: " << endl;
    cin >> reportName;
    cout << "Enter salary: " << endl;
    cin >> salary;
    startReporter(binaryFileName, reportName, salary);
    return 0;
}