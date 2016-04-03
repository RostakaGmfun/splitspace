layout (location=INPUT_VPOS) in vec3 inPosition;
layout (location=INPUT_VTEX) in vec2 inTexcoord;
layout (location=INPUT_VNORM) in vec3 inNormal;

out vec3 Position;
out vec2 Texcoord;
out vec3 Normal;

uniform mat4 MVP;

void main() {
    Normal = inNormal;
    Texcoord = inTexcoord;
    Position = inPosition;
    gl_Position = MVP * vec4(inPosition, 0.0);
}

