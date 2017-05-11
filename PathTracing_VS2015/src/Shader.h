/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Shader.h - trida pro praci se shader programem
 */

#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

#define SHADER_CNT 6	// Pocet shaderu
#define LOG_SIZE 512	// Velikost logu


class Shader {

public:

	Shader();

	std::string loadShader(GLchar* path);
	void attachShader(GLuint type, const std::string code, GLboolean verbose);
	GLuint compileProgram(GLboolean verbose);
	
	void useProgram();
	GLuint getProgram();

	// Typy shaderu
	enum shaderTypes {
		VERTEX,
		GEOMETRY,
		TESS_CONTROL,
		TESS_EVAL,
		FRAGMENT,
		COMPUTE
	};


private:

	// Atributy
	GLuint shaders[SHADER_CNT];
	GLuint program;

};