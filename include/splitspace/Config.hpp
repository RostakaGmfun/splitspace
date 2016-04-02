#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <splitspace/LogManager.hpp>

#include <string>
#include <vector>

namespace splitspace {

struct WindowConfig {
    int width;
    int height;
    bool fullscreen;
    std::string caption;
    bool vsync;
};

struct LoggingConfig {
    LogLevel level;
    std::string logFile;
};

class Config {
public:
    Config();
    ~Config();

    bool parse(const std::string &path);
    
    WindowConfig window;
    LoggingConfig log;

    std::vector<std::string> scenes;
    std::vector<std::string> matLibs;
    std::string shaderLib;

private:
    void fillDefaultWindow();
    void fillDefaultLog();

};

} // namespace splitspace

#endif // CONFIG_HPP
