#include "Logger.h"

#include <cstdarg>
#include <cstdio>
#include <windows.h>

void Logger::Log(const char* message)
{
    if (!message) return;

    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}

void Logger::LogFormat(const char* format, ...)
{
    if (!format) return;

    char buffer[1024];

    va_list args;
    va_start(args, format);
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
    va_end(args);

    Log(buffer);
}

void Logger::LogWideFormat(const wchar_t* format, ...)
{
    if (!format) return;

    wchar_t buffer[1024];

    va_list args;
    va_start(args, format);
    vswprintf_s(buffer, format, args);
    va_end(args);

    OutputDebugStringW(buffer);
    OutputDebugStringW(L"\n");
}
