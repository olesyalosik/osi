#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

using std::mutex, std::condition_variable, std::unique_lock, std::vector, std::this_thread::sleep_for,
        std::chrono::milliseconds, std::cout, std::endl, std::cin, std::thread;

mutex globalMutex;
mutex mainMutex;
mutex *localMutex;

condition_variable *cvContinue;
condition_variable *cvFinish;
condition_variable *cvWait;

void markerFunc(int threadIndex, vector<int> &nums) {

    unique_lock uniqueLock(localMutex[threadIndex - 1]);
    globalMutex.lock();

    srand(threadIndex);
    int numberOfMarkedElements = 0;
    vector<int> markedIndexes;

    while (true) {
        int res = rand() % nums.size();
        if (nums[res] == 0) {
            numberOfMarkedElements++;
            markedIndexes.push_back(res);
            sleep_for(milliseconds(5));
            nums[res] = threadIndex;
            sleep_for(milliseconds(5));
        } else {

            cout << "Number of thread: " << threadIndex << " number of marked elements: " << numberOfMarkedElements
                 << " can't mark index: " << res << endl;

            cvWait[threadIndex - 1].notify_one();

            globalMutex.unlock();

            cvContinue[threadIndex - 1].wait_for(uniqueLock, std::chrono::seconds(150));

            if (cvFinish[threadIndex - 1].wait_for(uniqueLock, std::chrono::milliseconds(250)) ==
                std::cv_status::timeout) {
                std::lock_guard lg(globalMutex);
                for (int markedIndex: markedIndexes) {
                    nums[markedIndex] = 0;
                }
                return;
            } else
                globalMutex.lock();

        }
    }
}

bool checkThreads(const vector<bool> &canWork) {
    for (bool i: canWork)
        if (i)
            return true;
    return false;
}

int main() {
    unique_lock uniqueLock(mainMutex);
    globalMutex.lock();

    vector<int> nums;
    vector<thread> threads;
    int n;
    int numberOfThreads;
    int indexToBeKilled;

    cout << "Enter array size: " << endl;
    cin >> n;
    cout << "Enter number of threads: " << endl;
    cin >> numberOfThreads;

    nums.resize(n);
    vector<bool> canWork(numberOfThreads, true);
    localMutex = new mutex[numberOfThreads];
    cvContinue = new condition_variable[numberOfThreads];
    cvFinish = new condition_variable[numberOfThreads];
    cvWait = new condition_variable[numberOfThreads];

    for (int i = 0; i < numberOfThreads; i++) {
        globalMutex.unlock();
        threads.emplace_back(markerFunc, i + 1, ref(nums));
        cvWait[i].wait_for(uniqueLock, std::chrono::seconds(250));
        globalMutex.lock();
    }

    while (checkThreads(canWork)) {

        cout << "Array of numbers: " << endl;
        for (int i = 0; i < n; i++) {
            cout << nums[i] << " ";
        }

        cout << endl << "Enter index of thread to be killed: " << endl;
        cin >> indexToBeKilled;
        indexToBeKilled--;

        globalMutex.unlock();

        cvContinue[indexToBeKilled].notify_all();
        threads[indexToBeKilled].join();
        canWork[indexToBeKilled] = false;

        for (int i = 0; i < numberOfThreads; i++) {
            if (canWork[i]) {

                globalMutex.unlock();
                cvContinue[i].notify_all();
                std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(50));
                cvFinish[i].notify_all();
                cvWait[i].wait_for(uniqueLock, std::chrono::seconds(250));
                globalMutex.lock();

            }
        }

    }

    globalMutex.unlock();

    cout << "Array of numbers:" << endl;

    for (int i: nums) {
        cout << i << " ";
    }

    delete[] localMutex;
    delete[] cvWait;
    delete[] cvContinue;
    delete[] cvFinish;

    return 0;
}
