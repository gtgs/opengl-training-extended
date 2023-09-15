#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <memory.h>
#include <vector>
#include "vertex.h"

GLuint compileProgram(GLuint vertexShaderId, GLuint fragmentShaderId);
GLuint loadShaderFromFile(const char *shader, GLuint type);
GLuint loadTexture(const char *filename, GLuint format, int width, int height, GLint bitsPerPixel);
GLuint loadVertexDataFromFile(const char *filename, std::vector<vertex> &data, GLuint numberOfVertices);
void renderScene(GLuint shaderId);

GLuint windowWidth = 800, windowHeight = 600;
GLuint viewportWidth = 0, viewportHeight = 0;

bool paused = true;
float rotation = 0.0f;

GLuint polygonMode = GL_FILL;
std::vector<vertex> vertices;
std::vector<vertex> verticesGround;
std::vector<vertex> verticesDebugMap;
/** vbo */
GLuint vertexBuffer = 0;
GLuint vertexBufferGround = 0;
/** shader stuff */
GLuint vertexShader = 0;
GLuint fragmentShader = 0;
GLuint programShadowReceiverId = 0;
GLuint vertexShaderDepthMap = 0;
GLuint fragmentShaderDepthMap = 0;
GLuint programDepthMapId = 0;
GLuint vertexShaderShadowMapper = 0;
GLuint fragmentShaderShadowMapper = 0;
GLuint programShadowMapperId = 0;
/** textures */
GLuint mapCube;
GLuint mapBrick = 0;

/** framebuffer and textures */
GLuint framebuffer = 0;
GLuint mapShadow = 0;

/** initial vectors */
glm::vec3 ambientColor(0.8f, 0.8f, 0.8f);
glm::vec3 directionalLightPosition(-2, 8, -1);
glm::vec3 directionalLightColor(0.6f, 0.8f, 0.1f);

glm::vec3 cameraPosition(0, 5, 30);
glm::vec3 cameraTarget(0, 0, 0);
glm::vec3 cameraUpVector(0, 1, 0);

glm::mat4 model = glm::mat4(1.0f);

GLFWwindow *window = nullptr;

