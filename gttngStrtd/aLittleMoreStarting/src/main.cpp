#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <iostream>
#include <cmath>

//defines
#define windowWidth 800
#define windowHeight 600

void vertexBuffer(GLuint *vbo);
void processInput(GLFWwindow *window);
void frameBuffer_size_callback(GLFWwindow *window, int width, int height);

GLFWwindow* windowInit( int width, int height);

int main(){
	//create triangle
	float t1[] = {
		 0.0f, -0.5f, 0.0f,  0.5f, 0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
		0.25f,  0.8f, 0.0f,  0.0f, 0.0f, 1.0f	};

	//create window
	GLFWwindow* window = windowInit( windowWidth, windowHeight);

	//link vertex and fragment
	Shader shader_1( "shaders/vertex.vert", "shaders/fragment.frag");
	
	//Vertex buffer and array object 
	unsigned int VBO;
	unsigned int VAO;
	
	int nmbr_Attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nmbr_Attribs);
	std::cout<< "Max vertex Attributes" << nmbr_Attribs << std::endl;
	//Bind
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t1), t1, GL_STATIC_DRAW);

	GLuint positionAttribute = glGetAttribLocation(shader_1.ID, "aPos");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)0);

	GLuint colourAttribute = glGetAttribLocation(shader_1.ID, "aColour");
	glEnableVertexAttribArray(colourAttribute);
	glVertexAttribPointer(colourAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)*6, (void*)(3*sizeof(float)));
	
	std::cout<< colourAttribute<<" position:"<< positionAttribute <<std::endl;

	glBindVertexArray(0);	
	//render loop
	while(!glfwWindowShouldClose(window)){
		processInput(window);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader_1.use();

		glBindVertexArray(VAO);
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

void frameBuffer_size_callback(GLFWwindow *window, int width, int height){
	glViewport(0, 0, width, height);
}

GLFWwindow* windowInit( int width, int height){
	GLFWwindow* window;
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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

