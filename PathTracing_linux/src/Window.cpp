/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Window.cpp - trida pro spravu okna
 */

#include "Window.h"

float x_off, y_off;
float lx_off, lz_off;
float lastX, lastY;
float rWidth, rHeight;
bool firstMouse = true;
bool pressed;
bool resized;
bool light_move;
bool save_img = false;
bool walls = false;


#define CAMERA_OFFSET 0.05		// Posuv kamery
#define LIGHT_OFFSET 0.07		// Posuv svetla

#define CAMERA_MAX_X 0.45		// Maximalni x pozice kamery
#define CAMERA_MIN_X -0.45		// Minimalni x pozice kamery
#define CAMERA_MAX_Y 0.3		// Maximalni y pozice kamery
#define CAMERA_MIN_Y -0.3		// Minimalni y pozice kamery

#define LIGHT_MAX_X 0.45		// Maximalni x pozice svetla
#define LIGHT_MIN_X -0.45		// Minimalni x pozice svetla
#define LIGHT_MAX_Z 0.55		// Maximalni z pozice svetla
#define LIGHT_MIN_Z -0.35		// Minimalni z pozice svetla


// Osetreni udalosti od klavesnice
void keyboard(GLFWwindow* window, int key, int scancode, int action, int mode) {
	
	// Klavesy WASD - kamera
	if (key == GLFW_KEY_W && action == GLFW_PRESS)
		y_off += CAMERA_OFFSET;
	if (key == GLFW_KEY_A && action == GLFW_PRESS)
		x_off -= CAMERA_OFFSET;
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		y_off -= CAMERA_OFFSET;
	if (key == GLFW_KEY_D && action == GLFW_PRESS)
		x_off += CAMERA_OFFSET;

	// Klavesy IJKL - svetlo
	if (key == GLFW_KEY_I && action == GLFW_PRESS) {
		lz_off += LIGHT_OFFSET;
		light_move = true;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS) {
		lx_off -= LIGHT_OFFSET;
		light_move = true;
	}
	if (key == GLFW_KEY_K && action == GLFW_PRESS){
		lz_off -= LIGHT_OFFSET;
		light_move = true;
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS){
		lx_off += LIGHT_OFFSET;
		light_move = true;
	}

	// Klavesa P - ulozeni snimku
	if (key == GLFW_KEY_P && action == GLFW_PRESS) {
		save_img = true;
	}

	// Klavesa T - barvy steny
	if (key == GLFW_KEY_T && action == GLFW_PRESS) {
		walls = true;
	}
	
	// Kontrola max/min hodnot
	if (x_off > CAMERA_MAX_X) x_off = CAMERA_MAX_X;
	if (x_off < CAMERA_MIN_X) x_off = CAMERA_MIN_X;
	
	if (y_off > CAMERA_MAX_Y) y_off = CAMERA_MAX_Y;
	if (y_off < CAMERA_MIN_Y) y_off = CAMERA_MIN_Y;

	if (lx_off > LIGHT_MAX_X) lx_off = LIGHT_MAX_X;
	if (lx_off < LIGHT_MIN_X) lx_off = LIGHT_MIN_X;

	if (lz_off > LIGHT_MAX_Z) lz_off = LIGHT_MAX_Z;
	if (lz_off < LIGHT_MIN_Z) lz_off = LIGHT_MIN_Z;


}

// Udalost pri pohybu mysi
void mouse(GLFWwindow* window, double xpos, double ypos) {

	if (pressed) {
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		x_off -= 0.002f * (xpos - lastX);
		y_off -= 0.002f * (lastY - ypos);

		lastX = xpos;
		lastY = ypos;
	}

	lastX = xpos;
	lastY = ypos;

	// Kontrola max/min hodnot
	if (x_off > CAMERA_MAX_X) x_off = CAMERA_MAX_X;
	if (x_off < CAMERA_MIN_X) x_off = CAMERA_MIN_X;

	if (y_off > CAMERA_MAX_Y) y_off = CAMERA_MAX_Y;
	if (y_off < CAMERA_MIN_Y) y_off = CAMERA_MIN_Y;

	
}

