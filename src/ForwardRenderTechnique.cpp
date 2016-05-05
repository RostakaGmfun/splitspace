#include <splitspace/ForwardRenderTechnique.hpp>
#include <splitspace/RenderManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Scene.hpp>
#include <splitspace/Shader.hpp>
#include <splitspace/Material.hpp>
#include <splitspace/Mesh.hpp>
#include <splitspace/Camera.hpp>
#include <splitspace/Object.hpp>

namespace splitspace {

ForwardRenderTechnique::ForwardRenderTechnique(Engine *e): RenderTechnique(e),
                                                           m_shader(nullptr)
{}

ForwardRenderTechnique::~ForwardRenderTechnique() {
    destroy();
}

bool ForwardRenderTechnique::init() {
    Shader *defaultShader = static_cast<Shader*>(m_resManager->loadResource(m_resManager->getDefaultShader()));
    if(!defaultShader) {
        return false;
    }
    m_shader = defaultShader;
    return true;
}

void ForwardRenderTechnique::update(float dt) {
    static_cast<void>(dt);
}

void ForwardRenderTechnique::render() {
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    if(!m_scene) {
        return;
    }

    glUseProgram(m_shader->getProgramId());
    const auto &renderMap = m_scene->getRenderMap();
    const auto &lightList = m_scene->getLightList();
    int i = 0;
    for(const auto &l : lightList) {
        m_shader->setLight(i++, l);
    }
    m_shader->setNumLights(lightList.size());
    for(auto it : renderMap) {
        if(it.first) {
            if(!setupMaterial(m_shader, it.first)) {
                m_logManager->logErr("Failed to setup material");
                continue;
            }
        }

        for(auto o : it.second) {
            m_shader->setMVP(m_viewCamera->getVP()*o->getWorldMat());
            if(!setupMesh(m_shader, o->getMesh())) {
                m_logManager->logErr("Failed to setup mesh");
                continue;
            }
            drawCall(o->getMesh()->getNumVerts());
        }
    }
}

void ForwardRenderTechnique::destroy() {
    // ?
}

} // namepsace splitspace
