#ifndef CAMERA_H
#define CAMERA_H

#include "Ray.h"
#include <vecmath.h>
#include <float.h>
#include <cmath>


class Camera
{
public:
	//generate rays for each screen-space coordinate
	virtual Ray generateRay( const Vector2f& point ) = 0 ; 
	
	virtual float getTMin() const = 0 ; 
	virtual ~Camera(){}
protected:
	Vector3f center; 
	Vector3f direction;
	Vector3f up;
	Vector3f horizontal;

};

///TODO: Implement Perspective camera
///Fill in functions and add more fields if necessary
class PerspectiveCamera: public Camera
{
public:
	PerspectiveCamera(const Vector3f& center, const Vector3f& direction,const Vector3f& up , float angle){
		this->center = center;
		this->direction = direction;
		this->up = up;
		this->angle = angle;
		this->horizontal = Vector3f::cross(direction, up).normalized();
		this->w = direction.normalized();
		this->u = Vector3f::cross(w, up).normalized();
		this->v = Vector3f::cross(this->u, this->w).normalized();
	}

	Ray generateRay( const Vector2f& point){
		float x = point[0];
		float y = point[1];
		float alpha = 1.0f / tan(this->angle / 2.0f);
		Vector3f rayDirect = (((this->v*x + this->u*y)) + alpha*this->w).normalized();
		Ray r = Ray(this->center, rayDirect);
		return r;
	}

	float getTMin() const { 
		return 0.0f;
	}

private:
	float angle;
	Vector3f w;
	Vector3f u;
	Vector3f v;
};

#endif //CAMERA_H
