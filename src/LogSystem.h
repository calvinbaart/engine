#ifndef _LOGSYSTEM_H_
#define _LOGSYSTEM_H_

#include "Config.h"

class LogSystem
{
private:
   static LogSystemPtr s_log_system;
public:
    LogSystem();
    ~LogSystem();

    void log(string base, ...) const;
    void warn(string base, ...) const;
    void err(string base, ...) const;
    void todo(string base, ...) const;

    static LogSystemPtr get()
    {
        if (s_log_system == nullptr)
        {
            s_log_system = NEW_0(LogSystem);
        }

        return s_log_system;
    }
};

#endif