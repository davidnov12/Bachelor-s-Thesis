/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Window.h - trida pro spravu okna
 */

#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "png/lodepng.h"


class Window {

public:

	Window(int width, int height, std::string title, bool visible);
	Window(int width, int height, std::string title, bool visible, GLFWwindow* shared);

	void closeWindow();
	GLFWwindow* getWindow();

	bool getCloseState();
	void swapBuffers();
	void setTitle(std::string title);

	// Pozice kamery - jeji offset
	float getXOffset();
	float getYOffset();

	// Pozice svetla - jeho offset
	float getLXOffset();
	float getLZOffset();

	float getCurrentWidth();
	float getCurrentHeight();
	
	bool isResized();
	bool lightMove();
	bool wallsColor();

	void savePNG();

private:

	// Atributy okna
	int width, height;
	std::string title;
	bool visible;

	int saves = 0;

	// Okno
	GLFWwindow* window;
	GLFWwindow* share;

	// Vytvoreni okna
	GLFWwindow* createWindow();
};
