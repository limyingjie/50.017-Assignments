#include "Sphere.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;

Sphere::Sphere(Vector3f center, float radius, Material* material) :Object3D(material) {
	this->center = center;
	this->radius = radius;
	this->material = material;
}

bool Sphere::intersect(const Ray& r, Hit& h, float tmin) {
	Vector3f Ro = r.getOrigin();
	Vector3f Rd = r.getDirection();

	//adjust for perspective
	Ro = Ro - center;

	float a = Vector3f::dot(Rd, Rd); // since ||Rd|| = 1
	float b = Vector3f::dot(Rd, Ro) * 2;
	float c = Vector3f::dot(Ro, Ro) - radius*radius;
	float d = (b*b - 4 * a*c);

	//cerr << a << "," << b << "," << c << "," << d << endl;

	if (d < 0) {
		return false; // quadratic eqn unsolvable, ray misses sphere
	}
	else {
		d = sqrt(d);
	}

	float t1 = (-b - d) / 2 / a;
	float t2 = (-b + d) / 2 / a;

	//cerr << t1 << "," << t2 << endl;

	if (t1 >= tmin && t1 <= h.getT()) {
		h.set(t1, material, (Ro + t1*Rd).normalized());
		//cerr << "HIT" << "," << t1 << endl;
		return true;
	}
	else if (t2 >= tmin && t2 <= h.getT()) {
		h.set(t2, material, (Ro + t2*Rd).normalized());
		//cerr << "HIT" << "," << t2 << endl;
		return true;
	}
	else {
		return false;
	}
}