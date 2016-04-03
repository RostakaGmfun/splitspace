#include <splitspace/Texture.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/ResourceManager.hpp>

#include <SOIL/SOIL.h>

namespace splitspace {

Texture::Texture(Engine *e, TextureManifest *manifest): Resource(e, manifest),
                                                                 m_width(0),
                                                                 m_height(0),
                                                                 m_numChannels(0),
                                                                 m_data(nullptr),
                                                                 m_glName(0)
{}

bool Texture::load() {
    if(!m_manifest) {
        m_logMan->logErr("(Texture) No manifest specified");
        return false;
    }
    
    std::string path = m_resMan->getResPath()+"textures/"+m_manifest->name;
    m_data = SOIL_load_image(path.c_str(), &m_width, &m_height,
                                          &m_numChannels, SOIL_LOAD_AUTO);
    if(!m_data) {
        m_logMan->logErr("(Texture) Error loading texture from file " + m_manifest->name);
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
            return false;
    }

    if(!m_renderMan->createTexture(m_data, format, m_width, m_height, m_glName)) {
        m_logMan->logErr("(Texture) Error creating GL texture");
        unload();
        return false;
    }
    m_isLoaded = true;
    return true;
}

void Texture::unload() {
    m_logMan->logInfo("(Texture) Unloading "+m_manifest->name);
    m_renderMan->destroyTexture(m_glName);
    m_isLoaded = false;
}

} // namespace splitspace
