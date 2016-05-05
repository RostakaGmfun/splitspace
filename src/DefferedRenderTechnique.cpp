#include <splitspace/DefferedRenderTechnique.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/ResourceManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/Config.hpp>
#include <splitspace/Shader.hpp>
#include <splitspace/Camera.hpp>
#include <splitspace/Object.hpp>
#include <splitspace/Mesh.hpp>
#include <splitspace/Material.hpp>

namespace splitspace {

bool GBuffer::init(int w, int h) {
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);

    glGenTextures(GBUFFER_NUM_TARGETS, m_buffers);

    std::vector<GLenum> drawBuffers;

    for(int i = 0;i<GBUFFER_NUM_TARGETS;i++) {
        glBindTexture(GL_TEXTURE_2D, m_buffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, nullptr);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, 
                               GL_TEXTURE_2D, m_buffers[i], 0);
        drawBuffers.push_back(GL_COLOR+GL_COLOR_ATTACHMENT0+i);
    }

    glGenTextures(1, &m_depthBuffer);
    glBindTexture(GL_TEXTURE_2D, m_depthBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, w, h, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
                           GL_TEXTURE_2D, m_depthBuffer, 0);

    glDrawBuffers(drawBuffers.size(), drawBuffers.data());

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    return true;
}

void GBuffer::bindWrite() {
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

void GBuffer::bindRead() {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
}

DefferedRenderTechnique::DefferedRenderTechnique(Engine *e): RenderTechnique(e)
{}

DefferedRenderTechnique::~DefferedRenderTechnique() {
    destroy();
}

bool DefferedRenderTechnique::init() {
    m_gbuffer = new GBuffer();
    int w = m_engine->config->window.width;
    int h = m_engine->config->window.height;

    if(!m_gbuffer->init(w, h)) {
        m_logManager->logErr("(GBuffer) Failed to create framebuffer");
        return false;
    }

    m_firstPass = static_cast<Shader *>(m_resManager->loadResource("defferedShader_pass1"));
    if(!m_firstPass) {
        m_logManager->logErr("DefferedRenderTechnique) Failed to load first pass shader");
        return false;
    }

    m_secondPass = static_cast<Shader *>(m_resManager->loadResource("defferedShader_pass2"));
    if(!m_firstPass) {
        m_logManager->logErr("(DefferedRenderTechnique) Failed to load second pass shader");
        return false;
    }
    return true;
}

void DefferedRenderTechnique::update(float dt) {
    static_cast<void>(dt);
}

void DefferedRenderTechnique::render() {
    // First pass: render GBuffer (Position, Normal and Diffuse Color)
    m_gbuffer->bindWrite();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // copypasted from ForwardRenderTechnique
    // should do smth about it
    if(!m_scene) {
        return;
    }

    glUseProgram(m_firstPass->getProgramId());
    const auto &renderMap = m_scene->getRenderMap();
    for(auto it : renderMap) {
        if(it.first) {
            if(!setupMaterial(m_firstPass, it.first)) {
                m_logManager->logErr("Failed to setup material");
                continue;
            }
        }

        for(auto o : it.second) {
            m_firstPass->setMVP(m_viewCamera->getVP()*o->getWorldMat());
            if(!setupMesh(m_firstPass, o->getMesh())) {
                m_logManager->logErr("Failed to setup mesh");
                continue;
            }
            drawCall(o->getMesh()->getNumVerts());
        }
    }

    // Second pass: render to default framebuffer (screen)
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use GBuffer to fetch positions, normals, and colors
    m_gbuffer->bindRead();
}

void DefferedRenderTechnique::destroy() {

}


} // namepsace splitspace
