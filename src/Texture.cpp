#include <splitspace/Texture.hpp>
#include <splitspace/LogManager.hpp>

#include <SOIL/SOIL.h>

namespace splitspace {

Texture::Texture(Engine *e, TextureManifest *manifest): Resource(e, manifest),
                                                                 m_width(0),
                                                                 m_height(0),
                                                                 m_numChannels(0),
                                                                 m_data(nullptr),
                                                                 m_glName(0)
{}

Texture::~Texture() {
    unload();
}

bool Texture::load() {
    if(!m_manifest) {
        m_logMan->logErr("(Texture) No manifest specified");
        return false;
    }
    
    m_data = SOIL_load_image(m_manifest->name.c_str(), &m_width, &m_height,
                                          &m_numChannels, SOIL_LOAD_AUTO);
    if(!m_data) {
        m_logMan->logErr("(Texture) Error loading texture from file " + m_manifest->name);
        unload();
        return false;
    }
   
    ImageFormat format = IMAGE_RGB;
    switch(m_numChannels) {
        case 1:
            format = IMAGE_R;
        break;
        case 3:
            format = IMAGE_RGB;
        break;
        case 4:
            format = IMAGE_RGBA;
        break;
        default:
            m_logMan->logErr("(Texture) Unsupported texture format with "
                            +std::to_string(m_numChannels)+" channels.");
            unload();
            return false;
    }

    TextureManifest *manifest = static_cast<TextureManifest *>(m_manifest);
    if(!m_renderMan->createTexture(m_data, manifest->mipmappingEnabled,
                                           format,
                                           m_width,
                                           m_height,
                                           m_glName)) {
        m_logMan->logErr("(Texture) Error creating GL texture");
        unload();
        return false;
    }
                                         
    return true;
}

void Texture::unload() {

}

} // namespace splitspace
