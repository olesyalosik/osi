///changes all min and max values with average value

#include <iostream>
#include <windows.h>
#include "params.h"
#include "HelperFunctions.h"

using std::cout;
using std::cin;
using std::endl;

int main() {
    int arrayMin;
    int arrayMax;
    int arrayAverage;
    int *arr;
    int numberOfElements;

    cout << "Enter number of elements: " << endl;
    cin >> numberOfElements;

    arr = new int[numberOfElements];
    cout << "Enter array elements: " << endl;
    for (int i = 0; i < numberOfElements; i++) {
        cin >> arr[i];
    }

    MinMaxThreadParams minMaxParams = {};
    minMaxParams.arr = arr;
    minMaxParams.numberOfElements = numberOfElements;

    AverageThreadParams averageParams = {};
    averageParams.arr = arr;
    averageParams.numberOfElements = numberOfElements;

    HANDLE thread1 = CreateThread(NULL, 0, HelperFunctions::findMinMax, &minMaxParams, 0, 0);
    HANDLE thread2 = CreateThread(NULL, 0, HelperFunctions::findAverage, &averageParams, 0, 0);

    if (NULL == thread1 || NULL == thread2) {
        HelperFunctions::ErrorHandler(TEXT("CreateThread"));
        ExitProcess(3);
    }

    WaitForSingleObject(thread1, INFINITE);
    WaitForSingleObject(thread2, INFINITE);

    arrayMin = minMaxParams.arrayMin;
    arrayMax = minMaxParams.arrayMax;
    arrayAverage = averageParams.arrayAverage;

    for (int i = 0; i < numberOfElements; i++) {
        if (arr[i] == arrayMin || arr[i] == arrayMax) {
            arr[i] = arrayAverage;
        }
        cout << arr[i] << " ";
    }

    CloseHandle(thread1);
    CloseHandle(thread2);

    return 0;
}