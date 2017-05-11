/*
 * Path tracing na GPU
 * Bakalarska prace
 * David Novak, xnovak1l
 * FIT VUT Brno, 2016
 *
 * Octree.cpp - trida pro praci s oktalovym stromem
 */


#include "Octree.h"

Octree::Octree(){
}

void Octree::build(Scene::Model md, int totalTriangles, int primitivesPerList){
	
	triangles_count = totalTriangles;

	std::cout << "Building Octree..." << std::endl;

	initOctree();
	buildOctree(md, primitivesPerList);
	linkOctree();

	std::cout << "Octree builded." << std::endl;
}


void Octree::build(Scene::Model* md, int modelCount, int totalTriangles, int primitivesPerList) {

	triangles_count = totalTriangles;
	std::cout << "Building Octree..." << std::endl;

	// Zpracovani vsech modelu
	for (int i = 0; i < modelCount; i++) {

		mod = md[i];
		int lf = 0;

		initOctree();
		buildOctree(mod, primitivesPerList);
		linkOctree();

	}

	std::cout << "Octree builded." << std::endl;
}

int * Octree::getIndices() {
	return indices.data();
}

Octree::Node * Octree::getNodes() {
	return nodes.data();
}

int Octree::getIndicesLength() {
	return indices.size();
}

int Octree::getNodesCount() {
	return nodes.size();
}

void Octree::initOctree() {
	
	Node n;
	
	n.index = n.count = 0;
	n.leaf = 1;
	n.start = vec4(LEFT, DOWN, BACK, 1.0f);
	n.end = vec4(RIGHT, UP, FRONT, 1.0f);
	
	for (int i = 0; i < 8; i++)
		n.childs[i] = -1;

	nodes.push_back(n);

	std::vector<int> c;
	tmp_indices.push_back(c);

	nodes_count = 1;
}

void Octree::buildOctree(Scene::Model md, int primitivesPerList) {

	makeChilds(0);

	for (int i = 0; i < triangles_count; i++) {
		tmp_indices.at(0).push_back(i);
	}

	// Vsechny uzly Octree
	for (int i = 0; i < nodes.size(); i++) {
		
		// Listove uzly
		if (nodes.at(i).leaf) {

			// Vsechny trojuhelniky
			for (int j = 0; j < tmp_indices.at(nodes.at(i).gap).size(); j++) {
			
				int a = tmp_indices.at(nodes.at(i).gap).at(j);

				if (trinagleAABBIntersection(vec3(md.data[a].vertex0), vec3(md.data[a].vertex1), vec3(md.data[a].vertex2), vec3(nodes.at(i).start), vec3(nodes.at(i).end))) {
					tmp_indices.at(i).push_back(a);
				}
			}

			}

			// Expanze uzlu
			if (tmp_indices.at(i).size() > primitivesPerList && nodes.size() < MAX_NODES) {
				makeChilds(i);
			}

		}

}

bool Octree::isInBox(vec3 point, vec3 startBox, vec3 endBox){
	if (point.x >= startBox.x && point.y >= startBox.y && point.z <= startBox.z) 
		if(point.x <= endBox.x && point.y <= endBox.y && point.z >= endBox.z)
			return true;
	
	return false;
}