// Udalost pri stisknuti mysi
void press(GLFWwindow* window, int button, int action, int mods) {

	if (action == GLFW_PRESS) pressed = true;
	else pressed = false;
}

// Udalost pri zmene velikosti okna
void resize(GLFWwindow* window, int width, int height) {
	rWidth = (float)width;
	rHeight = (float)height;

	glViewport(0, 0, width, height);

	resized = true;
}


Window::Window(int width, int height, std::string title, bool visible){
	this->width = width;
	this->height = height;
	this->title = title;
	this->visible = visible;
	this->share = NULL;
	window = createWindow();
}

Window::Window(int width, int height, std::string title, bool visible, GLFWwindow * shared){
	this->width = width;
	this->height = height;
	this->title = title;
	this->visible = visible;
	this->share = shared;
	window = createWindow();
}

void Window::closeWindow(){
	glfwTerminate();
}

GLFWwindow * Window::getWindow(){
	return window;
}

bool Window::getCloseState(){
	if(glfwWindowShouldClose(window)) return true;
	
	glfwPollEvents();
	return false;
}

float Window::getXOffset() {
	return x_off;
}

float Window::getYOffset() {
	return y_off;
}

float Window::getLXOffset(){
	return lx_off;
}

float Window::getLZOffset(){
	return lz_off;
}

float Window::getCurrentWidth(){
	return rWidth;
}

float Window::getCurrentHeight(){
	return rHeight;
}

bool Window::isResized(){
	
	if (save_img) {
		savePNG();
		save_img = false;
	}
	
	if (resized) {
		resized = !resized;
		width = getCurrentWidth();
		height = getCurrentHeight();
		return true;
	}
	else
		return false;
}

bool Window::lightMove(){
	if (light_move) {
		light_move = false;
		return true;
	}
	return false;
}

bool Window::wallsColor(){
	if(walls){
		walls = false;
		return true;
	}
	return false;
}

void Window::savePNG(){
	char* pixels = new char[width * height * 4];

	// Data z framebufferu
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	std::vector<unsigned char> data(width * height * 4), png;

	// Prohozeni pozic pixelu
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			int oldPos = (height - i - 1) * (width * 4) + 4 * j;
			int newPos = i * (width * 4) + 4 * j;
			data[newPos] = pixels[oldPos];
			data[newPos + 1] = pixels[oldPos + 1];
			data[newPos + 2] = pixels[oldPos + 2];
			data[newPos + 3] = (char)0xff;
		}
	}

	// Ulozeni do PNG souboru
	lodepng::encode(png, data, width, height);
	lodepng::save_file(png, "screenshots/screen_" + std::to_string(saves) + ".png");

	// Odstraneni dat
	data.clear();
	png.clear();
	delete[] pixels;

	saves++;
}

void Window::swapBuffers(){
	glfwSwapBuffers(window);
}

void Window::setTitle(std::string title){
	glfwSetWindowTitle(window, title.c_str());
}

GLFWwindow * Window::createWindow(){
	GLFWwindow* window;
	int w, h;

	if (!glfwInit())
		return NULL;

	// Atributy okna
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
	if(!visible) glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 16);
	
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);

	// Povedlo se vytvorit okno ?
	if (!window) {
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(window);

	// Nastaveni callbacku k udalostem
	glfwSetKeyCallback(window, keyboard);
	glfwSetMouseButtonCallback(window, press);
	glfwSetCursorPosCallback(window, mouse);
	glfwSetWindowSizeCallback(window, resize);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		glfwTerminate();
		return NULL;
	}

	// Nastaveni orezoveho okna
	glfwGetFramebufferSize(window, &w, &h);
	glViewport(0, 0, w, h);

	// Povoleni MSAA a Blendingu
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return window;
}
