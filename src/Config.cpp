#include <splitspace/Config.hpp>

#include <fstream>
#include <vector>
#include "json/json.hpp"

using json = nlohmann::json;

namespace splitspace {

Config::Config()
{}

Config::~Config()
{}

bool Config::parse(const std::string &path) {
    std::ifstream f(path);
    if(!f.is_open()) {
        return false;
    }
    
    json jconfig;
    try {
        f >> jconfig;
    } catch(std::invalid_argument e) {
        std::cerr << "[" << path << "]" << "Parse error:" << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }

    try {
        auto jwindow = jconfig["window"];
        if(jwindow.is_null()) {
            fillDefaultWindow();
        } else {
            if(!jwindow.is_object()) {
                std::cerr << "[" << path << "]" << " window should be object!" << std::endl;
                return false;
            }
            window.width = jwindow["width"];
            window.height = jwindow["height"];
            window.fullscreen = jwindow["fullscreen"];
            window.vsync = jwindow["vsync"];
            window.caption = jwindow["caption"];
        }

        auto jlogging = jconfig["logging"];
        if(jlogging.is_null()) {
            fillDefaultLog();
        } else {
            if(jlogging["file"].is_null())
                log.logFile = "";
            else
                log.logFile = jlogging["file"];
            std::string lvl = jlogging["level"];
            if(lvl=="INFO")
                log.level = LOG_INFO;
            else if(lvl=="WARN")
                log.level = LOG_WARN;
            else if(lvl=="ERROR")
                log.level = LOG_ERROR;
            else 
                log.level = LOG_WARN; // default
        }
    } catch(std::domain_error e) {
        std::cerr << "[" << path << "]" << " Parse error:" << e.what() << std::endl;
        return false;
    }

    auto jscenes = jconfig["scenes"];
    if(!jscenes.is_null()) {
        if(!jscenes.is_array()) {
            std::cerr << "[" << path << "]" << " \"scenes\" expected to be array of strings" << std::endl;
            return false;
        }

        try {
            for(auto it = jscenes.begin();it!=jscenes.end();it++) {
                scenes.push_back(*it);
            }
        } catch(std::domain_error e) {
            std::cerr << "[" << path << "]" << " \"scenes\": expected array of strings" << std::endl;
            return false;
        }
    }

    auto jmatlibs = jconfig["materials"];
    if(!jmatlibs.is_null()) {

        if(!jmatlibs.is_array()) {
            std::cerr << "[" << path << "]" << " \"materials\" expected to be array of strings" << std::endl;
            return false;
        }

        try {
            for(auto it = jmatlibs.begin();it!=jmatlibs.end();it++) {
                matLibs.push_back(*it);
            }
        } catch(std::domain_error e) {
            std::cerr << path << " \"materials\": expected array of strings" << std::endl;
            return false;
        }
    }
    if(!jconfig["shaders"].is_null()) {
        shaderLib = jconfig["shaders"];
    }

    return true;
}

void Config::fillDefaultWindow() {
    window.width = 640;
    window.height = 480;
    window.fullscreen = false;
    window.vsync = false;
}

void Config::fillDefaultLog() {
    log.logFile = "";
    log.level = LOG_WARN;
}
} // namespace splitspace

