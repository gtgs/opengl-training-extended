#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <memory.h>
#include <vector>
#include "vertex.h"
#define WIDTH 800
#define HEIGHT 600
GLuint compileProgram(GLuint vertexShaderId, GLuint fragmentShaderId);
GLuint loadShaderFromFile(const char* shader, GLuint type);
GLuint loadTexture(const char* filename, int width, int height, GLint bitsPerPixel);
GLuint loadVertexDataFromFile(const char* filename, std::vector<vertex> &data, GLuint numberOfVertices);

GLint y = 0;
GLint x = 0;
GLuint polygonMode = GL_FILL;
int main() {
	if(!glfwInit()) {
		fprintf(stderr, "problem with starting glfw");
		return EXIT_FAILURE;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window;
	window = glfwCreateWindow( WIDTH, HEIGHT, "Tutorial 14-lighting", NULL, NULL);
	if( NULL == window){
	    fprintf( stderr, "Failed to open a window\n" );
	    glfwTerminate();
	    return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	glewInit();

	GLuint va;
	glGenVertexArrays(1, &va);
	glBindVertexArray(va);

	std::vector<vertex> vertices;
	std::vector<GLuint> indices;


	if(EXIT_FAILURE == loadVertexDataFromFile("./assets/models/cube.vertices", vertices, 36)){
		fprintf( stderr, "Failed to load position information\n" );
		glfwTerminate();
		return EXIT_FAILURE;
	}

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	/** set the data pointer **/
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), &vertices[0], GL_STATIC_DRAW);

	GLuint vertexShader = loadShaderFromFile("./assets/shaders/m_v_p_uv_light_vs.glsl",GL_VERTEX_SHADER);
	if(!vertexShader){
		fprintf(stderr, "cannot load vertex shader");
		return EXIT_FAILURE;
	}

	GLuint fragmentShader = loadShaderFromFile("./assets/shaders/m_v_p_uv_light_fs.glsl",GL_FRAGMENT_SHADER);
	if(!vertexShader){
		fprintf(stderr, "cannot load fragment shader");
		return EXIT_FAILURE;
	}

	GLuint programId = compileProgram(vertexShader, fragmentShader);
	if(!programId) {
		fprintf(stderr, "error in generating shaders");
		return EXIT_FAILURE;

	}
	int width = 0;
	int height = 0;
	int bpp = 0;
	GLuint map = loadTexture("./assets/textures/cubemap.png", width, height, bpp);
	if(map== 0){
		fprintf(stderr, "could not load texture");
		return EXIT_FAILURE;
	}


	glm::mat4 identity = glm::mat4(1.0f);


	glm::mat4 model = identity;
//	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 0.0f));
//	glm::mat4 model = identity * translation;


	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);


	do{
		if(glfwGetKey(window, GLFW_KEY_UP)) {
			y++;
		}
		if(glfwGetKey(window, GLFW_KEY_DOWN)) {
			y--;
		}
		if(glfwGetKey(window, GLFW_KEY_LEFT)) {
			x--;
		}
		if(glfwGetKey(window, GLFW_KEY_RIGHT)) {
			x++;
		}
		if(glfwGetKey(window, GLFW_KEY_SPACE)){
			if(GL_FILL == polygonMode){
				polygonMode = GL_LINE;
			} else if(GL_LINE == polygonMode) {
				polygonMode = GL_POINT;
			} else {
				polygonMode = GL_FILL;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
		glUseProgram(programId);
	    glEnableVertexAttribArray(0);
	    glEnableVertexAttribArray(1);
	    glEnableVertexAttribArray(2);
	    glEnableVertexAttribArray(3);

	    glm::mat4 view = glm::lookAt(
			glm::vec3(x, y, 10),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		glm::mat4 projection = glm::perspective(glm::radians(60.0f), WIDTH * 1.0f/HEIGHT, 0.1f, 1000.0f);

	    GLuint vs_m = glGetUniformLocation(programId, "m");
	    GLuint vs_v = glGetUniformLocation(programId, "v");
	    GLuint vs_p = glGetUniformLocation(programId, "p");

	    glUniformMatrix4fv(vs_m, 1, GL_FALSE, &model[0][0]);
	    glUniformMatrix4fv(vs_v, 1, GL_FALSE, &view[0][0]);
	    glUniformMatrix4fv(vs_p, 1, GL_FALSE, &projection[0][0]);


	    glm::vec3 ambientColor(0.8f, 0.8f, 0.8f) ;
	    GLfloat ambientStrength = 1.0f;

	    GLuint fs_ambientColor = glGetUniformLocation(programId, "ambientColor");
	    GLuint fs_ambientStrength = glGetUniformLocation(programId, "ambientStrength");

	    glUniform3f(fs_ambientColor, ambientColor.r, ambientColor.g, ambientColor.b);
	    glUniform1f(fs_ambientStrength, ambientStrength);



	    glm::vec3 directionalLightPosition(7.5, 20, 5);
		glm::vec3 directionalLightColor(0.5f, 0.0f, 0.0f);
		GLfloat directionalLightStrength = 1.0f;

	    GLuint fs_directionalLightPosition = glGetUniformLocation(programId, "directionalLightPosition");
	    GLuint fs_directionalLightColor= glGetUniformLocation(programId, "directionalLightColor");
	    GLuint fs_directionalLightStrength = glGetUniformLocation(programId, "directionalLightStrength");

	    glUniform3f(fs_directionalLightPosition, directionalLightPosition.x, directionalLightPosition.y, directionalLightPosition.z);
	    glUniform3f(fs_directionalLightColor, directionalLightColor.x, directionalLightColor.y, directionalLightColor.z);
	    glUniform1f(fs_directionalLightStrength, directionalLightStrength);


	    /** set the attrib pointer inside the data **/
	    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0);
	    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(glm::vec3)));
	    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec3)));
	    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec3) + sizeof(glm::vec2)));

	    glBindTexture(GL_TEXTURE_2D, map);

	    glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

	    glDisableVertexAttribArray(0);
	    glDisableVertexAttribArray(1);
	    glDisableVertexAttribArray(2);
	    glDisableVertexAttribArray(3);

	    glfwSwapBuffers(window);
	    glfwPollEvents();
	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

}




