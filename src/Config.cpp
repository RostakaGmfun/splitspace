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

bool Config::parse(std::string path) {
    std::ifstream f(path);
    if(!f.is_open())
        return false;
   
    json jconfig;
    f >> jconfig;

    try {
        auto jwindow = jconfig["window"];
        if(jwindow.is_null()) {
            fillDefaultWindow();
        } else {
            if(!jwindow.is_object()) {
                std::cerr << "[main.json] window should be object!" << std::endl;
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
        std::cerr << "[main.json] Parse error:" << e.what() << std::endl;
        return false;
    }

    auto jscenes = jconfig["scenes"];
    if(jscenes.is_null()) {
        std::cerr << "[main.json]  Error! no scenes found"  << std::endl;
        return false;
    }

    if(!jscenes.is_array()) {
        std::cerr << "[main.json] \"scenes\" expected to be array of strings" << std::endl;
        return false;
    }

    try {
        for(auto it = jscenes.begin();it!=jscenes.end();it++) {
            scenes.push_back(*it);
        }
    } catch(std::domain_error e) {
        std::cerr << "[main.json] \"scenes\": expected array of strings" << std::endl;
        return false;
    }
    
    auto jmatlibs = jconfig["materials"];
    if(jmatlibs.is_null()) {
        std::cerr << "[main.json]  Error! no material libraries found"  << std::endl;
        return false;
    }

    if(!jmatlibs.is_array()) {
        std::cerr << "[main.json] \"materials\" expected to be array of strings" << std::endl;
        return false;
    }

    try {
        for(auto it = jmatlibs.begin();it!=jmatlibs.end();it++) {
            matLibs.push_back(*it);
        }
    } catch(std::domain_error e) {
        std::cerr << "[main.json] \"materials\": expected array of strings" << std::endl;
        return false;
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

