#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <iostream>
#include <cmath>
#include <camera.h>


void vertexBuffer(GLuint *vbo);
void processInput(GLFWwindow *window);
void frameBuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callBack(GLFWwindow* window, double xPos, double yPos);
void scroll_callBack(GLFWwindow* window, double xoffset, double yoffset);
GLFWwindow* windowInit( int width, int height);


const unsigned int windowWidth = 800;
const unsigned int windowHeight = 600;

float deltaTime, lastFrame = 0.0f;

float lastX = 400, lastY = 300;
bool firstMouse = true;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

glm::vec3 lightPos(1.2f, 1.2f, 2.0f);

int main(){
	//create triangle
	float vertices[] = {
	// positions          //normal Vectors  
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};	
	GLFWwindow* window = windowInit( windowWidth, windowHeight);

	//link vertex and fragment
	Shader lightingShader( "shaders/basicLighting.vs", "shaders/basicLighting.fs");
	Shader lightCubeShader( "shaders/lightCube.vs", "shaders/lightCube.fs");
	
	//Vertex buffer and array object 
	unsigned int VBO, VAO;
	
	//Bind
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint positionAttribute_LS = glGetAttribLocation(lightingShader.ID, "aPos");
	glEnableVertexAttribArray(positionAttribute_LS);
	glVertexAttribPointer(positionAttribute_LS, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);

	GLuint normalAttribute = glGetAttribLocation(lightingShader.ID, "aNormal");
	glEnableVertexAttribArray(normalAttribute);
	glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3 * sizeof(float))); 

	unsigned int lightVAO;

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	GLuint positionAttribute_LCS = glGetAttribLocation(lightCubeShader.ID, "aPos");
	glEnableVertexAttribArray(positionAttribute_LCS);
	glVertexAttribPointer(positionAttribute_LCS, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);


	glEnableVertexAttribArray(0);
	glBindVertexArray(0);	
	//render loop
	while(!glfwWindowShouldClose(window)){
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;	

		processInput(window);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightPos.x = sin(glfwGetTime()) * 1.5f;
		lightPos.y = cos(glfwGetTime()) * 1.5f;
		lightPos.z = cos(glfwGetTime()) * 1.0f;
	 	lightingShader.use();
		lightingShader.setVec3("lightPos", lightPos);
       	lightingShader.setVec3("objectColour", 1.0f, 0.5f, 0.31f);
		lightingShader.setVec3("lightColour", 1.0f, 1.0f, 1.0f);
		lightingShader.setVec3("viewPos", camera.Position);
        
		glm::mat4 projection = glm::mat4(1.0f);

       	glm::mat4 view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);			
		lightingShader.setMat4("view", view);
		lightingShader.setMat4("projection", projection);
			
		glm::mat4 model = glm::mat4(1.0f); 
		lightingShader.setMat4("model", model);
		
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightCubeShader.use();
		lightCubeShader.setMat4("projection", projection);
		lightCubeShader.setMat4("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShader.setMat4("model", model);

		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	//clean up
	glfwTerminate();
	return 0;
}

void vertexBuffer(GLuint *vbo){}

void processInput(GLFWwindow *window){
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
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
	glfwSetCursorPosCallback( window, mouse_callBack);
	glfwSetScrollCallback(window, scroll_callBack);

	if(!(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))){
		std::cout << "Failed Loading Glad" << std::endl;
		return nullptr;
	}
	glEnable(GL_DEPTH_TEST);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return window;
}
void mouse_callBack(GLFWwindow* window, double xPos, double yPos){
	if(firstMouse){
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}	
	float xOffset = xPos - lastX;
	float yOffset = yPos - lastY;
	lastX = xPos;
	lastY = yPos;
	
	camera.ProcessMouseMovement(xOffset, yOffset);
}
void scroll_callBack(GLFWwindow* window, double xoffset, double yoffset){
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
