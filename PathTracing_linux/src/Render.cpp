/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Render.cpp - trida rendereru
 */

#include "Render.h"


std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> uniGen(0, 1);

Render::Render(Shader* program, Scene* scene, Camera* cam, Octree* tree){
	this->program = program;
	this->scene = scene;
	camera = cam;
	octree = tree;

	float cx = -1.0, cy = 1.0;
	int i = 0;

	// Sestaveni prumetny
	for (int y = 0; y < 10; y++) {

		for (int x = 0; x < 10; x++) {
			
			screenCoords[(i * 6)] = vec3(cx, cy - 0.2, -0.5);
			screenCoords[(i * 6) + 1] = vec3(cx, cy, -0.5);
			screenCoords[(i * 6) + 2] = vec3(cx + 0.2, cy - 0.2, -0.5);
			screenCoords[(i * 6) + 3] = vec3(cx + 0.2, cy - 0.2, -0.5);
			screenCoords[(i * 6) + 4] = vec3(cx + 0.2, cy, -0.5);
			screenCoords[(i * 6) + 5] = vec3(cx, cy, -0.5);

			i += 1;
			cx += 0.2;
		}

		cx = -1.0;
		cy -= 0.2;
	}
	

	createBuffers();
}

void Render::updateScene(){

	// Trojuhelniky
	if(scene->modelsCount() != 0){
		GLuint geom_buff;
		glGenBuffers(1, &geom_buff);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, geom_buff);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Scene::Primitive) * scene->totalTriangles(), scene->getModel(0).data, GL_STATIC_DRAW);
	}

	// Octree (uzly + indexy)
	if(octree != nullptr){
		if (octree->getNodesCount() != 0) {
			GLuint nodes_buff;
			glGenBuffers(1, &nodes_buff);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, nodes_buff);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Octree::Node) * octree->getNodesCount(), octree->getNodes(), GL_STATIC_DRAW);
		}

		if(octree->getNodesCount() != 0){
			GLuint indices_buff;
			glGenBuffers(1, &indices_buff);
			glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, indices_buff);
			glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * octree->getIndicesLength(), octree->getIndices(), GL_STATIC_DRAW);
		}
	}

	// Koule
	GLuint sphere_buff;
	glGenBuffers(1, &sphere_buff);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, sphere_buff);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Sphere) * scene->getSpheres().size(), scene->getSpheres().data(), GL_STATIC_DRAW);
}

void Render::setUniforms(float x, float y, float lx, float lz, bool resized, bool light_move, bool* moved, bool walls, int width, int height){

	*moved = false;

	// Jakakoliv zmena sceny
	if (x != last_x || y != last_y || resized || light_move || walls) {
		glClear(GL_COLOR_BUFFER_BIT);
		
		id = 2.0;
		step = stride;
		last_x = x;
		last_y = y;
		*moved = true;
		
		// Zmena barev sten
		if (walls) wallColor = !wallColor;
		
		if (wallColor) {
			scene->setWallColor(Scene::LEFT_WALL, vec3(0.1, 0.6, 0.9));
			scene->setWallColor(Scene::RIGHT_WALL, vec3(0.8, 0.9, 0.1));
		}
		else {
			scene->setWallColor(Scene::LEFT_WALL, vec3(0.9, 0.2, 0.4));
			scene->setWallColor(Scene::RIGHT_WALL, vec3(0.1, 0.9, 0.1));
		}
	}

	// Uniformy
	glUniform1i(glGetUniformLocation(program->getProgram(), "triangles_count"), scene->totalTriangles());
	glUniform1i(glGetUniformLocation(program->getProgram(), "spheres_count"), scene->spheresCount());
	glUniform3f(glGetUniformLocation(program->getProgram(), "view_pos"), (camera->getPosition().x) + x, (camera->getPosition().y) + y, camera->getPosition().z);
	glUniform3f(glGetUniformLocation(program->getProgram(), "light_pos"), (scene->getLight().x) + lx, scene->getLight().y, (scene->getLight().z) + lz);
	glUniform1f(glGetUniformLocation(program->getProgram(), "sample_ID"), id);
	glUniform1f(glGetUniformLocation(program->getProgram(), "stride"), step);
	glUniform1f(glGetUniformLocation(program->getProgram(), "width"), width);
	glUniform1f(glGetUniformLocation(program->getProgram(), "height"), height);

	// Steny
	for (int i = 0; i < 5; i++) {
		std::string s = "cornell_box.walls[" + std::to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().walls[i]);
		s = "cornell_box.colors[" + std::to_string(i) + "]";
		glUniform3f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().colors[i].r, scene->getBox().colors[i].g, scene->getBox().colors[i].b);
		s = "cornell_box.normals[" + std::to_string(i) + "]";
		glUniform3f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().normals[i].x, scene->getBox().normals[i].y, scene->getBox().normals[i].z);
		s = "cornell_box.absorption[" + std::to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().absorption[i]);
		s = "cornell_box.disperse[" + std::to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().disperse[i]);
		s = "cornell_box.reflectivity[" + std::to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), scene->getBox().reflectivity[i]);
	}

	// Nahodna cisla
	for (int i = 0; i < 50; i++) {
		std::string s = "randoms[" + std::to_string(i) + "]";
		glUniform1f(glGetUniformLocation(program->getProgram(), s.c_str()), uniGen(gen));
	}	
	
}

void Render::setStride(int newStride){
	stride = newStride;
}

float Render::getSamples(){
	return id;
}

void Render::draw(){

	id += stride;
	step += stride;

	program->useProgram();

	glBindVertexArray(vao);
	
	// Kresleni snimku po castech
	for (int i = 0; i < 100; i++) {
		glDrawArrays(GL_TRIANGLES, i * 6, 6);
	}
	
	glBindVertexArray(0);
}

void Render::finish(){
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Render::createBuffers(){
	
	// VAO - vertex array object
	glGenBuffers(1, &vao);
	glBindVertexArray(vao);

	// VBO - vertex buffer object
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screenCoords), screenCoords, GL_STATIC_DRAW);

	// Atribut pro shadery - souradnice prumetny
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Odvazani VAO
	glBindVertexArray(0);
}