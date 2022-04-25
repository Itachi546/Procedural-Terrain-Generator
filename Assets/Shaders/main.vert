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

/***********************************************************************************************************************************************************/

// Attributes

layout(location = 0) in vec2 position;


/***********************************************************************************************************************************************************/

// Uniforms

layout(std140, binding = 0) uniform PerFrameData {

    mat4 projection;
    mat4 view;
    mat4 VP;
    vec3 cameraPosition;
    float _unused;
};

// Transform Buffer
layout(std430, binding = 1) restrict readonly buffer Matrices
{
  TerrainData in_TerrainData[];
};

layout(binding = 0) uniform sampler2D u_Heightmap;

/***********************************************************************************************************************************************************/

// Outgoing
layout(location = 0) out vec3 worldPos;
layout(location = 1) out flat int id;
layout(location = 2) out flat int clipLevel;
layout(location = 3) out float morphFactor;

/***********************************************************************************************************************************************************/

float getHeightFromTexture(vec2 uv)
{
   return texture2D(u_Heightmap, (uv + 1024.0f) / 2048.0f).r * 200.0f;
}

float linstep(float low, float high, float value){
  return clamp((value-low)/(high-low), 0.0, 1.0);
}

// The height calculation should be done in such a way that at the edges
// it includes the sample from the next level

float getHeight(vec2 worldPos, float scale, float morphFactor)
{
  // Get the height at current level
  vec2 ownPos = worldPos;
  float ownHeight = getHeightFromTexture(ownPos);

  // Calcualte the offset of vertex for next heigher grid level 
  // This is generally in multiple of scale of next gridSize
  vec2 modPos = mod(worldPos, scale * 2.0f);

  // Check if the offset lies in currentGridLevel or not
  if (length(modPos) > 0.5f)
  {
    vec2 n1 = worldPos + modPos;
    vec2 n2 = worldPos - modPos;
    float h1 = getHeightFromTexture(n1);
    float h2 = getHeightFromTexture(n2);

    float h = (h1 + h2) * 0.5f;
    ownHeight = (1.0f - morphFactor) * ownHeight + morphFactor * h;
  }
  
  return ownHeight;
}


void main()
{
    TerrainData	terrainData	= in_TerrainData[gl_BaseInstanceARB + gl_InstanceID];
    mat2 rotate = rot(terrainData.id.y);
    vec2 worldPosition = rotate * (position * terrainData.scale) + terrainData.translate;

    const float gridSize = 127.0f * terrainData.scale.x;
    const float transitionWidth = gridSize * 0.1f;
    vec2 alpha = (abs(worldPosition - cameraPosition.xz) - (gridSize * 0.5f - transitionWidth - 1.0f)) / transitionWidth;
    alpha = clamp(alpha, 0.0, 1.0);

    morphFactor = max(alpha.x, alpha.y);

    float height = getHeight(worldPosition, terrainData.scale.x, morphFactor);
    gl_Position = VP * vec4(worldPosition.x, height, worldPosition.y, 1.0f);

    id =        int(terrainData.id.x);
    clipLevel = int(terrainData.id.y);
    worldPos = vec3(worldPosition.x, height, worldPosition.y);

}