#ifndef FWD_RENDER_TECHNIQUE_HPP
#define FWD_RENDER_TECHNIQUE_HPP

#include <splitspace/RenderTechnique.hpp>

namespace splitspace {

class Material;
class Mesh;

class ForwardRenderTechnique: public RenderTechnique {
public:
    ForwardRenderTechnique(Engine *e);
    ~ForwardRenderTechnique();

    bool init();
    void update(float dt);
    void render();
    void destroy();

private:
    Shader *m_shader;
};

} // namespace splitspace

#endif // FWD_RENDER_TECHNIQUE_HPP
