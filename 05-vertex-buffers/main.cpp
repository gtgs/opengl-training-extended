#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define WIDTH 800
#define HEIGHT 600
GLuint compileProgram(GLuint vertexShaderId, GLuint fragmentShaderId);
GLuint loadShaderFromFile(const char* shader, GLuint type);


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
	    -1.0f,-1.0f,-1.0f,
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

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	/** set the data pointer **/
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

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

	glm::mat4 identity = glm::mat4(1.0f);


	glm::mat4 model = identity;
//	glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 0.0f));
//	glm::mat4 model = identity * translation;

	glm::mat4 camera = glm::lookAt(
		glm::vec3(0, 10, 10),
		glm::vec3(0, 0, 0),
		glm::vec3(0, 1, 0)
	);

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), WIDTH * 1.0f/HEIGHT, 0.1f, 1000.0f);

	glm::mat4 mvp = projection * camera * model;

	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(programId);
	    glEnableVertexAttribArray(0);
	    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	    GLuint vs_mvp = glGetUniformLocation(programId, "mvp");
	    glUniformMatrix4fv(vs_mvp, 1, GL_FALSE, &mvp[0][0]);


	    /** set the attrib pointer inside the data **/
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glDrawArrays(GL_TRIANGLES, 0, 12 * 3);
	    glDisableVertexAttribArray(0);
	    glfwSwapBuffers(window);
	    glfwPollEvents();
	}
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );

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
