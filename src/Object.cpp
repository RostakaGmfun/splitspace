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
        m_material = m_resMan->loadMaterial(om->materialManifest->name);
        if(!m_material) {
            m_logMan->logErr("("+om->name+") Failed to load material");
            return false;
        }
    }

    if(!om->meshManifest) {
        m_logMan->logErr("("+om->name+") No mesh manifest specified");
        return false;
    }

    return true;    
}

void Object::update(float dt) {

}

void Object::unload() {

}

} // namespace splitspace