int main()
{
	if (!glfwInit())
	{
		fprintf(stderr, "problem with starting glfw");
		return EXIT_FAILURE;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	window = glfwCreateWindow(windowWidth, windowHeight, "Tutorial 16-Shadow Mapping", NULL, NULL);

	if (NULL == window)
	{
		fprintf(stderr, "Failed to open a window\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwGetFramebufferSize(window, (int *)&viewportWidth, (int *)&viewportHeight);
	fprintf(stderr, "framebuffer size is %d x %d", viewportWidth, viewportWidth);

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	glewInit();

	GLuint va;
	glGenVertexArrays(1, &va);
	glBindVertexArray(va);

	if (EXIT_FAILURE == loadVertexDataFromFile("./assets/models/cube.vertices", vertices, 36))
	{
		fprintf(stderr, "Failed to load position information for cube\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	if (EXIT_FAILURE == loadVertexDataFromFile("./assets/models/ground.vertices", verticesGround, 6))
	{
		fprintf(stderr, "Failed to load position information for ground\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &vertexBufferGround);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferGround);
	glBufferData(GL_ARRAY_BUFFER, verticesGround.size() * sizeof(vertex), &verticesGround[0], GL_STATIC_DRAW);

	vertexShader = loadShaderFromFile("./assets/shaders/mvp_uv_normal_directional_light_sr_vs.glsl", GL_VERTEX_SHADER);
	if (!vertexShader)
	{
		fprintf(stderr, "cannot load vertex shader");
		return EXIT_FAILURE;
	}

	fragmentShader = loadShaderFromFile("./assets/shaders/mvp_uv_normal_directional_light_sr_fs.glsl", GL_FRAGMENT_SHADER);
	if (!fragmentShader)
	{
		fprintf(stderr, "cannot load fragment shader");
		return EXIT_FAILURE;
	}
	programShadowReceiverId = compileProgram(vertexShader, fragmentShader);
	if (!programShadowReceiverId)
	{
		fprintf(stderr, "error in generating shaders");
		return EXIT_FAILURE;
	}

	vertexShaderDepthMap = loadShaderFromFile("./assets/shaders/mvp_uv_vs.glsl", GL_VERTEX_SHADER);
	if (!vertexShaderDepthMap)
	{
		fprintf(stderr, "cannot load vertex shader");
		return EXIT_FAILURE;
	}

	fragmentShaderDepthMap = loadShaderFromFile("./assets/shaders/mvp_uv_fs.glsl", GL_FRAGMENT_SHADER);
	if (!fragmentShaderDepthMap)
	{
		fprintf(stderr, "cannot load fragment shader");
		return EXIT_FAILURE;
	}

	programDepthMapId = compileProgram(vertexShaderDepthMap, fragmentShaderDepthMap);

	vertexShaderShadowMapper = loadShaderFromFile("./assets/shaders/directional_shadow_mapper_vs.glsl", GL_VERTEX_SHADER);
	if (!vertexShaderShadowMapper)
	{
		fprintf(stderr, "cannot load vertex shader for shadowMapper");
		return EXIT_FAILURE;
	}

	fragmentShaderShadowMapper = loadShaderFromFile("./assets/shaders/directional_shadow_mapper_fs.glsl", GL_FRAGMENT_SHADER);
	if (!fragmentShaderShadowMapper)
	{
		fprintf(stderr, "cannot load fragment shader for shadowMapper");
		return EXIT_FAILURE;
	}

	programShadowMapperId = compileProgram(vertexShaderShadowMapper, fragmentShaderShadowMapper);
	if (!programShadowMapperId)
	{
		fprintf(stderr, "error in generating shaders programShadowMapper");
		return EXIT_FAILURE;
	}

	int width = 0;
	int height = 0;
	int bpp = 0;
	mapCube = loadTexture("./assets/textures/cubemap.png", GL_RGBA, width, height, bpp);
	if (mapCube == 0)
	{
		fprintf(stderr, "could not load texture");
		return EXIT_FAILURE;
	}

	mapBrick = loadTexture("./assets/textures/brick.png", GL_RGB, width, height, bpp);
	if (mapBrick == 0)
	{
		fprintf(stderr, "could not load texture");
		return EXIT_FAILURE;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glGenTextures(1, &mapShadow);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mapShadow);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewportWidth, viewportHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	/** remove oversampling */
	float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mapShadow, 0);

	do
	{
		glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUpVector);
		glfwGetFramebufferSize(window, (int *)&viewportWidth, (int *)&viewportHeight);
		/** 1st render  pass - generate the shadow map */
		float nearPlane = 0.1f;
		float farPlane = 20.0f;
		glViewport(0, 0, viewportWidth, viewportHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mapShadow, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		glClear(GL_DEPTH_BUFFER_BIT);

		glm::mat4 directionalLightView = glm::lookAt(directionalLightPosition, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 directionalLightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
		glm::mat4 directionalLightTransform = directionalLightProjection * directionalLightView; /** this will be passed to the shadow mapper shader as a uniform */

		glUseProgram(programShadowMapperId);
		GLuint uniformDirectionalLightTransform = glGetUniformLocation(programShadowMapperId, "directionalLightTransform");
		glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, &directionalLightTransform[0][0]);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mapShadow);

		renderScene(programShadowMapperId);

		/** 2nd render pass - normal world rendering */
		glViewport(0, 0, viewportWidth, viewportHeight);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(60.0f), windowWidth * 1.0f / windowHeight, 0.1f, 1000.0f);

		glUseProgram(programShadowReceiverId);
		GLuint uniformView = glGetUniformLocation(programShadowReceiverId, "v");
		GLuint uniformProjection = glGetUniformLocation(programShadowReceiverId, "p");
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, &projection[0][0]);

		GLfloat ambientStrength = 0.6f;
		GLuint uniformAmbientColor = glGetUniformLocation(programShadowReceiverId, "ambientColor");
		GLuint uniformAmbientStrength = glGetUniformLocation(programShadowReceiverId, "ambientStrength");
		glUniform3f(uniformAmbientColor, ambientColor.r, ambientColor.g, ambientColor.b);
		glUniform1f(uniformAmbientStrength, ambientStrength);

		GLfloat directionalLightStrength = 1.5f;
		GLuint uniformDirectionalLightPosition = glGetUniformLocation(programShadowReceiverId, "directionalLightPosition");
		GLuint uniformDirectionalLightColor = glGetUniformLocation(programShadowReceiverId, "directionalLightColor");
		GLuint uniformDirectionalLightStrength = glGetUniformLocation(programShadowReceiverId, "directionalLightStrength");
		glUniform3f(uniformDirectionalLightPosition, directionalLightPosition.x, directionalLightPosition.y, directionalLightPosition.z);
		glUniform3f(uniformDirectionalLightColor, directionalLightColor.r, directionalLightColor.g, directionalLightColor.b);
		glUniform1f(uniformDirectionalLightStrength, directionalLightStrength);

		glUseProgram(programShadowReceiverId);
		uniformDirectionalLightTransform = glGetUniformLocation(programShadowReceiverId, "directionalLightTransform");
		glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, &directionalLightTransform[0][0]);
		renderScene(programShadowReceiverId);

		glfwSwapBuffers(window);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_P))
		{
			paused = !paused;
		}
		if (glfwGetKey(window, GLFW_KEY_W))
		{
			cameraPosition.z -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_S))
		{
			cameraPosition.z += 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_A))
		{
			cameraPosition.x -= 0.1f;
		}
		if (glfwGetKey(window, GLFW_KEY_D))
		{
			cameraPosition.x += 0.1f;
		}

		if (glfwGetKey(window, GLFW_KEY_B))
		{
			directionalLightPosition.x += 0.15f;
		}

		if (glfwGetKey(window, GLFW_KEY_V))
		{
			directionalLightPosition.x -= 0.15f;
		}

		if (glfwGetKey(window, GLFW_KEY_R))
		{
			if (GL_FILL == polygonMode)
			{
				polygonMode = GL_LINE;
			}
			else if (GL_LINE == polygonMode)
			{
				polygonMode = GL_POINT;
			}
			else
			{
				polygonMode = GL_FILL;
			}
		}
		if (!paused)
		{
			rotation += 0.5f;
		}

	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

	glDeleteBuffers(1, &vertexBuffer);
	glDeleteProgram(programShadowReceiverId);
	glDeleteTextures(1, &mapCube);
	glDeleteTextures(1, &mapBrick);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(1, &mapShadow);

	glfwTerminate();
}

