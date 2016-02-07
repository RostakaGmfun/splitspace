#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <string>
#include <map>
#include <vector>

namespace splitspace {

class Engine;
class LogManager;

class Texture;
class Shader;
class Mesh;
class Material;
class Object;

struct TextureManifest;
struct ObjectManifest;
struct MeshManifest;
struct MaterialManifest;
struct ShaderManifest;

class ResourceManager {
public:
    ResourceManager(Engine *e);
    ~ResourceManager();

    bool loadMaterialLib(std::vector<std::string> ml);
    bool loadMaterialLib(std::string name);

    void destroy();

    bool addTextureManifest(TextureManifest *tm);
    bool addObjectManifest(ObjectManifest *om);
    bool addMeshManifest(MeshManifest *mm);
    bool addMaterialManifest(MaterialManifest *mm);
    bool addShaderManifest(ShaderManifest *mm);
    
    TextureManifest *getTextureManifest(std::string name);
    ObjectManifest *getObjectManifest(std::string name);
    MeshManifest *getMeshManifest(std::string name);
    MaterialManifest *getMaterialManifest(std::string name);
    ShaderManifest *getShaderManifest(std::string name);
    
    Texture *loadTexture(std::string name);
    Shader *loadShader(std::string name);
    Mesh *loadMesh(std::string name);
    Material *loadMaterial(std::string name);
    Object *loadObject(std::string name);
    
    void unloadTexture(std::string name);
    void unloadShader(std::string name);
    void unloadMesh(std::string name);
    void unloadMaterial(std::string name);
    void unloadObject(std::string name);

    int collectGarbage();

    void unloadAll();
    void unloadTextures();
    void unloadShaders();
    void unloadMeshes();
    void unloadMaterials();
    void unloadObjects();

private:
    TextureManifest *readTextureManifest(std::string name);

private:
    Engine *m_engine;
    LogManager *m_logMan;

    std::map<std::string, Texture *> m_textureCache;
    std::map<std::string, Shader *> m_shaderCache;
    std::map<std::string, Mesh *> m_meshCache;
    std::map<std::string, Material *> m_materialCache;
    std::map<std::string, Object *> m_objectCache;

    std::map<std::string, TextureManifest *> m_textureManifests;
    std::map<std::string, ObjectManifest *> m_objectManifests;
    std::map<std::string, MeshManifest *> m_meshManifests;
    std::map<std::string, MaterialManifest *> m_materialManifests;
    std::map<std::string, ShaderManifest *> m_shaderManifests;

};

} // namespace splitspace

#endif // RSOURCE_MANAGER_HPP
