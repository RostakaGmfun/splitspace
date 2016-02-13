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
    LogManager(const std::string &sinkFname = "");
    ~LogManager();

    void logErr(const std::string &str);
    void logWarn(const std::string &str);
    void logInfo(const std::string &str);

    void flush();

    void setLevel(LogLevel l) { m_logLevel = l; }

private:
    LogLevel m_logLevel;
    std::ofstream m_sink;
};

} // namespace splitspace

#endif // LOG_MANAGER_H
