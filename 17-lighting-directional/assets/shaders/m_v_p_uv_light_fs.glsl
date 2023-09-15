#version 330 core
out vec4 finalColor;

uniform sampler2D map;

in vec3 _color;
in vec2 _uv;
in vec3 _normal;

in vec3 _fragmentPositionInWorld;

/** ambient light **/
uniform vec3 ambientColor;
uniform float ambientStrength;

/* directional light **/
uniform vec3 directionalLightPosition;
uniform vec3 directionalLightColor;
uniform float directionalLightStrength;

void main() {
	/** normalize the normal vector**/
	vec3 n = normalize(_normal);
	/** calculate the light direction **/
	vec3 lightDirection = normalize(directionalLightPosition - _fragmentPositionInWorld);
	/** use a dot product to get the cosine, this will decide what angle the light is falling at **/
	float diffuseCoefficient = max(dot(n, lightDirection), 0.0);

	vec3 diffuse = diffuseCoefficient * directionalLightStrength * directionalLightColor;
	vec3 ambient = ambientColor.rgb * ambientStrength;

	vec3 combined = diffuse + ambient;

	finalColor =  texture(map, _uv) * vec4(combined.rgb, 1.0);
}
