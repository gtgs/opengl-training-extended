#version 330 core
out vec4 finalColor;

uniform mat4 m;
uniform sampler2D map;
uniform sampler2D directionalShadowMap;

in vec3 _color;
in vec2 _uv;
in vec3 _normal;

in vec3 _fragmentPositionInWorld;
in vec4 _fragmentPositionInLightSpace;

/** ambient light **/
uniform vec3 ambientColor;
uniform float ambientStrength;

/* directional light **/
uniform vec3 directionalLightPosition;
uniform vec3 directionalLightColor;
uniform float directionalLightStrength;

void main() {
	/** 1. calculate the ambient component */
	vec3 ambient = ambientColor.rgb * ambientStrength;

	/** 2. calculate the diffuse component */
	/** normalize the normal vector**/
	mat3 normalMatrix = transpose(inverse(mat3(m)));
	vec3 n = normalize(normalMatrix * _normal);
	/** calculate the light direction **/
	vec3 lightDirection = normalize(directionalLightPosition - _fragmentPositionInWorld);
	/** use a dot product to get the cosine, this will decide what angle the light is falling at **/
	float diffuseCoefficient = max(dot(n, lightDirection), 0.0);
	vec3 diffuse = diffuseCoefficient * directionalLightStrength * directionalLightColor;

	/** 3. calculate specular */
	vec3 specular = vec3(0.0, 0.0, 0.0);

	/** 4. calculate shadows */
	/** calculate a shadow factor */
	vec3 projectedCoords = _fragmentPositionInLightSpace.xyz / _fragmentPositionInLightSpace.w;
	/** adjust for textre coordinates */
	projectedCoords = projectedCoords * 0.5 + 0.5;
	/** find the depth of the closest object on this viewing angle*/
	float closestDepth = texture(directionalShadowMap, projectedCoords.xy).r;
	/** find the depth of the current fragement on this viewing angle*/
	float currentDepth = projectedCoords.z;
	/** calculate a  shadow factor */
	float bias = 0.005;
	float shadowFactor = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	/** finally, calcualte total lighting*/
	vec3 combined = ambient + ((1.0 - shadowFactor) * (diffuse + specular));

	/** mix lighting with the albedo to get the final color of pixel */
	finalColor = texture(map, _uv) * vec4(combined.rgb, 1.0);
}
