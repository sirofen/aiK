// https://github.com/grenadium/ShugoConsole
#pragma once

#include <QString>

class WinTaskScheduler
{
public:
    static bool isTaskInstalled(const QString& taskName);
    static bool deleteTask(const QString& taskName);
    static bool createStartupTask(const QString& taskName, const QString& taskPath);
};
