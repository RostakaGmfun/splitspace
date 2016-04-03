#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <splitspace/Resource.hpp>
#include <splitspace/RenderManager.hpp>

namespace splitspace {

class LogManager;
struct TextureManifest: public ResourceManifest {
    TextureManifest(): ResourceManifest(RES_TEXTURE)
    {}
};

class Texture: public Resource {
public:
    Texture(Engine *e, TextureManifest *manifest);

    virtual bool load();
    virtual void unload();

private:
    int m_width;
    int m_height;
    int m_numChannels;
    unsigned char *m_data;
    GLuint m_glName;
};

} // namespace splitspace

#endif // TEXTURE_HPP
