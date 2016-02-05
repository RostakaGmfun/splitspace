#include <splitspace/RenderManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/WindowManager.hpp>
#include <splitspace/LogManager.hpp>

#include <chrono>


namespace splitspace {
RenderManager::RenderManager(Engine *e): m_winManager(e->windowManager),
                                         m_logManager(e->logManager),
                                         m_window(m_winManager->getSDLWindow()),
                                         m_frameDrawCalls(0),
                                         m_totalDrawCalls(0),
                                         m_totalShaders(0),
                                         m_totalMeshes(0),
                                         m_totalTextures(0),
                                         m_totalFrames(0),
                                         m_averageFrameTime(0),
                                         m_memoryUsed(0)

{}

RenderManager::~RenderManager() {
    destroy();
}

bool RenderManager::init(bool vsync) {
    m_context = SDL_GL_CreateContext(m_winManager->getSDLWindow());
    if(!m_context) {
        m_logManager->logErr("(RenderManager) Error creating GL context");
        return false;
    }
    
    SDL_GL_MakeCurrent(m_winManager->getSDLWindow(), m_context);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)) {
        m_logManager->logErr("(RenderManager) Error initialising OpenGL 3.3 context:"
                           +std::string(SDL_GetError()));
        return false;
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3)) {
        m_logManager->logErr("(RenderManager) Error initialising OpenGL 3.3 context:"
                           +std::string(SDL_GetError()));
        return false;
    }

    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)) {
        m_logManager->logErr("(RenderManager) Error initialising OpenGL 3.3 context:"
                           +std::string(SDL_GetError()));
        return false;
    } 
    
    if(glewInit() != GLEW_OK) {
        m_logManager->logInfo("(RenderManager) Failed to initialise GLEW");
        return false;
    }
    
    m_logManager->logInfo("(RenderManager) Created GL 3.3 context");


    if(vsync) {
        if(SDL_GL_SetSwapInterval(-1)) {
            m_logManager->logWarn("(RendrManager) Failed to enable late swap tearing, falling back to vsync");
            if(SDL_GL_SetSwapInterval(1)) {
                m_logManager->logWarn("(RenderManager) Failed to enable vsync");
            } else {
                m_logManager->logInfo("(RenderManager) vsync enabled"); 
            }
        } else {
            m_logManager->logInfo("(RenderManager) Enabled late swap tearing");
        }
    } else {
        SDL_GL_SetSwapInterval(0);
    }

    setupGL();

    return true;
}
    
bool RenderManager::createTexture(void *data, bool useMipmaps, ImageFormat format, int w, int h, GLuint &glName) {
    GLuint n = 0;
    glGenTextures(1, &n);
    if(!n) {
        m_logManager->logErr("(RenderManager) Error creating GL texture");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, n);

    int psz = 0;
    GLint glformat;
    switch(format) {
        case IMAGE_R:
            psz = 1;
            glformat = GL_ALPHA;
        break;
        case IMAGE_RGB:
            psz = 3;
            glformat = GL_RGB;
        case IMAGE_RGBA:
            psz = 4;
            glformat = GL_RGBA;
        default:
            m_logManager->logErr("(RenderManager) Unknown image format specified");
            glDeleteTextures(1, &n);
            return false;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, glformat, w, h, 0, glformat, GL_UNSIGNED_BYTE, data);
    
    if(useMipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }


    int memSize = w*h*psz;
    
    
    m_memoryUsed+=memSize;
    m_totalTextures++;

    return true;
}

bool RenderManager::createSampler(bool useMipmaps, TextureFiltering filtering, 
                                                            GLuint &smaplerName) {

    GLuint sampler = 0;
    glGenSamplers(1, &sampler); 
    if(!sampler) {
        m_logManager->logErr("(RenderManager) Error creating GL sampler object");
        return false;
    }


    GLint minFilter, magFilter;

    if(filtering == TEX_FILTER_NEAREST) {
        magFilter = GL_NEAREST;
        if(useMipmaps) {
            minFilter = GL_NEAREST_MIPMAP_NEAREST;
        } else {
            minFilter = GL_NEAREST;
        }
    } else {
       magFilter = GL_NEAREST; 
        if(useMipmaps) {
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        } else {
            minFilter = GL_LINEAR;
        }
    }

    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, magFilter);
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, minFilter);

    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return true;
}

void RenderManager::setupGL() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f,0.1f,0.1f,1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
}

void RenderManager::render() {
    using namespace std::chrono;
    milliseconds cur, last = duration_cast<milliseconds>(
                        system_clock::now().time_since_epoch());
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    SDL_GL_SwapWindow(m_winManager->getSDLWindow());
    cur = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    int t = cur.count()-last.count();
    m_totalFrames++;
    m_averageFrameTime = m_averageFrameTime*(1-1.f/m_totalFrames)+(float)t/(m_totalFrames);
}

void RenderManager::destroy() {

}

void RenderManager::logStats() {
    m_logManager->logInfo("(RenderManager) STATS:");
    m_logManager->logInfo("(RenderManager) Total draw calls: "+std::to_string(m_totalDrawCalls));
    m_logManager->logInfo("(RenderManager) Total GL textures: "+std::to_string(m_totalTextures));
    m_logManager->logInfo("(RenderManager) Total shaders: "+std::to_string(m_totalShaders));
    m_logManager->logInfo("(RenderManager) Total meshes: "+std::to_string(m_totalMeshes));
    m_logManager->logInfo("(RenderManager) Average render time: "+std::to_string(m_averageFrameTime)+"(ms)");
    m_logManager->logInfo("(RenderManager) Memory used: "+std::to_string((float)m_memoryUsed/(2<<20))+"MB");
}

} // namespace splitspace
