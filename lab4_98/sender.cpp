#include <iostream>
#include <cstring>
#include <windows.h>

using std::cout;
using std::cin;
using std::endl;

const int semaphoreTimeout = 25;

void writeMsg(FILE *out, const char *msg) {
    //need to use int instead of ulonglong
    int msgLen = strlen(msg) + 1;
    fwrite(&msgLen, sizeof(msgLen), 1, out);
    for (int i = 0; i < msgLen; i++) {
        fwrite(&msg[i], sizeof(char), 1, out);
    }
}

int main(int argc, char *argv[]) {
    const char *filename = argv[1];
    FILE *out;

    const char *senderIndexStr = argv[2];

    char *readyName = new char[20];
    char *continueName = new char[20];
    char *finishName = new char[20];
    strcpy(readyName, "sender");
    strcpy(continueName, "continue");
    strcpy(finishName, "finish");
    strcat(readyName, senderIndexStr);
    strcat(continueName, senderIndexStr);
    strcat(finishName, senderIndexStr);

    HANDLE senderReady = OpenEvent(EVENT_ALL_ACCESS, EVENT_MODIFY_STATE, readyName);
    HANDLE senderContinue = OpenEvent(EVENT_ALL_ACCESS, EVENT_MODIFY_STATE, continueName);
    HANDLE senderFinish = OpenEvent(EVENT_ALL_ACCESS, EVENT_MODIFY_STATE, finishName);

    HANDLE stringSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, SEMAPHORE_MODIFY_STATE, "stringSemaphore");
    HANDLE senderSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, SEMAPHORE_MODIFY_STATE, "senderSemaphore");

    char cmd;

    do {
        cout << "Enter 'w' to write message to file or 'q' to stop process" << endl;

        do {
            cin >> cmd;
            if (cmd != 'w' && cmd != 'q') {
                cout << "Wrong command, try again!" << endl;
                continue;
            }
        } while (!(cmd == 'w' || cmd == 'q'));

        if (cmd == 'q') {
            cout << "Exiting process" << endl;
            SetEvent(senderReady);
            SetEvent(senderFinish);
            break;
        }

        if (cmd == 'w') {
            fopen_s(&out, filename, "ab");

            char *msg = new char[20];
            cout << "Enter message(max msg len 20):" << endl;
            cin >> msg;

            if (WaitForSingleObject(stringSemaphore, semaphoreTimeout) == WAIT_TIMEOUT) {
                cout << "File is overloaded" << endl;
                delete[] msg;
                SetEvent(senderReady);
                fclose(out);
                WaitForSingleObject(senderContinue, INFINITE);
                if (WaitForSingleObject(senderFinish, semaphoreTimeout) == WAIT_TIMEOUT)
                    continue;
                else
                    break;
            } else {
                ReleaseSemaphore(senderSemaphore, 1, NULL);

                writeMsg(out, msg);

                fclose(out);
                delete[] msg;

                continue;
            }
        }
    } while (true);

    delete[] readyName;
    delete[] continueName;
    delete[] finishName;

}