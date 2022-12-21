#include <iostream>
#include <windows.h>
#include <vector>

using namespace std;

bool checkProcesses(const vector<bool>& isFinished) {
    for (bool i : isFinished) {
        if (!i) {
            return false;
        }
    }
    return true;
}

int main() {
    string filename;
    int numberOfStrings = 0;
    int  numberOfSenders = 0;
    FILE *in;


    cout << "Enter binary file name:" << endl;
    cin >> filename;
    cout << "Enter number of strings: " << endl;
    cin >> numberOfStrings;
    cout << "Enter number of senders: " << endl;
    cin >> numberOfSenders;

    vector<string> processArgs(numberOfSenders);
    vector<bool> isFinished(numberOfSenders, false);
    for (int i = 0; i < numberOfSenders; i++) {
        processArgs[i] ="sender.exe ";
    }

    //Initialization of communication between processes

    vector<HANDLE> isSenderReady(numberOfSenders);
    vector<HANDLE> senderContinue(numberOfSenders);
    vector<HANDLE> senderFinish(numberOfSenders);

    for (int i = 0; i < numberOfSenders; i++) {
        string readyName, continueName, finishName;
        string senderIndex = to_string(i + 1);

        readyName += "sender" + senderIndex;
        continueName += "continue" + senderIndex;
        finishName += "finish" + senderIndex;

        isSenderReady[i] = CreateEvent(nullptr, FALSE, FALSE, readyName.c_str());
        senderContinue[i] = CreateEvent(nullptr, FALSE, FALSE, continueName.c_str());
        senderFinish[i] = CreateEvent(nullptr, FALSE, FALSE, finishName.c_str());

        if (isSenderReady[i] == nullptr || senderContinue[i] == nullptr || senderFinish[i] == nullptr) {
            cout << "Event error " << GetLastError() << endl;
            return 0;
        }

        processArgs[i] += filename;
        processArgs[i] += " ";
        processArgs[i] += senderIndex;
    }

    HANDLE stringSemaphore = CreateSemaphore(nullptr, numberOfStrings, numberOfStrings, "stringSemaphore");
    HANDLE senderSemaphore = CreateSemaphore(nullptr, 0, numberOfStrings, "senderSemaphore");

    if (stringSemaphore == nullptr || senderSemaphore == nullptr) {
        cout << "Semaphore error " << GetLastError() << endl;
        return 0;
    }

    //Initialize processes data
    vector<STARTUPINFO> si(numberOfSenders);
    vector<PROCESS_INFORMATION> pi(numberOfSenders);

    for (int i = 0; i < numberOfSenders; i++) {
        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
    }

    //Creating processes
    for (int i = 0; i < numberOfSenders; i++) {
        if (!CreateProcess(nullptr,
                           processArgs[i].data(),
                           nullptr,
                           nullptr,
                           FALSE,
                           CREATE_NEW_CONSOLE,
                           nullptr,
                           nullptr,
                           &si[i],
                           &pi[i])
                ) {
            cout << "Creating process " << i << " failed" << GetLastError() << endl;

            return 0;
        }
    }

    char cmd;

    while (!checkProcesses(isFinished)) {
        for (int i = 0; i < numberOfSenders; i++) {
            if (!isFinished[i]) {
                WaitForSingleObject(isSenderReady[i], INFINITE);

                if (WaitForSingleObject(senderFinish[i], 25) != WAIT_TIMEOUT) {

                    isFinished[i] = true;

                    CloseHandle(pi[i].hProcess);
                    CloseHandle(pi[i].hThread);

                }
            }
        }

        cout << "Enter 'r' to read messages from file or 'q' to stop processes" << endl;
        do {
            cin >> cmd;
            if (cmd != 'q' && cmd != 'r') {
                cout << "Wrong command, try again!" << endl;
                continue;
            }
        } while (!(cmd == 'q' || cmd == 'r'));

        if (cmd == 'q') {
            for (int i = 0; i < numberOfSenders; i++)
                if (!isFinished[i]) {

                    //Wait for process to quit
                    SetEvent(senderContinue[i]);
                    Sleep(25);
                    SetEvent(senderFinish[i]);
                    Sleep(25);

                    CloseHandle(pi[i].hProcess);
                    CloseHandle(pi[i].hThread);
                }
            break;
        }



        if (cmd == 'r') {
            fopen_s(&in, filename.c_str(), "rb");
            auto *msg = new char[20];
            while (WaitForSingleObject(senderSemaphore, 30) != WAIT_TIMEOUT) {

                int msgLength = 0;
                fread(&msgLength, sizeof(msgLength), 1, in);
                for (int i = 0; i < msgLength; i++)
                {
                    fread(&msg[i], sizeof(char), 1, in);
                }

                cout << msg << endl;
            }
            fclose(in);
            delete[] msg;
            ReleaseSemaphore(stringSemaphore, numberOfStrings, nullptr);

            for (int i = 0; i < numberOfSenders; i++) {
                SetEvent(senderContinue[i]);
            }
        }

        //Sleep to wait for senders to continue
        Sleep(300);

        //Clear the file using write mode
        fopen_s(&in, filename.c_str(), "wb");
        fclose(in);

        cout << "Waiting for messages" << endl;
    }

    return 0;
}