void renderScene(GLuint shaderId)
{
	GLuint uniformModel = 0;
	glUseProgram(shaderId);

	/** draw the cube **/
	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(rotation), glm::vec3(0, 1, 0));
	uniformModel = glGetUniformLocation(shaderId, "m");
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, &model[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mapCube);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(glm::vec3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(glm::vec3) + sizeof(glm::vec3)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)));

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	glUseProgram(0);

	/** draw the ground plane */
	glUseProgram(shaderId);
	model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(10.0, 1, 10.0));
	model = glm::translate(model, glm::vec3(0, -5.2f, 0));
	uniformModel = glGetUniformLocation(shaderId, "m");
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, &model[0][0]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferGround);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mapBrick);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mapShadow);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(glm::vec3)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(glm::vec3) + sizeof(glm::vec3)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void *)(sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)));

	GLuint uniformAlbedoMap = glGetUniformLocation(shaderId, "map");
	glUniform1i(uniformAlbedoMap, 0);
	GLuint uniformDirectionalShadowMap = glGetUniformLocation(shaderId, "directionalShadowMap");
	glUniform1i(uniformDirectionalShadowMap, 1);

	glDrawArrays(GL_TRIANGLES, 0, verticesGround.size());

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

	glUseProgram(0);
}

GLuint loadTexture(const char *filename, GLuint format, int width, int height, GLint bitsPerPixel)
{
	fprintf(stderr, "Loading texture from %s\n", filename);
	GLuint textureId;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &textureId);
	if (textureId == 0)
	{
		fprintf(stderr, "cannot generate texture");
		return EXIT_FAILURE;
	}
	glBindTexture(GL_TEXTURE_2D, textureId);
	stbi_set_flip_vertically_on_load(true);
	unsigned char *imageData = stbi_load(filename, &width, &height, &bitsPerPixel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imageData);
	return textureId;
}

