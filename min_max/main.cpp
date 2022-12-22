///changes all min and max values with findAverage value

#include <iostream>
#include <thread>
#include <vector>

using std::vector;
using std::cout, std::cin, std::endl;
using std::thread, std::ref;

const int maxMinTimeout = 7;
const int averageTimeout = 12;

void findMinMax(vector<int> arr, int numberOfElements, int &arrayMin, int &arrayMax) {
    arrayMax = arr[0];
    arrayMin = arr[0];
    for (int i = 0; i < numberOfElements; i++) {
        if (arr[i] > arrayMax) {
            arrayMax = arr[i];
            std::this_thread::sleep_for(std::chrono::milliseconds(maxMinTimeout));
        }
        if (arr[i] < arrayMin) {
            arrayMin = arr[i];
            std::this_thread::sleep_for(std::chrono::milliseconds(maxMinTimeout));
        }
    }
    cout << "min=" << arrayMin << "  max=" << arrayMax << "\n";
}

void findAverage(vector<int> arr, int numberOfElements, int &arrayAverage) {
    int sum = 0;
    for (int i = 0; i < numberOfElements; i++) {
        sum += arr[i];
        std::this_thread::sleep_for(std::chrono::milliseconds(averageTimeout));
    }
    arrayAverage = sum / numberOfElements;
    cout << "findAverage=" << arrayAverage << "\n";
}

int main() {
    int arrayMin;
    int arrayMax;
    int arrayAverage;
    vector<int> arr;
    int numberOfElements;
    cout << "Enter number of elements: " << endl;
    cin >> numberOfElements;
    arr.resize(numberOfElements);
    cout << "Enter array elements: " << endl;
    for (int i = 0; i < numberOfElements; i++) {
        cin >> arr[i];
    }
    thread th1(findMinMax, arr, numberOfElements, ref(arrayMin), ref(arrayMax));
    thread th2(findAverage, arr, numberOfElements, ref(arrayAverage));
    th1.join();
    th2.join();
    for (int i: arr) {
        if (i == arrayMin || i == arrayMax) {
            i = arrayAverage;
        }
        cout << i << " ";
    }
    return 0;
}
