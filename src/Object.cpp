#include <splitspace/Object.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/Material.hpp>
#include <splitspace/Mesh.hpp>

namespace splitspace {

Object::Object(Engine *e, ObjectManifest *man, Entity *parent):
                                                Entity(e, man, parent)
{}

bool Object::load() {
    if(!m_manifest) {
        m_logMan->logErr("(Object) No manifest provided");
        return false;
    }

    if(m_manifest->name.empty()) {
        m_logMan->logErr("(Object) Emtpy Object names not supported");
        return false;
    }

    ObjectManifest *om = static_cast<ObjectManifest *>(m_manifest);
    if(om->materialManifest) {
        m_material = static_cast<Material *>(m_resMan->loadResource(om->materialManifest->name));
        if(!m_material) {
            return false;
        }
    } else {
        m_material = m_resMan->loadDefaultMaterial();
    }

    if(!om->meshManifest) {
        m_logMan->logErr("("+om->name+") No mesh manifest specified");
        return false;
    }

    m_mesh = static_cast<Mesh *>(m_resMan->loadResource(om->meshManifest->name));
    if(!m_mesh) {
        return false;
    }
    m_isLoaded = true;
    return true;    
}

void Object::unload() {
    m_logMan->logInfo("(Object) Unloading "+m_manifest->name);
    m_isLoaded = false;
}

} // namespace splitspace
