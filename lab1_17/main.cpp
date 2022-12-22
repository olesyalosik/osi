#include <iostream>
#include <windows.h>
#include <fstream>
#include "employee.h"

using std::string, std::cin, std::cout, std::to_string, std::endl, std::ios, std::ifstream;

void startCreator(const string &binaryFileName, int numberOfRecords) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    string numberOfRecordsStr = to_string(numberOfRecords);
    string cmdArgs = binaryFileName + " " + numberOfRecordsStr;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(".\\creator.exe",
                       &cmdArgs[0],
                       nullptr,
                       nullptr,
                       FALSE,
                       CREATE_NEW_CONSOLE,
                       nullptr,
                       nullptr,
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

    ifstream binaryIn(binaryFileName, ios::binary | ios::in);
    Employee e{};
    for (int i = 0; i < numberOfRecords; ++i) {
        binaryIn.read((char *) &e, sizeof(Employee));
        cout << e.num << " " << e.name << " " << e.hours << endl;
    }


}

void startReporter(const string &binaryFileName, const string &reportName, int salary) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    string salaryStr = to_string(salary);
    string cmdArgs = binaryFileName + " " + reportName + " " + salaryStr;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(".\\reporter.exe",
                       &cmdArgs[0],
                       nullptr,
                       nullptr,
                       FALSE,
                       CREATE_NEW_CONSOLE,
                       nullptr,
                       nullptr,
                       &si,
                       &pi)
            ) {
        cout << "Launching reporter failed " << GetLastError();
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    ifstream in(reportName);

    string in_s;

    cout << "Report on file " << binaryFileName << endl;
    cout << "Employee number, name, working hours, salary" << endl;

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

    system("pause");

    return 0;
}