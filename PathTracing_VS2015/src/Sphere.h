/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Sphere.h - trida pro reprezentaci koule
 */

#pragma once

#include <glm/glm.hpp>

using namespace glm;

class Sphere {

public:

	// Material koule
	typedef struct material {
		float absorption;
		float disperse;
		float reflection;
		vec3 color;
	} Material;

	Sphere(float radius, vec3 center, Material material);

private:

	// Parametry koule
	vec3 center;
	float radius;
	vec3 color;
	float reflection;
	float disper;
	float absorp;
	float gap[2];
};