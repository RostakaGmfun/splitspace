#include <splitspace/RenderTechnique.hpp>
#include <splitspace/Object.hpp>
#include <splitspace/Material.hpp>
#include <splitspace/Mesh.hpp>
#include <splitspace/Shader.hpp>

#include <GL/glew.h>
#include <GL/gl.h>

namespace splitspace {

bool RenderTechnique::setupMaterial(Shader *shader, const Material *m) {
    if(!shader || !m) {
        return false;
    }
    shader->setMaterial(m);
    return true;
}

bool RenderTechnique::setupMesh(Shader *shader, const Mesh *m) {
    if(!m || !shader || !m_viewCamera) {
        return false;
    }

    glBindVertexArray(m->getVAO());
    return true;
}

void RenderTechnique::drawCall(std::size_t numVerts) {
    glDrawArrays(GL_TRIANGLES, 0, numVerts);
}

} //namespace splitspace

