#include <fstream>
#include <iostream>
#include "employee.h"
#include <vector>
#include <string>
#include <cstdlib>

using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;

int main(int argc, char **argv) {

    const char *binaryName = argv[0];
    const char *reportName = argv[1];
    int salary = atoi(argv[2]);

    ifstream binaryIn(binaryName, std::ios::in | std::ios::binary);
    ofstream out(reportName);

    binaryIn.seekg(0, std::ios::end);
    int numberOfRecords = binaryIn.tellg() / sizeof(Employee);
    binaryIn.seekg(0, std::ios::beg);

    Employee *employees = new Employee[numberOfRecords];
    for (int i = 0; i < numberOfRecords; i++) {
        binaryIn.read((char *) &employees[i], sizeof(Employee));
    }

    out << "File report " << binaryName << endl;
    out << "Employee number, name, working hours, salary" << endl;
    for (int i = 0; i < numberOfRecords; i++) {
        out << employees[i].num << " " << employees[i].name << " " << employees[i].hours << " "
            << employees[i].hours * salary << endl;
    }

    out.close();
    binaryIn.close();
    cout << "Report created" << endl;
    system("pause");
    return 0;
}