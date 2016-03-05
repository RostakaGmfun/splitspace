#include <splitspace/RenderManager.hpp>
#include <splitspace/Engine.hpp>
#include <splitspace/WindowManager.hpp>
#include <splitspace/LogManager.hpp>
#include <splitspace/Scene.hpp>
#include <splitspace/Object.hpp>

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
    
bool RenderManager::createTexture(const void *data, ImageFormat format, int w, int h, GLuint &glName) {
    glName = 0;
    glGenTextures(1, &glName);
    if(!glName) {
        m_logManager->logErr("(RenderManager) Error creating GL texture");
        return false;
    }

    glBindTexture(GL_TEXTURE_2D, glName);

    GLint glformat;
    switch(format) {
        case IMAGE_R:
            glformat = GL_ALPHA;
        break;
        case IMAGE_RGB:
            glformat = GL_RGB;
        break;
            glformat = GL_RGBA;
        break;
        default:
            m_logManager->logErr("(RenderManager) Unknown image format specified");
            destroyTexture(glName);
            glBindTexture(GL_TEXTURE_2D, 0);
            return false;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, glformat, w, h, 0, glformat, GL_UNSIGNED_BYTE, data);
    
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    m_memoryUsed+=getTextureSize(glName);
    m_totalTextures++;
    return true;
}

std::size_t RenderManager::getTextureSize(const GLuint texId) {
    if(!glIsTexture(texId)) {
        return 0;
    }
  
    glBindTexture(GL_TEXTURE_2D, texId);

    GLint tw = 0, th = 0, tf = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tw);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &tf);
    
    int depth = 0;

    switch(tf) {
        case GL_ALPHA:
            depth = 1;
        break;
        case GL_RGB:
            depth = 3;
        break;
        case GL_RGBA:
            depth = 4;
        break;
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    return tw*th*depth;

}
    
void RenderManager::destroyTexture(GLuint &texId) {
    if(glIsTexture(texId)) {
        glDeleteTextures(1, &texId);
        texId = 0;
    } else {
        m_logManager->logWarn("(RenderManager) Trying to destroy GL object which does not appear to be of Texture type");
    }
}

bool RenderManager::createSampler(bool useMipmaps, TextureFiltering filtering, 
                                                            GLuint &samplerName) {

    samplerName = 0;
    glGenSamplers(1, &samplerName); 
    if(!samplerName) {
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

    glSamplerParameteri(samplerName, GL_TEXTURE_MAG_FILTER, magFilter);
    glSamplerParameteri(samplerName, GL_TEXTURE_MIN_FILTER, minFilter);

    glSamplerParameteri(samplerName, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glSamplerParameteri(samplerName, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return true;
}

void RenderManager::destroySampler(GLuint &sampler) {
    if(glIsTexture(sampler)) {
        glDeleteTextures(1, &sampler);
        sampler = 0;
    } else {
        m_logManager->logWarn("(RenderManager) Trying to destroy GL object which does not appear to be of Sampler type");
    }
}

bool RenderManager::createMesh(const void *vData, VertexFormat format, int numVerts, GLuint &vboName, GLuint &vaoName) {
    if(!vData || numVerts<=0) {
        return false;
    }

    if(!createVAOAndVBO(vaoName, vboName)) {
        destroyVAOAndVBO(vaoName, vboName);
        return false;
    }

    glBindVertexArray(vaoName);
    glBindBuffer(GL_ARRAY_BUFFER, vboName);
    GLint bsize = 0;
    switch(format) {
        case VERTEX_3DT:
            glBufferData(GL_ARRAY_BUFFER, numVerts*sizeof(Vertex3DT),
                         vData, GL_STATIC_DRAW);
        break;
        case VERTEX_3DN:
            glBufferData(GL_ARRAY_BUFFER, numVerts*sizeof(Vertex3DN),
                         vData, GL_STATIC_DRAW);
        break;
        case VERTEX_3DTN:
            glBufferData(GL_ARRAY_BUFFER, numVerts*sizeof(Vertex3DTN),
                         vData, GL_STATIC_DRAW);
        break;
        default:
            m_logManager->logErr("(RenderManager) Wrong vertex format specified");
            destroyVAOAndVBO(vaoName, vboName);
            return false;
        break;

    }

    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
    m_memoryUsed+=bsize;
    m_totalMeshes++;

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

void RenderManager::destroyMesh(GLuint &vao, GLuint &vbo) {
    if(!glIsBuffer(vbo)) {
        return;
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    GLint bsize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
    m_memoryUsed-=bsize;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    destroyVAOAndVBO(vao, vbo);
}

bool RenderManager::createVAOAndVBO(GLuint &vao, GLuint &vbo) {
    glGenBuffers(1, &vbo);
    if(!vbo) {
        m_logManager->logErr("(RenderManager) Failed to create VBO");
        return false;
    }

    glGenVertexArrays(1, &vao);
    if(!vao) {
        m_logManager->logErr("(RenderManager) Failed to create VAO");
        return false;
    }

    return true;
}

void RenderManager::destroyVAOAndVBO(GLuint &vao, GLuint &vbo) {
    if(glIsBuffer(vbo)) {
        glDeleteBuffers(1, &vbo);
        vbo = 0;
    } else {
        m_logManager->logWarn("(RenderManager) Trying to destroy GL object which does not appear to be of Buffer type");
    }

    if(glIsVertexArray(vbo)) {
        glDeleteVertexArrays(1, &vao);
        vao = 0;
    } else {
        m_logManager->logWarn("(RenderManager) Trying to destroy GL object which does not appear to be of Vertex Array type");
    }
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
    beginFrame();
    renderScene();
    endFrame();
    cur = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    int t = cur.count()-last.count();
    m_totalFrames++;
    m_averageFrameTime = m_averageFrameTime*(1-1.f/m_totalFrames)+(float)t/(m_totalFrames);
}

void RenderManager::beginFrame() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void RenderManager::renderScene() {
    if(!m_scene)
        return;
    
    auto &renderMap = m_scene->getRenderMap();
    for(auto it : renderMap) {
        if(it.first) {
            setupMaterial(it.first);
        }

        for(auto o : it.second) {
            setupMesh(o->getMesh());
            drawCall();
        }
    }
}

void RenderManager::setupMaterial(const Material *m) {

}

void RenderManager::setupMesh(const Mesh *m) {

}

void RenderManager::drawCall() {

}

void RenderManager::endFrame() {
    SDL_GL_SwapWindow(m_winManager->getSDLWindow());
}

void RenderManager::destroy() {

}

void RenderManager::logStats() {
    m_logManager->logInfo("(RenderManager) STATS:");
    m_logManager->logInfo("\t Total draw calls: "+std::to_string(m_totalDrawCalls));
    m_logManager->logInfo("\t Total GL textures: "+std::to_string(m_totalTextures));
    m_logManager->logInfo("\t Total shaders: "+std::to_string(m_totalShaders));
    m_logManager->logInfo("\t Total meshes: "+std::to_string(m_totalMeshes));
    m_logManager->logInfo("\t Average render time: "+std::to_string(m_averageFrameTime)+"(ms)");
    m_logManager->logInfo("\t Memory used: "+std::to_string((float)m_memoryUsed/(2<<20))+"MB");
}

} // namespace splitspace
