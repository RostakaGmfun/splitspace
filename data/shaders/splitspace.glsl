/*
 * Splitspace suppport file for GLSL shaders
 * Contains built-in engine definitions
 */

/*
 * Rendering techinques
 */

#define RENDER_LAMBERT
#define RENDER_PHONG

struct LightInfo {
    vec3 position;
    vec3 rotation;
    vec3 diffuse;
    vec3 specular;
    float spotLightCutoff;
    float power;
    int type;
};

#define LIGHT_AMBIENT 1
#define LIGHT_SUN     2
#define LIGHT_POINT   3
#define LIGHT_SPOT    4
#define MAX_LIGHTS    8

vec3 lambertLighting(vec3 vertex, vec3 normal, LightInfo light) {
    vec3 L = normalize(light.position-vertex);
    switch(light.type) {
        case LIGHT_AMBIENT:
            return light.diffuse;
        case LIGHT_SUN:
            return clamp(light.diffuse*max(dot(normal, light.rotation), 0.0), 0.0, 1.0);
        case LIGHT_POINT:
            return clamp(light.diffuse*max(dot(normal, L), 0.0), 0.0, 1.0);
        default:
            return vec3(0, 0, 0);
    }
}

vec3 phongLighting(vec3 vertex, vec3 normal, LightInfo light) {
    //TODO
    return vec3(0, 0, 0);
}

vec3 splitspace_Lighting(int technique, vec3 vertex, vec3 normal, LightInfo light) {
    switch(technique) {
        case 1:
            return lambertLighting(vertex, normal, light);
        case 2:
            return phongLighting(vertex, normal, light);
        default:
            return vec3(0, 0, 0);
    }
}

/*
 * Locations for VS inputs
 */
#define INPUT_VPOS  0
#define INPUT_VTEX  1
#define INPUT_VNORM 2

