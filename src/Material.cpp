#include <splitspace/Material.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Texture.hpp>
#include <splitspace/ResourceManager.hpp>

namespace splitspace {

Material::Material(Engine *e, MaterialManifest *man):
                              Resource(e, man),
                              m_diffuseMap(nullptr),
                              m_normalMap(nullptr)
{}

bool Material::load() {
    if(!m_manifest) {
        m_logMan->logErr("(Material) No manifest specified");
        return false;
    }

    if(m_manifest->name.empty()) {
        m_logMan->logErr("(Material) Empty resource names not supported");
        return false;
    }

    MaterialManifest *mm = static_cast<MaterialManifest *>(m_manifest);

    if(mm->diffuseMap) {
        m_diffuseMap = m_resMan->loadTexture(mm->diffuseMap->name);
        if(!m_diffuseMap) {
            m_logMan->logErr("("+mm->name+") Error loading diffuse map");
            return false;
        }
    }

    if(mm->normalMap) {
        m_normalMap = m_resMan->loadTexture(mm->normalMap->name);
        if(!m_normalMap) {
            m_logMan->logErr("("+mm->name+") Error loading normal map");
            return false;
        }
    }

    return true;
}

void Material::unload() {
    if(m_diffuseMap)
        m_diffuseMap->unload();
    if(m_normalMap)
        m_normalMap->unload();
    decRefCount();
}

} // namepsace splitspace
