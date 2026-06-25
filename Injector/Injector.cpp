#include <iostream>
#include <Windows.h>
#include <string>


DWORD GetProcId(const char* procName)
{


    // Find the window of the game
    HWND acWindow = FindWindow(nullptr, "Cube 2: Sauerbraten");
    if (acWindow == NULL) {
        std::cout << "Failed to find game window." << std::endl;
        return 0;
    }

    DWORD processId;
    GetWindowThreadProcessId(acWindow, &processId);
    if (processId == 0) {
        std::cout << "Failed to get process ID." << std::endl;
    }

    return processId;
}

int main()
{
    const char* dllPath = "C:\\Users\\USER_NAME\\LOCATION\\TO_PATH\\x64\\Release\\ProofOfConcept.dll"; // IMPORTANT: Update path to dll
    const char* procName = "sauerbraten.exe";

    DWORD processId = GetProcId(procName);
    if (processId == 0) {
        std::cout << "Failed to get process ID. Is the game running?" << std::endl;
        return 1;
    }

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (hProc == NULL) {
        std::cout << "Failed to open process. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    size_t dllPathLen = strlen(dllPath) + 1; // Include null terminator
    void* spaceForDllPath = VirtualAllocEx(hProc, nullptr, dllPathLen, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (spaceForDllPath == NULL) {
        std::cout << "Failed to allocate memory in the target process. Error code: " << GetLastError() << std::endl;
        CloseHandle(hProc);
        return 1;
    }

    if (!WriteProcessMemory(hProc, spaceForDllPath, dllPath, dllPathLen, nullptr)) {
        std::cout << "Failed to write to process memory. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, spaceForDllPath, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return 1;
    }

    HANDLE hThread = CreateRemoteThread(hProc, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, spaceForDllPath, 0, nullptr);
    if (hThread == NULL) {
        std::cout << "Failed to create remote thread. Error code: " << GetLastError() << std::endl;
        VirtualFreeEx(hProc, spaceForDllPath, 0, MEM_RELEASE);
        CloseHandle(hProc);
        return 1;
    }

    // Wait for the thread to finish
    WaitForSingleObject(hThread, INFINITE);

    // Check the exit code of the thread
    DWORD exitCode;
    if (GetExitCodeThread(hThread, &exitCode) && exitCode == 0) {
        std::cout << "DLL injection failed. LoadLibraryA returned 0." << std::endl;
    }
    else {
        std::cout << "DLL injection successful." << std::endl;
    }

    VirtualFreeEx(hProc, spaceForDllPath, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProc);

    return 0;

}
