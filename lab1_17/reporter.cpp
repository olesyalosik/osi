#include <fstream>
#include <iostream>
#include <vector>
#include <string>

#include "employee.h"

using std::string, std::ifstream, std::ofstream,
        std::cout, std::stoi, std::ios, std::vector, std::endl;

int main(int _argc, char **argv) {

    const char *binaryName = argv[0];
    const char *reportName = argv[1];
    int salary = stoi(argv[2]);

    ifstream binaryIn(binaryName, ios::in | ios::binary);
    ofstream out(reportName);

    binaryIn.seekg(0, ios::end);
    int numberOfRecords = binaryIn.tellg() / sizeof(Employee);
    binaryIn.seekg(0, ios::beg);

    vector<Employee> employees(numberOfRecords);
    for (int i = 0; i < numberOfRecords; i++) {
        binaryIn.read((char *) &employees[i], sizeof(Employee));
    }

    out << "Report on file " << binaryName << endl;
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