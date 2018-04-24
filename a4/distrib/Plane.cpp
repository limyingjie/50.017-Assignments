#include "Plane.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;

Plane:: Plane(const Vector3f& normal, float d, Material* m) :Object3D(m) {
	this->normal = normal.normalized();
	this->d = d;
	this->material = m;
}

bool Plane::intersect(const Ray& r, Hit& h, float tmin) {
	Vector3f Ro = r.getOrigin();
	Vector3f Rd = r.getDirection();

	if (Vector3f::dot(normal, Rd) == 0) return false;

	float t = -(-d + Vector3f::dot(normal, Ro)) / Vector3f::dot(normal, Rd);
	if (t >= tmin && t <= h.getT()) {
		h.set(t, material, normal);
		//cerr << "HIT" << "," << t << endl;
		return true;
	}

	return false;
}