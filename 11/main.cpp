#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"
#include <memory.h>
#define WIDTH 800
#define HEIGHT 600
GLuint compileProgram(GLuint vertexShaderId, GLuint fragmentShaderId);
GLuint loadShaderFromFile(const char* shader, GLuint type);
GLuint loadTexture(const char* filename, int width, int height, GLint bitsPerPixel);
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
	window = glfwCreateWindow( WIDTH, HEIGHT, "Render a triangle", NULL, NULL);
	if( NULL == window){
	    fprintf( stderr, "Failed to open a window\n" );
	    glfwTerminate();
	    return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

	glewInit();

	/** create a VAO **/
	GLuint va;
	glGenVertexArrays(1, &va);
	glBindVertexArray(va);

	GLfloat vertices[] = {
	    -1.0f,-1.0f,-1.0f,		/** left upper**/
	    -1.0f,-1.0f, 1.0f,
	    -1.0f, 1.0f, 1.0f,

	    1.0f, 1.0f,-1.0f,
	    -1.0f,-1.0f,-1.0f,
	    -1.0f, 1.0f,-1.0f,

	    1.0f,-1.0f, 1.0f,
	    -1.0f,-1.0f,-1.0f,
	    1.0f,-1.0f,-1.0f,

		1.0f, 1.0f,-1.0f,
	    1.0f,-1.0f,-1.0f,
	    -1.0f,-1.0f,-1.0f,

		-1.0f,-1.0f,-1.0f,
	    -1.0f, 1.0f, 1.0f,
	    -1.0f, 1.0f,-1.0f,

		1.0f,-1.0f, 1.0f,
	    -1.0f,-1.0f, 1.0f,
	    -1.0f,-1.0f,-1.0f,

		-1.0f, 1.0f, 1.0f,
	    -1.0f,-1.0f, 1.0f,
	    1.0f,-1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
	    1.0f,-1.0f,-1.0f,
	    1.0f, 1.0f,-1.0f,

		1.0f,-1.0f,-1.0f,
	    1.0f, 1.0f, 1.0f,
	    1.0f,-1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
	    1.0f, 1.0f,-1.0f,
	    -1.0f, 1.0f,-1.0f,

		1.0f, 1.0f, 1.0f,
	    -1.0f, 1.0f,-1.0f,
	    -1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f,
	    -1.0f, 1.0f, 1.0f,
	    1.0f,-1.0f, 1.0f
	};

	GLfloat colors[] = {
	    0.583f,  0.771f,  0.014f,
	    0.609f,  0.115f,  0.436f,
	    0.327f,  0.483f,  0.844f,

	    0.822f,  0.569f,  0.201f,
	    0.435f,  0.602f,  0.223f,
	    0.310f,  0.747f,  0.185f,

	    0.597f,  0.770f,  0.761f,
	    0.559f,  0.436f,  0.730f,
	    0.359f,  0.583f,  0.152f,
	    0.483f,  0.596f,  0.789f,
	    0.559f,  0.861f,  0.639f,
	    0.195f,  0.548f,  0.859f,
	    0.014f,  0.184f,  0.576f,
	    0.771f,  0.328f,  0.970f,
	    0.406f,  0.615f,  0.116f,
	    0.676f,  0.977f,  0.133f,
	    0.971f,  0.572f,  0.833f,
	    0.140f,  0.616f,  0.489f,
	    0.997f,  0.513f,  0.064f,
	    0.945f,  0.719f,  0.592f,
	    0.543f,  0.021f,  0.978f,
	    0.279f,  0.317f,  0.505f,
	    0.167f,  0.620f,  0.077f,
	    0.347f,  0.857f,  0.137f,
	    0.055f,  0.953f,  0.042f,
	    0.714f,  0.505f,  0.345f,
	    0.783f,  0.290f,  0.734f,
	    0.722f,  0.645f,  0.174f,
	    0.302f,  0.455f,  0.848f,
	    0.225f,  0.587f,  0.040f,
	    0.517f,  0.713f,  0.338f,
	    0.053f,  0.959f,  0.120f,
	    0.393f,  0.621f,  0.362f,
	    0.673f,  0.211f,  0.457f,
	    0.820f,  0.883f,  0.371f,
	    0.982f,  0.099f,  0.879f
	};

	GLfloat uv[] = {
			0.000059f, 1.0f-0.000004f,
			0.000103f, 1.0f-0.336048f,
			0.335973f, 1.0f-0.335903f,
			1.000023f, 1.0f-0.000013f,
			0.667979f, 1.0f-0.335851f,
			0.999958f, 1.0f-0.336064f,
			0.667979f, 1.0f-0.335851f,
			0.336024f, 1.0f-0.671877f,
			0.667969f, 1.0f-0.671889f,
			1.000023f, 1.0f-0.000013f,
			0.668104f, 1.0f-0.000013f,
			0.667979f, 1.0f-0.335851f,
			0.000059f, 1.0f-0.000004f,
			0.335973f, 1.0f-0.335903f,
			0.336098f, 1.0f-0.000071f,
			0.667979f, 1.0f-0.335851f,
			0.335973f, 1.0f-0.335903f,
			0.336024f, 1.0f-0.671877f,
			1.000004f, 1.0f-0.671847f,
			0.999958f, 1.0f-0.336064f,
			0.667979f, 1.0f-0.335851f,
			0.668104f, 1.0f-0.000013f,
			0.335973f, 1.0f-0.335903f,
			0.667979f, 1.0f-0.335851f,
			0.335973f, 1.0f-0.335903f,
			0.668104f, 1.0f-0.000013f,
			0.336098f, 1.0f-0.000071f,
			0.000103f, 1.0f-0.336048f,
			0.000004f, 1.0f-0.671870f,
			0.336024f, 1.0f-0.671877f,
			0.000103f, 1.0f-0.336048f,
			0.336024f, 1.0f-0.671877f,
			0.335973f, 1.0f-0.335903f,
			0.667969f, 1.0f-0.671889f,
			1.000004f, 1.0f-0.671847f,
			0.667979f, 1.0f-0.335851f
	};

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	/** set the data pointer **/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint colorBuffer;
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	/** set the data pointer for color **/
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);


	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);


	GLuint vertexShader = loadShaderFromFile("./vs.glsl",GL_VERTEX_SHADER);
	if(!vertexShader){
		fprintf(stderr, "cannot load vertex shader");
		return EXIT_FAILURE;
	}

	GLuint fragmentShader = loadShaderFromFile("./fs.glsl",GL_FRAGMENT_SHADER);
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
	GLuint textureBrick = loadTexture("/home/girishsarwal/eclipse-workspace/OpenGL/Debug/assets/brick.png", width, height, bpp);
	if(textureBrick == 0){
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

	    glm::mat4 camera = glm::lookAt(
			glm::vec3(x, y, 10),
			glm::vec3(0, 0, 0),
			glm::vec3(0, 1, 0)
		);

		glm::mat4 projection = glm::perspective(glm::radians(60.0f), WIDTH * 1.0f/HEIGHT, 0.1f, 1000.0f);

		glm::mat4 mvp = projection * camera * model;


	    GLuint vs_mvp = glGetUniformLocation(programId, "mvp");
	    glUniformMatrix4fv(vs_mvp, 1, GL_FALSE, &mvp[0][0]);


	    /** set the attrib pointer inside the data **/
	    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	    glBindTexture(GL_TEXTURE_2D, textureBrick);

		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);

	    glDisableVertexAttribArray(0);
	    glDisableVertexAttribArray(1);
	    glDisableVertexAttribArray(2);

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
	unsigned char* imageData = stbi_load(filename, &width, &height, &bitsPerPixel, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
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
