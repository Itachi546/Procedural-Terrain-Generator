#version 450

layout(location = 0) out vec4 fragColor;

layout(location = 0) in flat int id;
layout(location = 1) in flat int clipLevel;


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
  return	vec3(1.0f, 0.0f, 0.0f);
}

void main()
{
   fragColor = vec4(1.0f);
}