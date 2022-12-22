#include <iostream>
#include <string>
#include <fstream>

#include "employee.h"

using std::cout, std::endl, std::cin, std::ofstream, std::ios, std::stoi;

int main(int _argc, char **argv) {

    int numberOfRecords = stoi(argv[1]);
    const char *filename = argv[0];

    ofstream binaryOut(filename, ios::out | ios::binary);
    Employee e{};
    cout << "Enter records(number, name, hours):" << endl;
    for (int i = 0; i < numberOfRecords; ++i) {
        cin >> e.num >> e.name >> e.hours;
        binaryOut.write((char *) &e, sizeof(Employee));
    }
    binaryOut.close();
    system("pause");
    return 0;
}