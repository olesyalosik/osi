#include <iostream>
#include <string>
#include "employee.h"
#include <fstream>
#include <cstdlib>

using std::cout;
using std::cin;
using std::ofstream;
using std::endl;

int main(int argc, char **argv) {

    int numberOfRecords = atoi(argv[1]);
    const char *filename = argv[0];

    ofstream binaryOut(filename, std::ios::out | std::ios::binary);
    Employee e = {};
    cout << "Enter records(number, name, hours):" << endl;
    for (int i = 0; i < numberOfRecords; ++i) {
        cin >> e.num >> e.name >> e.hours;
        binaryOut.write((char *) &e, sizeof(Employee));
    }
    binaryOut.close();
    system("pause");
    return 0;
}