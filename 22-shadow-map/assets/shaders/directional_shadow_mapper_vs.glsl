#version 330 
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec3 normal;

uniform mat4 directionalLightTransform;
uniform mat4 m;

void main() {
  gl_Position = directionalLightTransform * m * vec4(position, 1.0);
}