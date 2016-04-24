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
    void destroy();

private:
    void doPass(Shader *shader);
    bool setupMaterial(Shader *shader, const Material *material);
    bool setupMesh(Shader *shader, const Mesh *mesh);

    void drawCall(std::size_t numVerts);
};

} // namespace splitspace

#endif // FWD_RENDER_TECHNIQUE_HPP
