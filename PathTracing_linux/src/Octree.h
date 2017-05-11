/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Octree.h - trida pro praci s oktalovym stromem
 */

#pragma once

#include "Scene.h"

#define LEFT -0.7		// Leva okraj octree
#define RIGHT 0.7		// Pravy okraj octree
#define UP 0.7			// Horni okraj octree
#define DOWN -0.65		// Dolni okraj octree
#define FRONT -0.9		// Predni okraj octree
#define BACK 0.9		// Zadni okraj octree

#define MAX_NODES 3 * triangles_count		// Maximalni pocet uzlu octree


class Octree {

public:

	// Uzel octree
	typedef struct node {
		int childs[8];
		int index;
		int count;
		int leaf;
		int gap;
		vec4 start;
		vec4 end;
	} Node;

	Octree();
	
	// Sestaveni octree
	void build(Scene::Model md, int totalTriangles, int primitivesPerList);
	void build(Scene::Model* md, int modelCount, int totalTriangles, int primitivesPerList);
	
	int* getIndices();
	Node* getNodes();
	int getIndicesLength();
	int getNodesCount();

private:

	// Funkce pro sestaveni octree
	void initOctree();
	void buildOctree(Scene::Model md, int primitivesPerList);
	bool isInBox(vec3 point, vec3 startBox, vec3 endBox);
	void makeChilds(int index);
	bool trinagleAABBIntersection(vec3 v0, vec3 v1, vec3 v2, vec3 boxStart, vec3 boxEnd);
	void linkOctree();

	std::vector<Node> nodes;
	std::vector<std::vector<int>> tmp_indices;
	std::vector<int> indices;
	int nodes_count;
	int triangles_count;

	Scene::Model mod;
};