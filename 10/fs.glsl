#version 330 core
out vec4 diffuse;

uniform sampler2D map;

in vec3 _color;
in vec2 _uv;

void main() {

	diffuse = texture(map, _uv);
}
