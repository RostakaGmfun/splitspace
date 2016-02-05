#include <splitspace/PhysicsManager.hpp>
#include <splitspace/Engine.hpp>

namespace splitspace {

PhysicsManager::PhysicsManager(Engine *e): m_logMan(e->logManager)
{}
    
PhysicsManager::~PhysicsManager() {
    destroy();
}

void PhysicsManager::destroy() {

}

} // namespace splitspace
