#version 330 core
out vec4 diffuse;

uniform sampler2D map;

in vec3 _color;
in vec2 _uv;

/* ambient light **/
uniform vec3 ambientColor;
uniform float ambientStrength;

void main() {
	diffuse =  texture(map, _uv) * vec4(ambientColor.rgb, 1.0) * ambientStrength;
}
