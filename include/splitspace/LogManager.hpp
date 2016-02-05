#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include <string>
#include <fstream>

namespace splitspace {

enum LogLevel {
    LOG_ERROR,
    LOG_WARN,
    LOG_INFO
};

class LogManager {
public:
    LogManager(std::string sinkFname);
    ~LogManager();

    void logErr(std::string str);
    void logWarn(std::string str);
    void logInfo(std::string str);

    void flush();

    void setLevel(LogLevel l) { m_logLevel = l; }

private:
    LogLevel m_logLevel;
    std::ofstream m_sink;
};

} // namespace splitspace

#endif // LOG_MANAGER_H
