#version 330 core
out vec4 finalColor;

uniform sampler2D map;

in vec3 _color;
in vec2 _uv;
in vec3 _normal;

in vec3 _fragmentPositionInWorld;

uniform vec3 cameraPosition;

/** ambient light **/
uniform vec3 ambientColor;
uniform float ambientStrength;

/* directional light **/
uniform vec3 directionalLightPosition;
uniform vec3 directionalLightColor;
uniform float directionalLightStrength;

/** specular light **/
uniform vec3 spotLightPosition;
uniform vec3 spotLightColor;
uniform float spotLightStrength;
uniform int shininess;

void main() {
	/** normalize the normal vector**/
	vec3 n = normalize(_normal);
	/** calculate the light direction **/
	vec3 lightDirection = normalize(directionalLightPosition - _fragmentPositionInWorld);
	/** use a dot product to get the cosine, this will decide what angle the light is falling at **/
	float diffuseCoefficient = max(dot(n, lightDirection), 0.0);


	/** get the view direction **/
	vec3 viewDirection = normalize(cameraPosition - _fragmentPositionInWorld);
	/** reflect the incoming light direction around the normalized normal **/
	vec3 reflectionDirection = reflect(-lightDirection, n);
	/** find the dot product between view direction and refl, clamp it, and then expo it by shininess**/
	float specCoefficient = pow(max(dot(viewDirection, reflectionDirection), 0.0), shininess);


	vec3 diffuse = diffuseCoefficient * directionalLightStrength * directionalLightColor;
	vec3 ambient = ambientColor.rgb * ambientStrength;
	vec3 specular = specCoefficient * spotLightStrength * spotLightColor;
	vec3 combined = ambient + diffuse + specular;

	finalColor =  texture(map, _uv) * vec4(combined.rgb, 1.0);
}