GLuint loadTexture(const char* filename, int width, int height, GLint bitsPerPixel) {
	fprintf(stderr, "Loading texture from %s\n", filename);
	GLuint textureId;
	glGenTextures(1, &textureId);
	if(textureId == 0) {
			fprintf(stderr, "cannot generate texture");
			return EXIT_FAILURE;
	}
	glBindTexture(GL_TEXTURE_2D, textureId);
	stbi_set_flip_vertically_on_load(true);
	unsigned char* imageData = stbi_load(filename, &width, &height, &bitsPerPixel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(imageData);
	return textureId;
}

GLuint loadShaderFromFile(const char* shader, GLuint type) {
	GLuint shaderId = glCreateShader(type);
	FILE * fp = fopen(shader, "r");
	if(NULL == fp){
		fprintf(stderr, "cannot read shader file - %s", shader);
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	unsigned long length = ftell(fp);
	rewind(fp);
	char* shaderSource = (char*) malloc (sizeof(char) * (length + 1));
	memset(shaderSource, 0, sizeof(char) * (length + 1));
	fread(shaderSource, length, 1, fp);

	fprintf(stderr, "\n shader source is \n--%s (%lu bytes)--\n%s\n--END-- ", shader, length, shaderSource);

	/** compile the shader **/
	glShaderSource(shaderId, 1, &shaderSource, NULL);
	GLint compilationResult = false;
	glCompileShader(shaderId);
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compilationResult);
	if(!compilationResult) {
		GLint infoLogLength = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0) {
			char * msg = (char*) malloc(sizeof(char) * (infoLogLength + 1));
			glGetShaderInfoLog(shaderId, infoLogLength, 0, msg);
			fprintf(stderr, "\n there was an error compiling the shader - %s", msg);
			return 0;
		}
	}
	return shaderId;
}

GLuint compileProgram(GLuint vertexShaderId, GLuint fragmentShaderId) {
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	/** link the program **/
	GLint linkStatus = false;
	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &linkStatus);
	if(!linkStatus){
		GLint infoLogLength = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		if(infoLogLength > 0) {
			char * msg = ((char*) malloc(sizeof(char) * (infoLogLength + 1)));
			glGetProgramInfoLog(programId, infoLogLength, 0, msg);
			fprintf(stderr, "there was an error linking the shader - %s", msg);
			return 0;
		}
	}
	return programId;
}
GLuint loadVertexDataFromFile(const char* filename, std::vector<vertex> &data, GLuint numberOfVertices){
	FILE * fp = fopen(filename, "r");
	if(NULL == fp){
		fprintf(stderr, "cannot read file - %s\n", filename);
		return EXIT_FAILURE;
	}
	GLuint index = 0;
	fprintf(stderr, "reading file - %s\n", filename);
	while(index < (numberOfVertices)){
		vertex entry;
		fscanf(fp, "%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f", &entry.position.x, &entry.position.y, &entry.position.z, &entry.color.r, &entry.color.g, &entry.color.b, &entry.uv.s, &entry.uv.t, &entry.normal.x, &entry.normal.y, &entry.normal.z);
		fprintf(stderr, "v%d: %f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f\n", index, entry.position.x, entry.position.y, entry.position.z, entry.color.r, entry.color.g, entry.color.b, entry.uv.s, entry.uv.t, entry.normal.x, entry.normal.y, entry.normal.z);
		data.push_back(entry);
		index++;
	}
	fclose(fp);
	return EXIT_SUCCESS;
}

