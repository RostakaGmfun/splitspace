#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <splitspace/Resource.hpp>

#define GLM_FORCE_RADIANS
#include <glm/vec3.hpp>
#include <glm/matrix.hpp>
#include <string>
#include <vector>

namespace splitspace {

class Entity;

struct EntityManifest: public ResourceManifest {
    EntityManifest(ResourceType t = RES_ENTITY): ResourceManifest(t)
    {}
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
    std::string parent;
};

class Entity: public Resource {
public:
    
    Entity(Engine *e, EntityManifest *manifest, Entity *parent = nullptr);
    
    const glm::mat4 &getWorldMat() const { return m_world; }
    const glm::vec3 &getPos() const { return m_pos; }
    const glm::vec3 &getRot() const { return m_rot; }
    const glm::vec3 &getScale() const { return m_scale; }

    void setPos(const glm::vec3 &pos) { m_pos = pos; }
    void setRot(const glm::vec3 &rot) { m_rot = rot; }
    void setScale(const glm::vec3 &scale) { m_scale = scale; }

    bool setParent(Entity *e);
    bool addChild(Entity *e);
    bool removeChild(Entity *e);
    std::vector<Entity *> getChildren() const { return m_children; }
    Entity *getParent() const { return m_parent; } 

    virtual void update(float dt);

    virtual bool load();
    virtual void unload();

protected:
    void updateTransform();
    void updateChildren(float dt);
    
protected:
    std::string m_name;
    glm::vec3 m_pos;
    glm::vec3 m_rot;
    glm::vec3 m_scale;
    glm::mat4 m_world;

    Entity *m_parent;
    std::vector<Entity *> m_children;

};

} // namespace splitspace

#endif // ENTITY_HPP
