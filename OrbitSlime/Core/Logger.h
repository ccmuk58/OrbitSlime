#pragma once

class Logger
{
public:
    static void Log(const char* message);
    static void LogFormat(const char* format, ...);
    static void LogWideFormat(const wchar_t* format, ...);
};
