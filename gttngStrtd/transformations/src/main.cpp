#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb_image.h>
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
	float vertices[] = {
	// positions          // colors           // texture1 coords
     0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
     0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
	};
	
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	
	//create window
	GLFWwindow* window = windowInit( windowWidth, windowHeight);

	//link vertex and fragment
	Shader shader_1( "shaders/vertex.vert", "shaders/fragment.frag");
	
	//Vertex buffer and array object 
	unsigned int VBO, VAO, EBO;
	
	//Bind
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	GLuint positionAttribute = glGetAttribLocation(shader_1.ID, "aPos");
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)0);

	GLuint colourAttribute = glGetAttribLocation(shader_1.ID, "aColour");
	glEnableVertexAttribArray(colourAttribute);
	glVertexAttribPointer(colourAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(3*sizeof(float)));

	GLuint texAttrib = glGetAttribLocation(shader_1.ID, "aTexCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(float)*8, (void*)(6*sizeof(float)));
	
	std::cout<< "aPos:" << positionAttribute << " aColour:" << colourAttribute << " aTexCoord:" << texAttrib << std::endl;
	stbi_set_flip_vertically_on_load(true);
	//Texture 1 Container
	int TEXTURE_width, TEXTURE_height, nrChannels; 
	unsigned char *data = stbi_load("textures/container.jpg", &TEXTURE_width, &TEXTURE_height, &nrChannels, 0);

	unsigned int texture1;
	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (data)
	{	
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEXTURE_width, TEXTURE_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	    glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
	    std::cout << "Failed to load texture1" << std::endl;
	}
	
	unsigned char *data_awesomeface = stbi_load("textures/awesomeface.png", &TEXTURE_width, &TEXTURE_height, &nrChannels, 0);

	unsigned int texture2;
	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	if (data_awesomeface)
	{	
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TEXTURE_width, TEXTURE_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data_awesomeface);
	    glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
	    std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(data);
	stbi_image_free(data_awesomeface);
	
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
	trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
	
	shader_1.use();
	shader_1.setInt("ourTexture1", 0);
	shader_1.setInt("ourTexture2", 1);

	unsigned int transformLoc = glGetUniformLocation(shader_1.ID, "transform");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
	std::cout << "transform location:" << transformLoc << std::endl;
	
	glBindVertexArray(0);	
	//render loop
	while(!glfwWindowShouldClose(window)){
		processInput(window);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader_1.use();
		glActiveTexture(GL_TEXTURE0);	
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);	
		glBindTexture(GL_TEXTURE_2D, texture2);
		
		glBindVertexArray(VAO);
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

	if(!(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))){
		std::cout << "Failed Loading Glad" << std::endl;
		return nullptr;
	}

	return window;
}

