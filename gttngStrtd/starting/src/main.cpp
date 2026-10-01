#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

//defines
#define windowWidth 800
#define windowHeight 600
const char *vertexShaderSource =
"#version 150 core\n"
"in vec3 aPos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource =
"#version 150 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

void vertexBuffer(GLuint *vbo);
void processInput(GLFWwindow *window);
unsigned int compileShader(const char* src, GLenum type);
void frameBuffer_size_callback(GLFWwindow *window, int width, int height);
unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc);

GLFWwindow* windowInit( int width, int height);

int main(){
	//create triangle
	float t1[] = {
		-0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
	};
	float t2[]=	{
		-0.6f, -0.8f, 0.0f,
		 0.0f, -0.8f, 0.0f,
		-0.3f,  0.8f, 0.0f}; 

	//create window
	GLFWwindow* window = windowInit( windowWidth, windowHeight);

	//link vertex and fragment
	unsigned int shaderProgram = createShaderProgram( vertexShaderSource, fragmentShaderSource);
	
	//Vertex buffer and array object 
	unsigned int VBO_t1;
	unsigned int VAO_t1;

	unsigned int VBO_t2;
	unsigned int VAO_t2;
	
	int nmbr_Attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nmbr_Attribs);
	std::cout<< "Max vertex Attributes" << nmbr_Attribs << std::endl;
	//Bind
	glGenVertexArrays(1, &VAO_t1);
	glBindVertexArray(VAO_t1);
	glGenBuffers(1, &VBO_t1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_t1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t1), t2, GL_STATIC_DRAW);

	GLuint positionAttribute = glGetAttribLocation(shaderProgram, "aPos");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
	//unbind
	glGenVertexArrays(1, &VAO_t2);
	glBindVertexArray(VAO_t2);

	glGenBuffers(1, &VBO_t2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_t2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t1), t1, GL_STATIC_DRAW);

				
	GLuint positionAttribute2 = glGetAttribLocation(shaderProgram, "aPos");
	glEnableVertexAttribArray(positionAttribute2);
	glVertexAttribPointer(positionAttribute2, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
	glBindVertexArray(0);	
	//render loop
	while(!glfwWindowShouldClose(window)){
		processInput(window);
		glClearColor(0.0f, 0.5f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO_t1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		
		glBindVertexArray(VAO_t2);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//clean up
	glfwTerminate();
	return 0;
}

void vertexBuffer(GLuint *vbo){}

void processInput(GLFWwindow *window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

unsigned int compileShader(const char* src, GLenum type){
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	//check for errors
	int success = 0;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success){
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "shader compilation failed:\n" << infoLog << std::endl;
	}

	return shader;
}
void frameBuffer_size_callback(GLFWwindow *window, int width, int height){
	glViewport(0, 0, width, height);
}
unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc){
	unsigned int vertexShader = compileShader(vertexSrc, GL_VERTEX_SHADER);
	unsigned int fragmentShader = compileShader(fragmentSrc, GL_FRAGMENT_SHADER);

	unsigned int program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	//check for errors
	int success = 0;
	char infoLog[512];
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success){
		glGetProgramInfoLog(program, 512, NULL, infoLog);
		std::cout << "Program Linking Failed:\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

GLFWwindow* windowInit( int width, int height){
	GLFWwindow* window;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr){
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback( window, frameBuffer_size_callback);

	if(!(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))){
		std::cout << "Failed Loading Glad" << std::endl;
		return nullptr;
	}

	return window;
}

