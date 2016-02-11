#include <splitspace/LogManager.hpp>

#include <iostream>

namespace splitspace {

LogManager::LogManager(const std::string &sinkFname) {
    if(!sinkFname.empty()) {
        m_sink.open(sinkFname);
        if(!m_sink.is_open()) {
            std::cerr << "[Error] (LogManager) Error opening " << sinkFname << std::endl;
        }
    }
}

LogManager::~LogManager() {
}

void LogManager::logErr(const std::string &str) {
    if(str.empty())
        return;
    std::cerr << "[Error] " << str << "\n";
    if(m_sink.is_open())
        m_sink << "[Error] " << str << "\n";
}

void LogManager::logWarn(const std::string &str) {
    if(m_logLevel<LOG_WARN || str.empty())
        return;
    std::cerr << "[Warning] " << str << "\n";
    if(m_sink.is_open())
        m_sink << "[Warning] " << str << "\n";
}

void LogManager::logInfo(const std::string &str) {
    if(m_logLevel<LOG_INFO || str.empty())
        return;
    std::cout << "[Info] " << str << "\n";
    if(m_sink.is_open())
        m_sink << "[Info] " << str << "\n";
}

void LogManager::flush() {
    std::cout << std::flush;
    std::cerr << std::flush;
    if(m_sink.is_open())
        m_sink << std::flush;
}

} // namespace splitspace

