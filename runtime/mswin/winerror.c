#include <runtime/error.h>
#include <runtime/mswin/winapi.h>

void PlatformErrorMessage(const char* message)
{
    if (g_isGuiProgram)
        MessageBoxA(NULL, message, NULL, MB_ICONSTOP | MB_OK);
    else {
        HANDLE hStdErr = GetStdHandle(STD_ERROR_HANDLE);
        DWORD dwBytesWritten;
        WriteFile(hStdErr, "ERROR: ", 7, &dwBytesWritten, NULL);
        WriteFile(hStdErr, message, (DWORD)strlen(message), &dwBytesWritten, NULL);
        WriteFile(hStdErr, "\r\n", 2, &dwBytesWritten, NULL);
    }
}
