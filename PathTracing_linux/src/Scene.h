/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Scene.h - trida pro reprezentaci sceny
 */

#pragma once

#include <vector>
#include "Sphere.h"
#include "ModelLoader.h"
#include <glm/glm.hpp>

#define DEFAULT_DOWN -0.45		// Spodni stena
#define DEFAULT_UP 0.55			// Horni stena
#define DEFAULT_LEFT -0.7		// Leva stena
#define DEFAULT_RIGHT 0.7		// Prava stena
#define DEFAULT_FRONT -0.9		// Predni stena


using namespace glm;

class Scene {
		
public:

	// Steny sceny
	typedef enum wall {
		DOWN_WALL,
		UP_WALL,
		LEFT_WALL,
		RIGHT_WALL,
		FRONT_WALL
	};

	// Struktura pro reprezentaci cornell boxu
	typedef struct box {
		float walls[5];
		vec3 normals[5];
		vec3 colors[5];
		float absorption[5];
		float disperse[5];
		float reflectivity[5];
	} Box;

	// Struktura pro ulozeni primitiva (trojuhelniku)
	typedef struct primitive {
		vec4 vertex0;
		vec4 vertex1;
		vec4 vertex2;
		vec4 normal0;
		vec4 normal1;
		vec4 normal2;
		vec4 uv0_1;
		vec4 uv2;
		vec4 color_mat;
	} Primitive;

	// Struktura pro ulozeni modelu
	typedef struct model {
		Primitive* data;
	} Model;

	Scene(vec3 lightPosition);
	Scene(vec3 lightPosition, vec3 lightColor);

	// Ziskani obsahu sceny
	std::vector<Sphere> getSpheres();
	std::vector<Model> getModels();
	Model getModel(int pos);
	int totalTriangles();
	int modelsCount();
	int spheresCount();
	vec3 getLight();
	vec3 getLightColor();

	// Nastaveni boxu
	Box getBox();
	void setWall(int pos, float value);
	void setWallColor(int pos, vec3 color);

	// Pridani objektu do sceny
	void addSphere(Sphere obj);
	void addModel(ModelLoader obj);
	void addModel(Model mod);

	void translateObject(int modelID, vec3 translate);
	void scaleObject(int modelID, vec3 scale);

private:

	// Defaultni nastaveni sceny
	void defaultScene();

	// Obsah sceny
	std::vector<Sphere> spheres;
	std::vector<Model> models;

	// Svetlo sceny
	vec3 lightPos;
	vec3 lightCol;

	// Pocty trojuhelniku v modelech
	std::vector<int> triangles_count;

	Box cornellBox;
};