void Octree::makeChilds(int index){
	
	vec4 half = 0.5f * (nodes.at(index).start + nodes.at(index).end);

	Node news[8];

	// Nastaveni hodnot vsem childum
	for(int a = 0; a < 8; a++){
		for(int i = 0; i < 8; i++){
			news[a].childs[i] = -1;
			news[a].gap = index;
		}
		
		news[a].index = 0;
		news[a].count = 0;
		news[a].leaf = 1;
	}

	// Pozice vsech child uzlu

	vec4 start = nodes.at(index).start;
	vec4 end = nodes.at(index).end;

	news[0].start = vec4(start.x, start.y, start.z, 1.0f);
	news[0].end = vec4(half.x, half.y, half.z, 1.0f);

	news[1].start = vec4(start.x, start.y, half.z, 1.0f);
	news[1].end = vec4(half.x, half.y, end.z, 1.0f);

	news[2].start = vec4(start.x, half.y, start.z, 1.0f);
	news[2].end = vec4(half.x, end.y, half.z, 1.0f);

	news[3].start = vec4(start.x, half.y, half.z, 1.0f);
	news[3].end = vec4(half.x, end.y, end.z, 1.0f);

	news[4].start = vec4(half.x, start.y, start.z, 1.0f);
	news[4].end = vec4(end.x, half.y, half.z, 1.0f);

	news[5].start = vec4(half.x, start.y, half.z, 1.0f);
	news[5].end = vec4(end.x, half.y, end.z, 1.0f);

	news[6].start = vec4(half.x, half.y, start.z, 1.0f);
	news[6].end = vec4(end.x, end.y, half.z, 1.0f);

	news[7].start = vec4(half.x, half.y, half.z, 1.0f);
	news[7].end = vec4(end.x, end.y, end.z, 1.0f);

	// Zacleneni uzlu do stromu
	for (int a = 0; a < 8; a++) {
		std::vector<int> v;
		tmp_indices.push_back(v);
		nodes.push_back(news[a]);
		nodes.at(index).childs[a] = nodes_count + a;
	}

	// Oznaceni uzlu za neterminalni
	nodes.at(index).leaf = 0;

	nodes_count = nodes.size();
}

bool Octree::trinagleAABBIntersection(vec3 v0, vec3 v1, vec3 v2, vec3 boxStart, vec3 boxEnd){
	

	// Nejaky vrchol trojuhelniku lezi v AABB
	
	if (isInBox(v0, boxStart, boxEnd)) return true;

	if (isInBox(v1, boxStart, boxEnd)) return true;

	if (isInBox(v2, boxStart, boxEnd)) return true;


	float max_x = max(v0.x, max(v1.x, v2.x));
	float min_x = min(v0.x, min(v1.x, v2.x));
	float max_y = max(v0.y, max(v1.y, v2.y));
	float min_y = min(v0.y, min(v1.y, v2.y));
	float max_z = max(v0.z, max(v1.z, v2.z));
	float min_z = min(v0.z, min(v1.z, v2.z));


	// Trojuhelnik prochazi horni stenou AABB
	if (v0.y >= boxEnd.y || v1.y >= boxEnd.y || v2.y >= boxEnd.y) {
		if (v0.y <= boxEnd.y || v1.y <= boxEnd.y || v2.y <= boxEnd.y) {
			if (boxStart.x <= max_x && boxEnd.x >= min_x) {
				if (boxStart.z >= min_z && boxEnd.z <= max_z) {
					return true;
				}
			}

		}
	}

	// Trojuhelnik prochazi pravou stenou AABB
	if (v0.x >= boxEnd.x || v1.x >= boxEnd.x || v2.x >= boxEnd.x) {
		if (v0.x <= boxEnd.x || v1.x <= boxEnd.x || v2.x <= boxEnd.x) {
			if (boxStart.y <= max_y && boxEnd.y >= min_y) {
				if (boxStart.z >= min_z && boxEnd.z <= max_z) {
					return true;
				}
			}

		}
	}

	// Trojuhelnik prochazi zadni stenou AABB
	if (v0.z >= boxEnd.z || v1.z >= boxEnd.z || v2.z >= boxEnd.z) {
		if (v0.z <= boxEnd.z || v1.z <= boxEnd.z || v2.z <= boxEnd.z) {
			if (boxStart.x <= max_x && boxEnd.x >= min_x) {
				if (boxStart.y <= max_y && boxEnd.y >= min_y) {
					return true;
				}
			}

		}
	}

	return false;
}

void Octree::linkOctree(){
	int current_ptr = 0;

	for (int i = 0; i < nodes.size(); i++) {
		
		// Budou se ukladat pouze indexy z listovych uzlu
		if (nodes.at(i).leaf) {
			nodes.at(i).index = current_ptr;
			nodes.at(i).count = tmp_indices.at(i).size();
			current_ptr += tmp_indices.at(i).size();
			
			for(int j = 0; j < tmp_indices.at(i).size(); j++){
				indices.push_back(tmp_indices.at(i).at(j));
			}
		}
	}

	// Odstraneni vsech indexu
	tmp_indices.clear();
}