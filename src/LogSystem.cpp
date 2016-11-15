#include "LogSystem.h"

LogSystemPtr LogSystem::s_log_system = nullptr;

LogSystem::LogSystem()
{
}

LogSystem::~LogSystem()
{
}

void LogSystem::log(string base, ...) const
{
    static char buffer[2048];
    memset(buffer, 0, 2048);

    va_list args;
    va_start(args, base);
    
#ifdef _WIN32
    vsprintf_s(buffer, base.c_str(), args);
#else
    vsprintf(buffer, base.c_str(), args);
#endif
    
    cout << "[info] " << buffer << endl;
}

void LogSystem::warn(string base, ...) const
{
    static char buffer[2048];
    memset(buffer, 0, 2048);

    va_list args;
    va_start(args, base);
#ifdef _WIN32
    vsprintf_s(buffer, base.c_str(), args);
#else
    vsprintf(buffer, base.c_str(), args);
#endif

    cout << "[warn] " << buffer << endl;
}

void LogSystem::err(string base, ...) const
{
    static char buffer[2048];
    memset(buffer, 0, 2048);

    va_list args;
    va_start(args, base);
#ifdef _WIN32
    vsprintf_s(buffer, base.c_str(), args);
#else
    vsprintf(buffer, base.c_str(), args);
#endif

    cout << "[ err] " << buffer << endl;
}

void LogSystem::todo(string base, ...) const
{
    static char buffer[2048];
    memset(buffer, 0, 2048);

    va_list args;
    va_start(args, base);
#ifdef _WIN32
    vsprintf_s(buffer, base.c_str(), args);
#else
    vsprintf(buffer, base.c_str(), args);
#endif

    cout << "[todo] " << buffer << endl;
}
