/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Camera.cpp - trida kamery
 */

#include "Camera.h"

Camera::Camera(vec3 pos, double width, double height){
	this->width = width;
	this->height = height;
	position = pos;
}

mat4 Camera::getViewMat(){
	return lookAt(position, position + front, up);
}

mat4 Camera::getProjMat(){
	return perspective(FOV, width / height, 0.1, 1000.0);
}

vec3 Camera::getPosition(){
	return position;
}
