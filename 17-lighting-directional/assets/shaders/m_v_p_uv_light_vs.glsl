#version 330 core
layout(location=0) in vec3 position;
layout(location=1) in vec3 color;
layout(location=2) in vec2 uv;
layout(location=3) in vec3 normal;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

out vec3 _color;
out vec2 _uv;
out vec3 _normal;

out vec3 _fragmentPositionInWorld;

void main() {
	gl_Position = p * v * m * vec4(position, 1);
	_color = color;
	_uv = uv;
	_fragmentPositionInWorld = vec3(m * vec4(position, 1));
	_normal = normal;
}
