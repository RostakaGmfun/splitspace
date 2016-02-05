#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include <string>
#include <vector>
#include <map>

namespace splitspace{

class Scene;
class Engine;
class LogManager;
class ResourceManager;

class SceneManager {
public:
    SceneManager(Engine *e);
    ~SceneManager();

    bool init(std::vector<std::string> scenes);

    bool loadScene(std::string name);

    bool makeCurrent(std::string name);

    void update(float dt);

    void unloadScene(std::string name);

private:
    bool sceneCreate(std::string name);

private:
    std::map<std::string, Scene *> m_scenes;
    Scene *m_currentScene;
    Engine *m_engine;
    LogManager *m_logMan;
    ResourceManager *m_resMan;
};

} // namespace splitspace

#endif // SCENE_MANAGER_HPP
