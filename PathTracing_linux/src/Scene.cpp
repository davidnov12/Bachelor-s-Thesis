/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Scene.cpp - trida pro reprezentaci sceny
 */

#include "Scene.h"

Scene::Scene(vec3 lightPosition){
	lightPos = lightPosition;
	defaultScene();
}

Scene::Scene(vec3 lightPosition, vec3 lightColor){
	lightPos = lightPosition;
	lightCol = lightColor;
	defaultScene();
}

std::vector<Sphere> Scene::getSpheres(){
	return spheres;
}

std::vector<Scene::Model> Scene::getModels(){
	return models;
}

Scene::Model Scene::getModel(int pos){
	if (models.size() == 0) {
		Scene::Model m = { nullptr };
		return m;
	}
	return models.at(pos);
}

int Scene::totalTriangles(){
	int cnt = 0;

	for (int i = 0; i < triangles_count.size(); i++) {
		cnt += triangles_count.at(i);
	}

	return cnt;
}

int Scene::modelsCount(){
	return models.size();
}

int Scene::spheresCount(){
	return spheres.size();
}

vec3 Scene::getLight(){
	return lightPos;
}

vec3 Scene::getLightColor(){
	return lightCol;
}

Scene::Box Scene::getBox(){
	return cornellBox;
}

void Scene::setWall(int pos, float value){
	cornellBox.walls[pos] = value;
}

void Scene::setWallColor(int pos, vec3 color){
	cornellBox.colors[pos] = color;
}

void Scene::addSphere(Sphere obj){
	spheres.push_back(obj);
}

void Scene::addModel(ModelLoader obj){
	Model md;
	int len = obj.colors.size();
	Primitive* geometry = new Primitive[len];

	std::cout << "Preparing scene..." << std::endl;

	// Pripraveni dat do graficke pameti
	for (int a = 0; a < len; a++) {
		geometry[a].vertex0 = obj.vertex.at((a * 3) + 0);
		geometry[a].vertex1 = obj.vertex.at((a * 3) + 1);
		geometry[a].vertex2 = obj.vertex.at((a * 3) + 2);
		geometry[a].normal0 = obj.normal.at((a * 3) + 0);
		geometry[a].normal1 = obj.normal.at((a * 3) + 1);
		geometry[a].normal2 = obj.normal.at((a * 3) + 2);
		geometry[a].uv0_1 = obj.uvs.at((a * 2) + 0);
		geometry[a].uv2 = obj.uvs.at((a * 2) + 1);
		geometry[a].color_mat = obj.colors.at(a);
	}

	md.data = geometry;
	triangles_count.push_back(len);

	// Odstraneni dat z pameti
	obj.cleanData();

	models.push_back(md);

	std::cout << "Scene ready." << std::endl;
}

void Scene::addModel(Model mod){
	models.push_back(mod);
}

void Scene::translateObject(int modelID, vec3 translate){
	Model md = models.at(modelID);

	for(int i = 0; i < triangles_count.at(modelID); i++){
		md.data[i].vertex0 = md.data[i].vertex0 + vec4(translate, 0.0f);
		md.data[i].vertex1 = md.data[i].vertex1 + vec4(translate, 0.0f);
		md.data[i].vertex2 = md.data[i].vertex2 + vec4(translate, 0.0f);
	}
}

void Scene::scaleObject(int modelID, vec3 scale){
	Model md = models.at(modelID);

	for (int i = 0; i < triangles_count.at(modelID); i++) {
		md.data[i].vertex0 = md.data[i].vertex0 * vec4(scale, 1.0f);
		md.data[i].vertex1 = md.data[i].vertex1 * vec4(scale, 1.0f);
		md.data[i].vertex2 = md.data[i].vertex2 * vec4(scale, 1.0f);
	}
}

void Scene::defaultScene(){

	// Defaultni nastaveni sten boxu

	cornellBox.walls[0] = DEFAULT_DOWN;
	cornellBox.colors[0] = vec3(1);
	cornellBox.normals[0] = vec3(0, 1, 0);
	cornellBox.absorption[0] = 0.9;
	cornellBox.disperse[0] = 0.0;
	cornellBox.reflectivity[0] = 0.0;

	cornellBox.walls[1] = DEFAULT_UP;
	cornellBox.colors[1] = vec3(1);
	cornellBox.normals[1] = vec3(0, -1, 0);
	cornellBox.absorption[1] = 0.9;
	cornellBox.disperse[1] = 0.0;
	cornellBox.reflectivity[1] = 0.0;

	cornellBox.walls[2] = DEFAULT_LEFT;
	cornellBox.colors[2] = vec3(1);
	cornellBox.normals[2] = vec3(1, 0, 0);
	cornellBox.absorption[2] = 0.9;
	cornellBox.disperse[2] = 0.0;
	cornellBox.reflectivity[2] = 0.0;

	cornellBox.walls[3] = DEFAULT_RIGHT;
	cornellBox.colors[3] = vec3(1);
	cornellBox.normals[3] = vec3(-1, 0, 0);
	cornellBox.absorption[3] = 0.9;
	cornellBox.disperse[3] = 0.0;
	cornellBox.reflectivity[3] = 0.0;

	cornellBox.walls[4] = DEFAULT_FRONT;
	cornellBox.colors[4] = vec3(1);
	cornellBox.normals[4] = vec3(0, 0, 1);
	cornellBox.absorption[4] = 0.9;
	cornellBox.disperse[4] = 0.0;
	cornellBox.reflectivity[4] = 0.0;
}
