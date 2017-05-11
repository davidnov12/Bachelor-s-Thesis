/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * ModelLoader.h - trida pro nacteni modelu
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

using namespace glm;

class ModelLoader {

public:

	// Rezimy nacitani
	enum Modes {
		VERTEX_UVS_NORMALS,
		VERTEX_UVS,
		VERTEX_NORMALS,
		VERTEX
	};

	// Material modelu
	typedef struct material {
		float absorption;
		float disperse;
		float reflection;
		vec3 color;
	} Material;

	ModelLoader(std::string path, Modes mode, Material material);

	void cleanData();

	// Ziskana data
	std::vector<vec4> vertex;
	std::vector<vec4> uvs;
	std::vector<vec4> normal;
	std::vector<vec4> colors;
	
private:

	void loadModel(std::string path, Modes mode);

	// Nactene vrcholy, tex. souradnice, normaly
	std::vector<vec3> vertices;
	std::vector<vec2> tex_coords;
	std::vector<vec3> normals;

	Modes load_mode;
	Material mat;
};