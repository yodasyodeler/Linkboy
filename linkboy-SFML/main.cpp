#if defined(_WIN32)
#include <Windows.h>
#endif

#include <string.h>
#include "DebugLogger/CycleLogger.h"
#include "linkboy.h"


#if defined(_WIN32)
extern int main(int argc, char* argv[]);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	return main(__argc, __argv);
}
#else
#include <unistd.h>
void getExectuablePath(char* buf, const int bufLen) {
	int len = readlink("/proc/self/exe", buf, bufLen);
	while (buf[len-1] != '/')
		--len;
	buf[len] = '\0';
};
#endif // _WIN32

int main(int argc, char* argv[])
{
	const int PATHLENGTH = 200;
	char currentPath[PATHLENGTH];

	#if defined(_WIN32)
		strcpy_s(currentPath, PATHLENGTH, argv[0]);
	#else
		getExectuablePath(currentPath, PathLength);
	#endif

	if (argc > 1) {
		linkboy boy(currentPath, argv[1]);
		boy.startEmulation();
	}
	else {
		linkboy boy(currentPath);
		boy.startEmulation();
	}

	return 0;
}
