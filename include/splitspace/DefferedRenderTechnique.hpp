#ifndef DEFFERED_RENDER_TECHNIQUE_HPP
#define DEFFERED_RENDER_TECHNIQUE_HPP

#include <splitspace/RenderTechnique.hpp>
#include <GL/glew.h>
#include <GL/gl.h>

namespace splitspace {

class Material;
class Mesh;

enum GBUfferTarget {
    GBUFFER_POSITION,
    GBUFFER_NORMAL,
    GBUFFER_COLOR,

    GBUFFER_NUM_TARGETS
};

class GBuffer {
public:
    bool init(int w, int h);

    void bindWrite();
    void bindRead();

    void destroy();

private:
    GLuint m_buffers[GBUFFER_NUM_TARGETS];

    GLuint m_depthBuffer;
    GLuint m_fbo;
};

class DefferedRenderTechnique: public RenderTechnique {
public:
    DefferedRenderTechnique(Engine *e);
    ~DefferedRenderTechnique();

    bool init();
    void update(float dt);
    void render();
    void destroy();

private:
    GBuffer *m_gbuffer;
    Shader *m_firstPass;
    Shader *m_secondPass;
};

} // namepsace splitspace

#endif // DEFFERE_RENDER_TECHNIQUE_HPP
