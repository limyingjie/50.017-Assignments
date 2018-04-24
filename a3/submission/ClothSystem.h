#ifndef CLOTHSYSTEM_H
#define CLOTHSYSTEM_H

#include <vecmath.h>
#include <vector>
#ifdef _WIN32
#include "GL/freeglut.h"
#else
#include <GL/glut.h>
#endif

#include "particleSystem.h"

class ClothSystem: public ParticleSystem
{
///ADD MORE FUNCTION AND FIELDS HERE
public:
	ClothSystem(int m, int n, float spacing);
	vector<Vector3f> evalF(vector<Vector3f> state);
	
	void draw();
	void toggleWireframe();

private:
	int length;
	int width;
	float spacing;
	vector<vector<vector<int>>> springIndices;
	bool isWireframe;

};


#endif
