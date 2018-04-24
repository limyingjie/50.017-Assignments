#include "Triangle.h"
#include <vecmath.h>
#include <cmath>

#include <iostream>
using namespace std;

Triangle::Triangle(const Vector3f& a, const Vector3f& b, const Vector3f& c, Material* m) :Object3D(m) {
	hasTex = false;
	this->a = a;
	this->b = b;
	this->c = c;
}

bool Triangle::intersect(const Ray& ray, Hit& hit, float tmin) {
	Vector3f Ro = ray.getOrigin();
	Vector3f Rd = ray.getDirection();

	Matrix3f A(a.x() - b.x(), a.x() - c.x(), Rd.x(),
		a.y() - b.y(), a.y() - c.y(), Rd.y(),
		a.z() - b.z(), a.z() - c.z(), Rd.z());
	float detA = A.determinant();

	Matrix3f MatBeta(a.x() - Ro.x(), a.x() - c.x(), Rd.x(),
		a.y() - Ro.y(), a.y() - c.y(), Rd.y(),
		a.z() - Ro.z(), a.z() - c.z(), Rd.z());
	float beta = MatBeta.determinant() / detA;

	Matrix3f MatGamma(a.x() - b.x(), a.x() - Ro.x(), Rd.x(),
		a.y() - b.y(), a.y() - Ro.y(), Rd.y(),
		a.z() - b.z(), a.z() - Ro.z(), Rd.z());
	float gamma = MatGamma.determinant() / detA;

	Matrix3f MatT(a.x() - b.x(), a.x() - c.x(), a.x() - Ro.x(),
		a.y() - b.y(), a.y() - c.y(), a.y() - Ro.y(),
		a.z() - b.z(), a.z() - c.z(), a.z() - Ro.z());
	float t = MatT.determinant() / detA;

	if (beta < 0 || gamma < 0 || beta + gamma > 1) return false;

	if (t > tmin && t < hit.getT()) {

		float alpha = 1.0f - beta - gamma;

		//interpolate the normals/textures

		Vector3f normal = (alpha*this->normals[0] + beta*this->normals[1] + gamma*this->normals[2]).normalized();
		hit.set(t, this->material, normal);

		Vector2f texCoord = (alpha*this->texCoords[0] + beta*this->texCoords[1] + gamma*this->texCoords[2]);
		hit.setTexCoord(texCoord);

		return true;
	}

	return false;
}