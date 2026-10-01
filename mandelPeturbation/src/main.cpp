#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <iostream>
#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <mpfr.h>


void vertexBuffer(GLuint *vbo);
void frameBuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callBack(GLFWwindow *window, double Xpos, double Ypos);
void scroll_callBack(GLFWwindow *window, double xoffset, double yoffset);
void computeOrbit(mpfr_t cx, mpfr_t cy, int maxIter, std::vector<float>& orbit);
void zoom(float factor);
void updateZoomPrec();
void growPrec(mpfr_t x, int bits);

GLFWwindow* windowInit();
float lastFrameTime = 0.0f;
struct CurrFrame{
	mpfr_t center_real, center_imag;
	bool firstMouse = true;
	float zoom = 1.0f;
	float lastX {};
	float lastY {};

	CurrFrame(int bits) {
		mpfr_init2(center_real, bits);
		mpfr_init2(center_imag, bits);
		mpfr_set_d(center_real, -0.5, MPFR_RNDN);  
		mpfr_set_d(center_imag, 0.0, MPFR_RNDN);	

	}

   ~CurrFrame(){
	   mpfr_clear(center_real);
	   mpfr_clear(center_imag);
   }
};
CurrFrame frameDetails(128);


int main(){
	//create triangle
	float t1[] = {
    -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f
	};
	int indices[] = {
		
		1, 2, 3,
		0, 1, 3
	};

	//create window
	GLFWwindow* window = windowInit();
	//link vertex and fragment
	Shader shader_1( "shaders/vertex.vert", "shaders/fragment.frag");
	
	//Vertex buffer and array object 
	unsigned int VBO;
	unsigned int VAO;
	unsigned int EBO;

	int nmbr_Attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nmbr_Attribs);
	std::cout<< "Max vertex Attributes" << nmbr_Attribs << std::endl;
	
	//Bind for main rectangle
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

	//room on gpu for accurate center orbit
	unsigned int orbitSSBO;
	glGenBuffers(1, &orbitSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, orbitSSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, orbitSSBO);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(0);

	//render loop
	while(!glfwWindowShouldClose(window)){
		glBindFramebuffer(GL_FRAMEBUFFER, 0);	
		processInput(window);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader_1.use();

		// to hold accurate orbit	
		std::vector<float> centerOrbit; 
		int maxIter = 256 + (int)(log(1.0f / frameDetails.zoom) * 50);
		
		//find it	
		computeOrbit( frameDetails.center_real, frameDetails.center_imag, maxIter, centerOrbit);
		
		//pass it	
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, orbitSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, centerOrbit.size() * sizeof(float), centerOrbit.data(), GL_DYNAMIC_DRAW);
	
		glm::vec2 center_vec(
			(float)mpfr_get_d(frameDetails.center_real, MPFR_RNDN),
			(float)mpfr_get_d(frameDetails.center_imag, MPFR_RNDN)
		);
		
		shader_1.setInt("maxIter", maxIter);		
		shader_1.setFloat("zoom", frameDetails.zoom);
		shader_1.setVec2("center", center_vec);
		
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	//clean up
	glfwTerminate();

	return 0;
}

void vertexBuffer(GLuint *vbo){}

void processInput(GLFWwindow *window){
	float currentTime = static_cast<float>(glfwGetTime());	
	float deltaTime = currentTime - lastFrameTime;
	lastFrameTime = currentTime;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float zoomRate = 1.0f;	
	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		zoom(expf(-zoomRate * deltaTime));
	}
	else if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		zoom(expf(zoomRate * deltaTime));
	}
}

void frameBuffer_size_callback(GLFWwindow *window, int width, int height){
	glViewport(0, 0, width, height);
}

