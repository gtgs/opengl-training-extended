#version 330 core
out vec4 color;
uniform sampler2D map;
uniform sampler2D directionalShadowMap;

in vec2 _uv;

void main() {
	float depthValue = texture(directionalShadowMap, _uv).r;
	color = vec4(vec3(depthValue), 1.0);
}
