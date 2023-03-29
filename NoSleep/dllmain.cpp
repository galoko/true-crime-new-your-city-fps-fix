#include <Windows.h>
#include <cinttypes>
#include <profileapi.h>

LARGE_INTEGER Frequency;

VOID WINAPI MySleep(_In_ DWORD dwMilliseconds) {
    LARGE_INTEGER ElapsedMicroseconds, StartingTime, EndingTime;

    QueryPerformanceCounter(&StartingTime);

    while (true) {
        QueryPerformanceCounter(&EndingTime);
        ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;


        //
        // We now have the elapsed number of ticks, along with the
        // number of ticks-per-second. We use these values
        // to convert to the number of elapsed microseconds.
        // To guard against loss-of-precision, we convert
        // to microseconds *before* dividing by ticks-per-second.
        //

        ElapsedMicroseconds.QuadPart *= 1000000;
        ElapsedMicroseconds.QuadPart /= Frequency.QuadPart;

        if (ElapsedMicroseconds.QuadPart >= dwMilliseconds * 1000) {
            break;
        }
    }
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    QueryPerformanceFrequency(&Frequency);

    HMODULE hKernal = GetModuleHandleA("kernel32.dll");
    uint8_t* sleepPtr = (uint8_t*)GetProcAddress(hKernal, "Sleep");
    DWORD oldProtection;
    VirtualProtect(sleepPtr, 5, PAGE_EXECUTE_READWRITE, &oldProtection);

    sleepPtr[0] = 0xe9;
    *((DWORD*)(&sleepPtr[1])) = (DWORD)&MySleep - (DWORD)sleepPtr - 5;

    return TRUE;
}

