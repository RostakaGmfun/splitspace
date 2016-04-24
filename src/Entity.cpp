#include <splitspace/Entity.hpp>
#include <splitspace/LogManager.hpp>

#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace splitspace {

Entity::Entity(Engine *e, EntityManifest *manifest,Entity *parent):
                                                  Resource(e, manifest),
                                                  m_parent(parent)
{
    if(m_parent) {
        m_parent->addChild(this);
    }
}

bool Entity::load() {
    EntityManifest *em = static_cast<EntityManifest *>(m_manifest);
    m_pos = em->pos;
    m_rot = glm::radians(em->rot);
    m_scale = em->scale;
    if(m_manifest->type == RES_ENTITY) {
        m_isLoaded = true;
    }
    return true;
}

bool Entity::setParent(Entity *p) {
    if(m_parent && m_parent!=p) {
        m_parent->removeChild(this);
    }

    m_parent = p;
    return true;

}

bool Entity::addChild(Entity *e) {
    if(!e) {
        return false;
    }

    if(e->getParent() && e->getParent()!=this) {
        e->getParent()->removeChild(e);
    }

    auto it = std::find(m_children.begin(), m_children.end(), e);

    if(it==m_children.end()) {
        m_children.push_back(e);
    } else {
        return false;
    }

    e->setParent(this);

    return true;
}

bool Entity::removeChild(Entity *e) {
    if(!e) {
        return false;
    }

    auto it = std::find(m_children.begin(), m_children.end(), e);

    if(it==m_children.end()) {
        return false;
    }

    m_children.erase(it);
    e->setParent(nullptr);
    return true;
}

void Entity::update(float dt) {
    updateTransform();
    updateChildren(dt);
}

void Entity::updateTransform() {
    m_world = glm::mat4(1);
//TODO
//    if(m_parent) {
//        m_world*=m_parent->getWorldMat();
//    }
    m_world = glm::translate(m_world, m_pos);
    m_world = glm::rotate(m_world, m_rot.y, glm::vec3(0,1,0));
    m_world = glm::rotate(m_world, m_rot.x, glm::vec3(1,0,0));
    m_world = glm::rotate(m_world, m_rot.z, glm::vec3(0,0,1));
    m_world = glm::scale(m_world, m_scale);
}

void Entity::updateChildren(float dt) {
    for(auto it = m_children.begin(); it!=m_children.end();it++) {
        (*it)->update(dt);
    }
}

void Entity::unload() {
    m_isLoaded = false;
}


} // namespace splitspace
