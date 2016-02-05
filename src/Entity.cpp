#include <splitspace/Entity.hpp>
#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

namespace splitspace {

Entity::Entity(Engine *e, EntityManifest *manifest,Entity *parent):
                                                  Resource(e, manifest),
                                                  m_parent(parent)
{}

bool Entity::setParent(Entity *p) {
    if(!p) {
        return false;
    }
     
    if(!p->addChild(this)) {
        return false;
    }
    
    if(m_parent) {
        m_parent->removeChild(this);
    }

    m_parent = p;
    return true;

}

bool Entity::addChild(Entity *e) {
    if(!e) {
        return false;
    }

    auto it = std::find(m_children.begin(), m_children.end(), e);

    if(it==m_children.end()) {
        m_children.push_back(e);
    }

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
    return true;
}

void Entity::update(float dt) {
    updateTransform();
    updateChildren(dt);
}

void Entity::updateTransform() {
    m_world = glm::translate(glm::mat4(1), m_pos);
    m_world = glm::rotate(m_world, m_rot.y, glm::vec3(0,1,0));
    m_world = glm::rotate(m_world, m_rot.x, glm::vec3(1,0,0));
    m_world = glm::rotate(m_world, m_rot.z, glm::vec3(0,0,1));
    m_world = glm::scale(m_world, m_scale);
    if(m_parent)
        m_world*=m_parent->getWorld();
}

void Entity::updateChildren(float dt) {
    for(auto it = m_children.begin(); it!=m_children.end();it++) {
        (*it)->update(dt);
    }
}


} // namespace splitspace
