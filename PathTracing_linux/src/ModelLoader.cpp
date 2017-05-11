/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * ModelLoader.h - trida pro nacteni modelu
 */


#include "ModelLoader.h"


ModelLoader::ModelLoader(std::string path, Modes mode, Material material){
	load_mode = mode;
	mat = material;
	loadModel(path, mode);
}

void ModelLoader::cleanData(){
	vertices.clear();
	tex_coords.clear();
	normals.clear();
	vertex.clear();
	uvs.clear();
	normal.clear();
	colors.clear();
}

void ModelLoader::loadModel(std::string path, Modes mode){
	
	std::cout << "Loading model..." << std::endl;

	std::ifstream modelFile;
	modelFile.exceptions(std::ifstream::badbit);
	register std::string data;
	register std::string tmp;

	try {
		// Otevreni souboru
		modelFile.open(path);
		std::stringstream stream;

		// Precteni a uzavreni souboru
		stream << modelFile.rdbuf();
		modelFile.close();

		// Obsah souboru
		data = stream.str();
	}

	catch (std::ifstream::failure e) {

		std::cout << "READ ERROR" << std::endl;
		return;
	}

	register vec3 temp;
	register vec2 tem;
	register int indices[9];

	while (data.compare("") != 0) {

		// Novy radek
		if (data.find("\n") == std::string::npos) {
			tmp = data.substr(0, data.find("\0"));
			tmp = data;
			data = "";
		}
		else {
			tmp = data.substr(0, data.find("\n"));
			data = data.substr(data.find("\n") + 1, data.length() - data.find("\n") - 1);
		}

		
		// Vrchol
		if (tmp.substr(0, 2) == "v ") {
			tmp = tmp.substr(2, tmp.length() - 3);
			sscanf(tmp.c_str(), "%f %f %f", &temp.x, &temp.y, &temp.z);
			vertices.push_back(temp);
		}
		

		// Texturovaci souradnice
		else if (tmp.substr(0, 3) == "vt ") {
			tmp = tmp.substr(3, tmp.length() - 4);
			sscanf(tmp.c_str(), "%f %f", &tem.x, &tem.y);
			tex_coords.push_back(tem);
		}
		
		// Normala
		else if (tmp.substr(0, 3) == "vn ") {
			tmp = tmp.substr(3, tmp.length() - 4);
			sscanf(tmp.c_str(), "%f %f %f", &temp.x, &temp.y, &temp.z);
			normals.push_back(temp);
		}
		
		// Primitivum
		else if (tmp.substr(0, 2) == "f ") {
			
			tmp = tmp.substr(2, tmp.length());
			
			// Nacteni dat o primitivu
			if (mode == VERTEX_UVS_NORMALS) {
				sscanf(tmp.c_str(), "%d/%d/%d %d/%d/%d %d/%d/%d", indices, indices + 1, indices + 2,
					indices + 3, indices + 4, indices + 5, indices + 6, indices + 7, indices + 8);

				// Sestaveni primitiva

				vertex.push_back(vec4(vertices.at(indices[0] - 1), 1.0f));
				vertex.push_back(vec4(vertices.at(indices[3] - 1), 1.0f));
				vertex.push_back(vec4(vertices.at(indices[6] - 1), mat.disperse));

				uvs.push_back(vec4(tex_coords.at(indices[1] - 1), tex_coords.at(indices[4] - 1)));
				uvs.push_back(vec4(tex_coords.at(indices[7] - 1), 1.0f, 1.0f));

				normal.push_back(vec4(normals.at(indices[2] - 1), 1.0f));
				normal.push_back(vec4(normals.at(indices[5] - 1), 1.0f));
				normal.push_back(vec4(normals.at(indices[8] - 1), mat.absorption));

				colors.push_back(vec4(0.9, 0.9, 0.9, 0.0));

			}
			else if (mode == VERTEX_NORMALS) {
				
				// Nacteni dat o primitivu
				if (sscanf(tmp.c_str(), "%d//%d %d//%d %d//%d", indices, indices + 1, indices + 2,
					indices + 3, indices + 4, indices + 5) != 6) {
					return;
				}

				// Sestaveni primitiva

				vertex.push_back(vec4(vertices.at(indices[0] - 1), 1.0f));
				vertex.push_back(vec4(vertices.at(indices[2] - 1), 1.0f));
				vertex.push_back(vec4(vertices.at(indices[4] - 1), mat.disperse));

				uvs.push_back(vec4(0.0f));
				uvs.push_back(vec4(0.0f));

				normal.push_back(vec4(normals.at(indices[1] - 1), 1.0f));
				normal.push_back(vec4(normals.at(indices[3] - 1), 1.0f));
				normal.push_back(vec4(normals.at(indices[5] - 1), mat.absorption));

				colors.push_back(vec4(mat.color, mat.reflection));

			}
			else if (mode == VERTEX_UVS) {

				// Nacteni dat o primitivu
				if (sscanf(tmp.c_str(), "%d//%d %d//%d %d//%d", indices, indices + 1, indices + 2,
					indices + 3, indices + 4, indices + 5) != 6) {
					return;
				}

				// Sestaveni primitiva

				vertex.push_back(vec4(vertices.at(indices[0] - 1), 1.0f));
				vertex.push_back(vec4(vertices.at(indices[2] - 1), 1.0f));
				vertex.push_back(vec4(vertices.at(indices[4] - 1), mat.disperse));

				uvs.push_back(vec4(tex_coords.at(indices[1] - 1), tex_coords.at(indices[3] - 1)));
				uvs.push_back(vec4(tex_coords.at(indices[5] - 1), 1.0f, 1.0f));

				normal.push_back(vec4(0.0f));
				normal.push_back(vec4(0.0f));
				normal.push_back(vec4(mat.absorption));

				colors.push_back(vec4(mat.color, mat.reflection));

			}
			else if (mode == VERTEX) {

				// Nacteni dat o primitivu 
				sscanf(tmp.c_str(), "%d %d %d", indices, indices + 1, indices + 2);

				// Sestaveni primitiva

				vertex.push_back(vec4(vertices.at(indices[0] - 1), 1.0f));
				vertex.push_back(vec4(vertices.at(indices[1] - 1), 1.0f));
				vertex.push_back(vec4(vertices.at(indices[2] - 1), mat.disperse));

				uvs.push_back(vec4(0.0f));
				uvs.push_back(vec4(0.0f));

				normal.push_back(vec4(0.0f));
				normal.push_back(vec4(0.0f));
				normal.push_back(vec4(mat.absorption));

				colors.push_back(vec4(mat.color, mat.reflection));
			}
		}
	}
}
