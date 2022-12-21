#include <iostream>
#include <windows.h>
#include <vector>
#include <cstring>

using namespace std;

bool checkProcesses(vector<bool> isFinished) {
    for (int i = 0; i < isFinished.size(); i++) {
        if (!isFinished[i]) {
            return false;
        }
    }
    return true;
}

int main() {
    char *filename = new char[50];
    int numberOfStrings, numberOfSenders;
    FILE *in;


    cout << "Enter binary file name:" << endl;
    cin >> filename;
    cout << "Enter number of strings: " << endl;
    cin >> numberOfStrings;
    cout << "Enter number of senders: " << endl;
    cin >> numberOfSenders;

    vector<char *> processArgs(numberOfSenders);
    vector<bool> isFinished(numberOfSenders, false);
    for (int i = 0; i < numberOfSenders; i++) {
        processArgs[i] = new char[100];
        strcpy(processArgs[i], "sender.exe ");
    }

    //Initialization of communication between processes
    HANDLE *isSenderReady = new HANDLE[numberOfSenders];
    HANDLE *senderContinue = new HANDLE[numberOfSenders];
    HANDLE *senderFinish = new HANDLE[numberOfSenders];

    for (int i = 0; i < numberOfSenders; i++) {
        char *senderIndex = new char[20];
        char *readyName = new char[20];
        char *continueName = new char[20];
        char *finishName = new char[20];
        itoa(i + 1, senderIndex, 10);

        strcpy(readyName, "sender");
        strcpy(continueName, "continue");
        strcpy(finishName, "finish");

        strcat(readyName, senderIndex);
        strcat(continueName, senderIndex);
        strcat(finishName, senderIndex);

        isSenderReady[i] = CreateEvent(NULL, FALSE, FALSE, readyName);
        senderContinue[i] = CreateEvent(NULL, FALSE, FALSE, continueName);
        senderFinish[i] = CreateEvent(NULL, FALSE, FALSE, finishName);

        if (isSenderReady[i] == NULL || senderContinue[i] == NULL || senderFinish[i] == NULL) {
            cout << "Event error " << GetLastError() << endl;
            delete[] isSenderReady;
            delete[] senderContinue;
            delete[] senderFinish;

            return 0;
        }

        strcat(processArgs[i], filename);
        strcat(processArgs[i], " ");
        strcat(processArgs[i], senderIndex);
    }

    HANDLE stringSemaphore = CreateSemaphore(NULL, numberOfStrings, numberOfStrings, "stringSemaphore");
    HANDLE senderSemaphore = CreateSemaphore(NULL, 0, numberOfStrings, "senderSemaphore");

    if (stringSemaphore == NULL || senderSemaphore == NULL) {
        cout << "Semaphore error " << GetLastError() << endl;
        delete[] isSenderReady;
        delete[] senderContinue;
        delete[] senderFinish;
        return 0;
    }

    //Initialize processes data
    STARTUPINFO *si = new STARTUPINFO[numberOfSenders];
    PROCESS_INFORMATION *pi = new PROCESS_INFORMATION[numberOfSenders];

    for (int i = 0; i < numberOfSenders; i++) {
        ZeroMemory(&si[i], sizeof(STARTUPINFO));
        si[i].cb = sizeof(STARTUPINFO);
        ZeroMemory(&pi[i], sizeof(PROCESS_INFORMATION));
    }

    //Creating processes
    for (int i = 0; i < numberOfSenders; i++) {
        if (!CreateProcess(NULL,
                           processArgs[i],
                           NULL,
                           NULL,
                           FALSE,
                           CREATE_NEW_CONSOLE,
                           NULL,
                           NULL,
                           &si[i],
                           &pi[i])
                ) {
            cout << "Creating process " << i << " failed" << GetLastError() << endl;

            delete[] si;
            delete[] pi;
            delete[] isSenderReady;
            delete[] senderContinue;
            delete[] senderFinish;

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
            fopen_s(&in, filename, "rb");
            char *msg = new char[20];
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
            ReleaseSemaphore(stringSemaphore, numberOfStrings, NULL);

            for (int i = 0; i < numberOfSenders; i++) {
                SetEvent(senderContinue[i]);
            }
        }

        //Sleep to wait for senders to continue
        Sleep(300);

        //Clear the file using write mode
        fopen_s(&in, filename, "wb");
        fclose(in);

        cout << "Waiting for messages" << endl;
    }

    delete[] si;
    delete[] pi;
    delete[] isSenderReady;

    delete[] senderContinue;
    delete[] senderFinish;

    return 0;
}