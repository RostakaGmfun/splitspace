in vec3 Position;
in vec2 Texcoord;
in vec3 Normal;

out vec4 _OUT0;

uniform sampler2D diffuseMap;
uniform LightInfo lights[MAX_LIGHTS];
uniform int numLights;
uniform int _TECHNIQUE_;
uniform Material material;

void main() {
    _OUT0 = applyMaterial(material, diffuseMap, Texcoord);
    for(int i = 0;i<numLights;i++) {
        _OUT0.xyz+=splitspace_Lighting(_TECHNIQUE_, Position,
                                       Normal, lights[i], material);
    }
}

