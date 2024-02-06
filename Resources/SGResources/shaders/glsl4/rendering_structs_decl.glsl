#sg_ifndef RENDERING_STRUCTS_DECL_GLSL
#sg_define RENDERING_STRUCTS_DECL_GLSL

#define DIRECTIONAL_LIGHTS_MAX_COUNT    5

struct IRenderingComponent
{
    mat4 projectionMatrix; // can be unused
    mat4 viewMatrix; // can be unused
    mat4 spaceMatrix;
    vec3 position;
    vec3 rotation; // can be unused
    vec3 scale; // can be unused
};

struct ILight
{
    IRenderingComponent renderingPart;
    vec4 color;
    float intensity;
    int shadowSamplesCount;
};

struct DirectionalLight
{
    ILight lightPart;
    // todo: make for dir light
};

#sg_endif // RENDERING_STRUCTS_DECL_GLSL