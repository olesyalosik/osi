#ifndef LAB2_98_HELPERFUNCTIONS_H
#define LAB2_98_HELPERFUNCTIONS_H

#include <tchar.h>
#include <strsafe.h>
#include <windows.h>
#include "params.h"

class HelperFunctions {
private:
    static const int maxMinTimeout = 7;
    static const int averageTimeout = 12;
    static const int BUF_SIZE = 200;

public:
    static DWORD WINAPI findMinMax(LPVOID lpParameter) {
        TCHAR msgBuf[BUF_SIZE];
        size_t cchStringSize;
        DWORD dwChars;

        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hStdout == INVALID_HANDLE_VALUE)
            return 1;

        MinMaxThreadParams *params;
        params = (MinMaxThreadParams *) lpParameter;
        params->arrayMax = params->arr[0];
        params->arrayMin = params->arr[0];

        for (int i = 0; i < params->numberOfElements; i++) {
            if (params->arr[i] > params->arrayMax) {
                params->arrayMax = params->arr[i];
                Sleep(maxMinTimeout);
            }
            if (params->arr[i] < params->arrayMin) {
                params->arrayMin = params->arr[i];
                Sleep(maxMinTimeout);
            }
        }

        StringCchPrintf(msgBuf, BUF_SIZE, TEXT("min = %d, max = %d\n"),
                        params->arrayMin, params->arrayMax);
        StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
        WriteConsole(hStdout, msgBuf, (DWORD) cchStringSize, &dwChars, NULL);

        return 0;
    }

    static DWORD WINAPI findAverage(LPVOID lpParameter) {
        TCHAR msgBuf[BUF_SIZE];
        size_t cchStringSize;
        DWORD dwChars;

        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hStdout == INVALID_HANDLE_VALUE)
            return 1;

        AverageThreadParams *params;
        params = (AverageThreadParams *) lpParameter;

        int sum = 0;
        for (int i = 0; i < params->numberOfElements; i++) {
            sum += params->arr[i];
            Sleep(averageTimeout);
        }
        params->arrayAverage = sum / params->numberOfElements;

        StringCchPrintf(msgBuf, BUF_SIZE, TEXT("average = %d\n"),
                        params->arrayAverage);
        StringCchLength(msgBuf, BUF_SIZE, &cchStringSize);
        WriteConsole(hStdout, msgBuf, (DWORD) cchStringSize, &dwChars, NULL);

        ExitThread(0);
    }

    static void ErrorHandler(LPCTSTR lpszFunction) {
        // Retrieve the system error message for the last-error code.

        LPVOID lpMsgBuf;
        LPVOID lpDisplayBuf;
        DWORD dw = GetLastError();

        FormatMessage(
                FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                dw,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL);

        // Display the error message.

        lpDisplayBuf = (LPVOID) LocalAlloc(LMEM_ZEROINIT,
                                           (lstrlen((LPCTSTR) lpMsgBuf) + lstrlen((LPCTSTR) lpszFunction) + 40) *
                                           sizeof(TCHAR));
        StringCchPrintf((LPTSTR) lpDisplayBuf,
                        LocalSize(lpDisplayBuf) / sizeof(TCHAR),
                        TEXT("%s failed with error %d: %s"),
                        lpszFunction, dw, lpMsgBuf);
        MessageBox(NULL, (LPCTSTR) lpDisplayBuf, TEXT("Error"), MB_OK);

        LocalFree(lpMsgBuf);
        LocalFree(lpDisplayBuf);
    }

};

#endif //LAB2_98_HELPERFUNCTIONS_H
