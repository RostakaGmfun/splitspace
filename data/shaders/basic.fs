in vec3 Position;
in vec2 Texcoord;
in vec3 Normal;

out vec4 _OUT0;

uniform sampler2D diffuseTexture;
uniform LightInfo lights[MAX_LIGHTS];
uniform int numLights;
uniform int _TECHNIQUE_;

void main() {
    _OUT0 = texture(diffuseTexture, Texcoord);
    for(int i = 0;i<numLights;i++) {
        _OUT0.xyz+=splitspace_Lighting(_TECHNIQUE_, Position, Normal, lights[i]);
    }
}

