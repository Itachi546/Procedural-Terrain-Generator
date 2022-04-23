#version 450

/***********************************************************************************************************************************************************/
#extension GL_ARB_shader_draw_parameters : enable
/***********************************************************************************************************************************************************/

#define rot(ang) mat2(cos(ang), sin(ang), -sin(ang), cos(ang))

// Structs
struct TerrainData
{
  vec2 translate;
  vec2 scale;
  vec2 id;
};

// Attributes

layout(location = 0) in vec2 position;

// Uniforms

layout(std140, binding = 0) uniform PerFrameData {

    mat4 projection;
    mat4 view;
    mat4 VP;
};

// Transform Buffer
layout(std430, binding = 1) restrict readonly buffer Matrices
{
  TerrainData in_TerrainData[];
};

// Outgoing
layout(location = 0) out flat int id;
layout(location = 1) out flat int clipLevel;

/***********************************************************************************************************************************************************/


void main()
{
    TerrainData	terrainData	= in_TerrainData[gl_BaseInstanceARB + gl_InstanceID];

    mat2 rotate = rot(terrainData.id.y);
    vec2 worldPosition = rotate * (position * terrainData.scale) + terrainData.translate;
    gl_Position = VP * vec4(worldPosition.x, 0.0f, worldPosition.y, 1.0f);

    id =        int(terrainData.id.x);
    clipLevel = int(terrainData.id.y);
}