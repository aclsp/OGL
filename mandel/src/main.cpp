#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//defines
#define windowWidth 1920
#define windowHeight 1080

void vertexBuffer(GLuint *vbo);
void processInput(GLFWwindow *window);
void frameBuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callBack(GLFWwindow *window, double Xpos, double Ypos);
void scroll_callBack(GLFWwindow *window, double xoffset, double yoffset);

GLFWwindow* windowInit( int width, int height);

glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);

glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);

bool firstMouse = true;
float deltaTime, lastFrame = 0.0f;

float lastX = windowWidth / 2.0f;
float lastY = windowHeight / 2.0f;

float zoom = 1.0f;
int main(){
	//create triangle
	float t1[] = {
    -10.0f,  10.0f, 0.0f,
     10.0f,  10.0f, 0.0f,
     10.0f, -10.0f, 0.0f,
    -10.0f, -10.0f, 0.0f
	};
	int indices[] = {
		
		1, 2, 3,
		0, 1, 3
	};

	//create window
	GLFWwindow* window = windowInit( windowWidth, windowHeight);

	//link vertex and fragment
	Shader shader_1( "shaders/vertex.vert", "shaders/fragment.frag");
	
	//Vertex buffer and array object 
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	int nmbr_Attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nmbr_Attribs);
	std::cout<< "Max vertex Attributes" << nmbr_Attribs << std::endl;
	
	//Bind
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(t1), t1, GL_STATIC_DRAW);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	GLuint positionAttribute = glGetAttribLocation(shader_1.ID, "aPos");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);

	

	glBindVertexArray(0);	
	//render loop
	while(!glfwWindowShouldClose(window)){
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
	
		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader_1.use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, translation);

		glm::mat4 projection = glm::mat4(1.0f);
		
		glm::mat4 view = glm::mat4(1.0f);
		projection = glm::ortho(
			-2.5f * zoom, 
			 1.0f * zoom, 
			-1.5f * zoom, 
			 1.5f * zoom
		);
		
		int maxIter = 256 + (int)(log(1.0f / zoom) * 50);

		shader_1.setInt("maxIter", maxIter);		
		shader_1.setMat4("model", model);
		shader_1.setMat4("view", view);
		shader_1.setMat4("projection", projection);	

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
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
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback( window, mouse_callBack);
	glfwSetScrollCallback(window, scroll_callBack);

	if(!(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))){
		std::cout << "Failed Loading Glad" << std::endl;
		return nullptr;
	}

	return window;
}
void mouse_callBack( GLFWwindow* window, double xpos, double ypos){
	if (firstMouse){
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;

	translation.x += (xoffset / windowWidth)  * 3.5f * zoom;
	translation.y -= (yoffset / windowHeight) * 3.0f * zoom; 
}
void scroll_callBack( GLFWwindow* window, double xOffset, double yOffset){
	zoom *= (yOffset > 0) ? 0.9f : 1.1f;
}
// camera.h and camera.cpp
