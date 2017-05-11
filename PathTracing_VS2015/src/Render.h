/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Render.h - trida rendereru
 */

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <random>
#include "Shader.h"
#include "Scene.h"
#include "Camera.h"
#include "Octree.h"
#include "Window.h"


using namespace glm;

class Render{

public:

	Render(Shader* program, Scene* scene, Camera* cam, Octree* tree);

	void updateScene();
	void setUniforms(float x, float y, float lx, float lz, bool resized, bool light_move, bool* moved, bool walls, int width, int height);
	void setStride(int newStride);
	
	float getSamples();
	void draw();
	void finish();

private:

	// Souradnice prumetny
	vec3 screenCoords[600];

	GLuint vao, vbo;

	// Vzorky pro PT
	GLfloat id = 1.0, stride = 1.0, step = 1.0;
	GLfloat last_x = 0, last_y = 0;

	bool wallColor = true;

	Shader* program;
	Scene* scene;
	Camera* camera;
	Octree* octree;

	// Vytvoreni VAO a VBO
	void createBuffers();
};