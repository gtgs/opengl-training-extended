#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 color;
layout(location=2) in vec2 uv;

uniform mat4 mvp;

out vec3 _color;
out vec2 _uv;

void main() {
	gl_Position = mvp * vec4(position, 1);
	_color = color;
	_uv = uv;
}
