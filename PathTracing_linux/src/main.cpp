/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * main.cpp - spousteci modul aplikace
 */

#include "Window.h"
#include "Shader.h"
#include "Render.h"
#include "Camera.h"
#include "FPSMeter.h"
#include "Octree.h"

#define GLEW_STATIC

#define DEFAULT_WIDTH 600
#define DEFAULT_HEIGHT 600

#define SPHERE_SCENE 1
#define BUNNY_SCENE 2
#define DEER_SCENE 3
#define TELESCOPE_SCENE 4
#define DRAGON_SCENE 5

#define LIST 1
#define OCTREE 2

char title[100];
bool moved;

// Uvodni hlaska aplikace
void print_intro() {
	std::cout << "Path Tracing app" << std::endl;
	std::cout << "Choose scene:" << std::endl;
	std::cout << "1 - spheres in Cornell box" << std::endl;
	std::cout << "2 - bunny (336 faces) in Cornell box" << std::endl;
	std::cout << "3 - deer (1 503 faces) in Cornell box" << std::endl;
	std::cout << "4 - NASA telescope (13 812 faces) in Cornell box" << std::endl;
	std::cout << "5 - dragon (100 000 faces) in Cornell box (warning: long loading)" << std::endl;
}

// Vyber datove struktury
void choose_ds() {
	std::cout << "Data structure: " << std::endl;
	std::cout << "1 - list" << std::endl;
	std::cout << "2 - octree" << std::endl;
}

int main() {
	
	print_intro();
	int sc;
	int data_st;
	std::cin >> sc;

	Scene cornell(vec3(0.0, 0.5, -0.2));
	
	// Volba sceny
	if(sc == SPHERE_SCENE) {
		Sphere r(0.032, vec3(-0.15, 0.07, -0.45), Sphere::Material{ 1.0f, 0.4f, 1.0f, vec3(0.99f) });
		Sphere s(0.032, vec3(-0.15, -0.28, -0.45), Sphere::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		Sphere p(0.052, vec3(0.27, -0.22, 0.02), Sphere::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		Sphere q(0.042, vec3(-0.42, -0.24, -0.04), Sphere::Material{ 1.0f, 0.0f, 1.0f, vec3(0.99f) });
		
		cornell.addSphere(s);
		cornell.addSphere(p);
		cornell.addSphere(q);
		cornell.addSphere(r);

		data_st = LIST;
	}

	else if (sc == BUNNY_SCENE) {
		
		ModelLoader ml("model/bunny.obj", ModelLoader::VERTEX_NORMALS, ModelLoader::Material{0.9f, 0.0f, 0.0f, vec3(0.99f)});
		cornell.addModel(ml);
		cornell.scaleObject(0, vec3(0.12f));
		cornell.translateObject(0, vec3(0.2f, -0.58f, -0.05f));

		Sphere q(0.042, vec3(-0.42, -0.24, -0.04), Sphere::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		cornell.addSphere(q);

		choose_ds();
		std::cin >> data_st;
	}

	else if(sc == DEER_SCENE) {	
		ModelLoader ml("model/deer.obj", ModelLoader::VERTEX_UVS_NORMALS, ModelLoader::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		cornell.addModel(ml);
		cornell.scaleObject(0, vec3(0.022f));
		cornell.translateObject(0, vec3(0.2f, -0.4f, 0.1f));

		Sphere q(0.042, vec3(-0.42, -0.25, -0.04), Sphere::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		cornell.addSphere(q);	

		choose_ds();
		std::cin >> data_st;
	}

	else if (sc == TELESCOPE_SCENE) {
		ModelLoader ml("model/space.obj", ModelLoader::VERTEX_NORMALS, ModelLoader::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		cornell.addModel(ml);
		cornell.scaleObject(0, vec3(0.11f));
		cornell.translateObject(0, vec3(0.0f, -0.23f, 0.11f));
		
		choose_ds();
		std::cin >> data_st;
	}

	else if (sc == DRAGON_SCENE) {
		ModelLoader ml("model/dragon.obj", ModelLoader::VERTEX_NORMALS, ModelLoader::Material{ 0.9f, 0.0f, 0.0f, vec3(0.99f) });
		cornell.addModel(ml);
		cornell.scaleObject(0, vec3(0.28f));
		cornell.translateObject(0, vec3(-0.06f, -0.43f, -0.41f));

		choose_ds();
		std::cin >> data_st;
	}

	else {
		return 1;
	}

	Window window(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Path Tracing", true);
	FPSMeter fps;
	Shader path;
	Camera camera(vec3(0.0, 0.0, 2.0), DEFAULT_WIDTH, DEFAULT_HEIGHT);
	Render* tracer;
	Octree oct;

	// Priprava datove struktury
	if(data_st == LIST){
		Render tr(&path, &cornell, &camera, nullptr);
		tracer = &tr;
	}
	else if(data_st == OCTREE) {
		oct.build(cornell.getModels().data(), cornell.modelsCount(), cornell.totalTriangles(), 15);
		Render tr(&path, &cornell, &camera, &oct);
		tracer = &tr;
	}
	else {
		return 1;
	}

	// Priprava a kompilace shaderu
	path.attachShader(Shader::VERTEX, path.loadShader("src/shaders/path_tracing.vert"), GL_TRUE);
	if(data_st == LIST) path.attachShader(Shader::FRAGMENT, path.loadShader("src/shaders/path_tracing_list.frag"), GL_TRUE);
	else if(data_st == OCTREE) path.attachShader(Shader::FRAGMENT, path.loadShader("src/shaders/path_tracing_octree.frag"), GL_TRUE);
	path.compileProgram(GL_TRUE);

	// Nastaveni sceny
	cornell.setWallColor(Scene::LEFT_WALL, vec3(0.1, 0.6, 0.9));
	cornell.setWallColor(Scene::RIGHT_WALL, vec3(0.8, 0.9, 0.1));
	tracer->updateScene();

	
	// Hlavni smycka
	while (!window.getCloseState()) {

		tracer->setUniforms(window.getXOffset(), window.getYOffset(), window.getLXOffset(), window.getLZOffset(), window.isResized(), window.lightMove(), &moved, window.wallsColor(), window.getCurrentWidth(), window.getCurrentHeight());

		if (moved) fps.refreshTotalTime();

		fps.startTask();

		tracer->draw();

		fps.stopTask();
		fps.gainFPS();

		sprintf(title, "Path Tracing  Rendering Time: %.2f ms  Samples: %d  FPS: %.2f  Total: %.2f s", fps.getRenderingTime(), (int)tracer->getSamples(), fps.getFPS(), fps.getTotalTime());
		window.setTitle(title);

		window.swapBuffers();
	}

	// Uvolneni objektu
	tracer->finish();
	window.closeWindow();
	
	return 0;
}
