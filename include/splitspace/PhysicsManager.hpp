#ifndef PHYSICS_MANAGER_HPP
#define PHYSICS_MANAGER_HPP

namespace splitspace {

class Engine;
class LogManager;

class PhysicsManager {
public:
    PhysicsManager(Engine *e);
    ~PhysicsManager();

    void destroy();

private:
    LogManager *m_logMan;

};

} // namespace splitspace

#endif // PHYSICS_MANAGER_HPP
