#include <windows.h>
#include <iostream>
#include <cstdio>
#include <tchar.h>
#include <tlhelp32.h>
#include <time.h>

using namespace std;

DWORD rw = 0;
HANDLE handle;

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry))
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

int main(int argc, char* argv[])
{
	DWORD ID = 0;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			if (lstrcmp(entry.szExeFile, _T("blobby.exe")) == 0) {
				ID = entry.th32ProcessID;
			}
		}
	}

	CloseHandle(snapshot);

	uintptr_t moduleBase = GetModuleBaseAddress(ID, L"blobby.exe");

	handle = OpenProcess(PROCESS_ALL_ACCESS, false, ID);

	if (!handle) {
		cout << "Fehler! PID nicht gefunden!" << endl;
		system("pause");
		return 0;
	}

	uintptr_t baseWithOffset = moduleBase + 0x001653C8;

	float posPlayerX;
	float posPlayerY;

	float posEnemyX;
	float posEnemyY;

	float posBallX;
	float posBallY;

	int pointsPlayer;
	int pointsEnemy;

	byte leftPressed;
	byte rightPressed;
	byte jumpPressed;

	int pointer;

	SIZE_T sizeOfInt = sizeof(pointsPlayer);
	SIZE_T sizeOfFloat = sizeof(posPlayerX);
	SIZE_T sizeOfByte = sizeof(leftPressed);

	time_t start = time(0);
	//for(int i=0; i<75; i++) {
	while(true) {
		//Positions
		ReadProcessMemory(handle, (LPCVOID)baseWithOffset, &pointer, sizeOfInt, &rw);
		ReadProcessMemory(handle, (LPCVOID)pointer, &pointer, sizeOfInt, &rw);

		ReadProcessMemory(handle, (LPCVOID)(pointer+0x00), &posEnemyX, sizeOfFloat, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x04), &posEnemyY, sizeOfFloat, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x08), &posPlayerX, sizeOfFloat, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x0C), &posPlayerY, sizeOfFloat, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x10), &posBallX, sizeOfFloat, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x14), &posBallY, sizeOfFloat, &rw);

		//Points
		ReadProcessMemory(handle, (LPCVOID)baseWithOffset, &pointer, sizeOfInt, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x34), &pointer, sizeOfInt, &rw);

		ReadProcessMemory(handle, (LPCVOID)(pointer+0x08), &pointsEnemy, sizeOfInt, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x0C), &pointsPlayer, sizeOfInt, &rw);
		
		//Keys
		ReadProcessMemory(handle, (LPCVOID)baseWithOffset, &pointer, sizeOfInt, &rw);

		ReadProcessMemory(handle, (LPCVOID)(pointer+0x17), &leftPressed, sizeOfByte, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x18), &rightPressed, sizeOfByte, &rw);
		ReadProcessMemory(handle, (LPCVOID)(pointer+0x19), &jumpPressed, sizeOfByte, &rw);

		cout << posPlayerX << ";" << posPlayerY << ";" << posEnemyX  << ";" << posEnemyY << ";" << posBallX << ";" << posBallY << ";" << pointsEnemy << ";" << pointsPlayer << ";" << (int)leftPressed << ";" << (int)rightPressed << ";" << (int)jumpPressed << endl;
		
		Sleep(12);
	}
	time_t end = time(0);
	double time = difftime(end, start)/2;

	std::cout << std::endl << std::endl << "Zeit: " << time << std::endl;

	CloseHandle(handle);
	return 0;
}
