#version 450

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 worldPos;
layout(location = 1) in flat int id;
layout(location = 2) in flat int clipLevel;
layout(location = 3) in float morphFactor;


layout(binding = 0) uniform sampler2D u_Heightmap;
layout(binding = 1) uniform sampler2D u_GradientMap;

uniform int u_VertexCount;
uniform float u_TextureDims;
uniform float u_MaxHeight;

layout(std140, binding = 0) uniform PerFrameData {

    mat4 projection;
    mat4 view;
    mat4 VP;
    vec3 cameraPosition;
    float _unused;
};

vec3 getColorById(int id)
{
  if(id < 0.5f)
    return vec3(0.5, 0.7, 1.0f);
  else if(id < 3.5f)
    return vec3(0.0f, 1.0f, 0.0f);
  else if(id < 4.5)
    return vec3(0.0f, 0.0f, 1.0f);
  else
    return vec3(1.0f, 0.0f, 0.0f);
}

vec3 getColorByLevel(int level)
{
 if(level % 2 == 0)
   return vec3(0.5, 0.7, 1.0f);
 else
  return vec3(1.0f, 0.0f, 0.0f);
}

float getHeight(vec2 uv)
{
   return texture2D(u_Heightmap, (uv + u_TextureDims * 0.5f) / u_TextureDims).r * u_MaxHeight;
}

vec3 grassColor = vec3(0.01f, 0.5f, 0.01f);
vec3 rockColor = vec3(0.97f, 0.66f, 0.37f);
vec3 calculateColor(vec3 n, float h)
{
    float slope	= 1.0f - n.y;
    float factor = smoothstep(0.1, 0.3, slope);
    vec3 col = mix(grassColor, rockColor, factor);

    float h01 = (h / u_MaxHeight) * 0.8f;
    vec3 heightColor = texture2D(u_GradientMap, vec2(0.0f, h01)).rgb;
	return mix(col, heightColor, smoothstep(0.2, 0.3, h01));
}


vec3 getNormalFromTexture(vec2 worldPos)
{
  vec2 offset = vec2(1.0f, 0.0f);
  float h0 = getHeight(worldPos - offset);
  float h1 = getHeight(worldPos - offset.yx);
  float h2 = getHeight(worldPos + offset);
  float h3 = getHeight(worldPos + offset.yx);

  return vec3(h2 - h0, offset.x * 2.0f, h3 - h1);
}

const int viewMode = 0;
const float	fogDensity = 0.001f;
const float	fogGradient	= 1.5f;
const vec3 ld =	normalize(vec3(0.0f, 1.0f, -1.0f));
void main()
{
   vec3 normal = normalize(getNormalFromTexture(worldPos.xz));
   vec3 col = vec3(0.0f);

   if(viewMode == 0)
   {   
      float f = smoothstep(abs(normal.y), 0.3f, 0.5f);
      vec3 albedo = calculateColor(normal, worldPos.y); 
      col += max(dot(normal, ld), 0.0f) * albedo * 2.0f;
      col += (normal.y * 0.5 + 0.5f) * vec3(0.16, 0.20, 0.28);
      float d = length(worldPos - cameraPosition);
      float fog = clamp(exp(-pow(d * fogDensity, fogGradient)), 0.0, 1.0);
      col = mix(vec3(0.5, 0.7, 1.0),col, fog);

//	  col =	pow(col, vec3(0.4545));
      col = col / (1.0 + col);

   }
   if(viewMode == 1)
     col = normal;
   else if(viewMode == 2)
     col = mix(vec3(1.0f), vec3(0.5, 0.7,1.0f), morphFactor);
   col *= vec3(0.9f, 0.9f, 1.0f);
   fragColor = vec4(col, 1.0f);
}