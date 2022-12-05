///changes all min and max values with average value

#include <iostream>
#include <thread>
#include <vector>

using namespace std;

int minn, maxx, averagee;

void min_max(vector<int> arr, int n) {
    maxx = arr[0];
    minn = arr[0];
    for (int i = 0; i < n; i++) {
        if (arr[i] > maxx) {
            maxx = arr[i];
            this_thread::sleep_for(chrono::milliseconds(7));
        }
        if (arr[i] < minn) {
            minn = arr[i];
            this_thread::sleep_for(chrono::milliseconds(7));
        }
    }
    cout <<"min=" <<minn << "  max=" << maxx << "\n";
}

void average(vector<int> arr, int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
        this_thread::sleep_for(chrono::milliseconds(12));
    }
    averagee = sum / n;
    cout <<"average="<< averagee << "\n";
}

int main() {
    vector<int> arr;
    int n;
    cin >> n;
    arr.resize(n);
    for (int i = 0; i < n; i++) {
        cin >> arr[i];
    }
    thread th1(min_max, arr, n);
    thread th2(average, arr, n);
    th1.join();
    th2.join();
    for (int i: arr) {
        if (i == minn) {
            i = averagee;
        } else if (i == maxx) {
            i = averagee;
        }
        cout << i << " ";
    }
    return 0;
}