GLFWwindow* windowInit(){
	GLFWwindow* window;
	glfwInit();
	
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);	

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	window = glfwCreateWindow(mode->width, mode->height, "Aidan's MandelBrot Set", monitor, nullptr);
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
	
	int frameBufferWidth, frameBufferHeight;
	glfwGetFramebufferSize(
		window,
		&frameBufferWidth,
		&frameBufferHeight
	);

	glViewport(
		0,
		0,
		frameBufferWidth,
		frameBufferHeight
	);
	return window;
}
void computeOrbit(mpfr_t cx, mpfr_t cy, int maxIter, std::vector<float>& orbit){
	mpfr_t zx, zy, tmp, tmp2;
	mpfr_init2(zx, mpfr_get_prec(cx));
	mpfr_init2(zy, mpfr_get_prec(cx));
	mpfr_init2(tmp, mpfr_get_prec(cx));
	mpfr_init2(tmp2, mpfr_get_prec(cx));

	mpfr_set_d(zx, 0.0, MPFR_RNDN);
	mpfr_set_d(zy, 0.0, MPFR_RNDN);
	orbit.push_back(0.0);
	orbit.push_back(0.0);
	for( int i = 0; i < maxIter; i++){
		//tmp = zx;	
		//zx = zx*zx - zy*zy + cx;
		//zy = 2*tmp*zy + cy;
		
		mpfr_mul(tmp, zx, zx, MPFR_RNDN);
		mpfr_mul(tmp2, zy, zy, MPFR_RNDN);
		mpfr_sub(tmp, tmp, tmp2, MPFR_RNDN);
		
		mpfr_set(tmp2, zx, MPFR_RNDN);
		mpfr_mul(tmp2, zy, tmp2, MPFR_RNDN);
		mpfr_mul_d(tmp2, tmp2, 2.0, MPFR_RNDN);
		
		mpfr_add(zx, tmp, cx, MPFR_RNDN);
		mpfr_add(zy, tmp2, cy, MPFR_RNDN);

		orbit.push_back(mpfr_get_d(zx, MPFR_RNDN));
		orbit.push_back(mpfr_get_d(zy, MPFR_RNDN));
	}
	mpfr_clear(zx);
	mpfr_clear(zy);
	mpfr_clear(tmp);
	mpfr_clear(tmp2);
}
void mouse_callBack( GLFWwindow* window, double xpos, double ypos){
	if (frameDetails.firstMouse){
		frameDetails.lastX = xpos;
		frameDetails.lastY = ypos;
		frameDetails.firstMouse = false; 
	}

	float xoffset = xpos - frameDetails.lastX;
	float yoffset = ypos - frameDetails.lastY;
	frameDetails.lastX = xpos;
	frameDetails.lastY = ypos;
	
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	double dx = (xoffset / width) * 3.5 * frameDetails.zoom;
	double dy = (yoffset / height) * 3.0 * frameDetails.zoom;

	mpfr_add_d(frameDetails.center_real, frameDetails.center_real, dx, MPFR_RNDN);
	mpfr_sub_d(frameDetails.center_imag, frameDetails.center_imag, dy, MPFR_RNDN);
}
void scroll_callBack( GLFWwindow* window, double xOffset, double yOffset){
	frameDetails.zoom *= (yOffset > 0) ? 0.9f : 1.1f;
	double real_val = mpfr_get_d(frameDetails.center_real, MPFR_RNDN);
	double imag_val = mpfr_get_d(frameDetails.center_imag, MPFR_RNDN);


	mpfr_set_d(frameDetails.center_real, real_val, MPFR_RNDN);
	mpfr_set_d(frameDetails.center_imag, imag_val, MPFR_RNDN);
}
void zoom( float factor){
	frameDetails.zoom *= factor;
	updateZoomPrec();
}
void updateZoomPrec(){
	int bits = (int)(log2(1.0f / frameDetails.zoom)) + 128;
	growPrec(frameDetails.center_real, bits);
	growPrec(frameDetails.center_imag, bits);
}	
void growPrec(mpfr_t x, int bits){
	if (mpfr_get_prec(x) < bits)
		mpfr_prec_round(x, bits, MPFR_RNDN);
}
// camera.h and camera.cpp
