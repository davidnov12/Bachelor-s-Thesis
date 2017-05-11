/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Camera.h - trida kamery
 */

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

class Camera {

public:

	Camera(vec3 pos, double width, double height);

	// View a Projection matice
	mat4 getViewMat();
	mat4 getProjMat();

	vec3 getPosition();

private:
	
	const double FOV = 45.0;
	double width, height;

	// Smerove vektory
	vec3 position;
	vec3 up;
	vec3 front = vec3(0.0f, 0.0f, -1.0f);

};