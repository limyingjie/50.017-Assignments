#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "Object3D.h"
///TODO implement this class
///So that the intersect function first transforms the ray
///Add more fields as necessary
class Transform: public Object3D
{
public: 
	Transform(){}
	Transform( const Matrix4f& m, Object3D* obj ):o(obj){
		this->m = m;
		this->o = obj;
	}
	~Transform(){
	}
	virtual bool intersect( const Ray& r , Hit& h , float tmin){
		Vector3f transRo = (m.inverse() * Vector4f(r.getOrigin(), 1.0f)).xyz();
		Vector3f transRd = (m.inverse() * Vector4f(r.getDirection(), 0.0f)).xyz();

		Ray transRay = Ray(transRo, transRd);
		if (o->intersect(transRay, h, tmin)) {
			Vector3f transNorm = (m.inverse().transposed()*Vector4f(h.getNormal(), 0.0f)).normalized().xyz();
			h.set(h.getT(), h.getMaterial(), transNorm);
			return true;
		}
		return false;
	}

protected:
	Object3D* o; //un-transformed object	
	Matrix4f m;
};

#endif //TRANSFORM_H
