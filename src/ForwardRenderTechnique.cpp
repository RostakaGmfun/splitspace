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

ForwardRenderTechnique::ForwardRenderTechnique(Engine *e): RenderTechnique(e)
{}

ForwardRenderTechnique::~ForwardRenderTechnique() {
    destroy();
}

bool ForwardRenderTechnique::init() {
    m_passes.reserve(MAX_PASSES);
    Shader *defaultShader = static_cast<Shader*>(m_resManager->loadResource(m_resManager->getDefaultShader()));
    if(!defaultShader) {
        return false;
    }
    m_passes.push_back(defaultShader);
    return true;
}

void ForwardRenderTechnique::update(float dt) {
    static_cast<void>(dt);
}

void ForwardRenderTechnique::doPass(Shader *shader) {
    if(!m_scene) {
        return;
    }

    glUseProgram(shader->getProgramId());
    const auto &renderMap = m_scene->getRenderMap();
    const auto &lightList = m_scene->getLightList();
    int i = 0;
    for(const auto &l : lightList) {
        shader->setLight(i++, l);
    }
    shader->setNumLights(lightList.size());
    for(auto it : renderMap) {
        if(it.first) {
            if(!setupMaterial(shader, it.first)) {
                m_logManager->logErr("Failed to setup material");
                continue;
            }
        }

        for(auto o : it.second) {
            shader->setMVP(m_viewCamera->getVP()*o->getWorldMat());
            if(!setupMesh(shader, o->getMesh())) {
                m_logManager->logErr("Failed to setup mesh");
                continue;
            }
            drawCall(o->getMesh()->getNumVerts());
        }
    }
}

bool ForwardRenderTechnique::setupMaterial(Shader *shader, const Material *m) {
    if(!shader || !m) {
        return false;
    }
    shader->setMaterial(m);
    return true;
}

bool ForwardRenderTechnique::setupMesh(Shader *shader, const Mesh *m) {
    if(!m || !shader || !m_viewCamera) {
        return false;
    }

    glBindVertexArray(m->getVAO());
    return true;
}

void ForwardRenderTechnique::drawCall(std::size_t numVerts) {
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
}


void ForwardRenderTechnique::destroy() {
    // ?
}

} // namepsace splitspace