GLuint loadShaderFromFile(const char *shader, GLuint type)
{
	GLuint shaderId = glCreateShader(type);
	FILE *fp = fopen(shader, "r");
	if (NULL == fp)
	{
		fprintf(stderr, "cannot read shader file - %s", shader);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	unsigned long length = ftell(fp);
	rewind(fp);
	char *shaderSource = (char *)malloc(sizeof(char) * (length + 1));
	memset(shaderSource, 0, sizeof(char) * (length + 1));
	fread(shaderSource, length, 1, fp);

	fprintf(stderr, "\n shader source is \n--%s (%lu bytes)--\n%s\n--END-- ", shader, length, shaderSource);

	/** compile the shader **/
	glShaderSource(shaderId, 1, &shaderSource, NULL);
	GLint compilationResult = false;
	glCompileShader(shaderId);
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilationResult);
	if (!compilationResult)
	{
		GLint infoLogLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			char *msg = (char *)malloc(sizeof(char) * (infoLogLength + 1));
			glGetShaderInfoLog(shaderId, infoLogLength, 0, msg);
			fprintf(stderr, "\n there was an error compiling the shader - %s", msg);
			return 0;
		}
	}
	return shaderId;
}

GLuint compileProgram(GLuint vertexShaderId, GLuint fragmentShaderId)
{
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	/** link the program **/
	GLint linkStatus = false;
	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
	if (!linkStatus)
	{
		GLint infoLogLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			char *msg = ((char *)malloc(sizeof(char) * (infoLogLength + 1)));
			glGetProgramInfoLog(programId, infoLogLength, 0, msg);
			fprintf(stderr, "there was an error linking the shader - %s", msg);
			return 0;
		}
	}
	fprintf(stderr, "made shader %d from %d, %d", programId, vertexShaderId, fragmentShaderId);
	return programId;
}
GLuint loadVertexDataFromFile(const char *filename, std::vector<vertex> &data, GLuint numberOfVertices)
{
	FILE *fp = fopen(filename, "r");
	if (NULL == fp)
	{
		fprintf(stderr, "cannot read file - %s\n", filename);
		return EXIT_FAILURE;
	}
	GLuint index = 0;
	fprintf(stderr, "reading file - %s\n", filename);
	while (index < (numberOfVertices))
	{
		vertex entry;
		fscanf(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &entry.position.x, &entry.position.y, &entry.position.z, &entry.color.r, &entry.color.g, &entry.color.b, &entry.uv.s, &entry.uv.t, &entry.normal.x, &entry.normal.y, &entry.normal.z);
		fprintf(stderr, "v%d: %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", index, entry.position.x, entry.position.y, entry.position.z, entry.color.r, entry.color.g, entry.color.b, entry.uv.s, entry.uv.t, entry.normal.x, entry.normal.y, entry.normal.z);
		data.push_back(entry);
		index++;
	}
	fclose(fp);
	return EXIT_SUCCESS;
}
