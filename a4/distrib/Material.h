#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "Ray.h"
#include "Hit.h"
#include "texture.hpp"
///TODO:
///Implement Shade function that uses ambient, diffuse, specular and texture
class Material
{
public:
	
	Material( const Vector3f& d_color ,const Vector3f& s_color=Vector3f::ZERO, float s=0):
	diffuseColor( d_color),specularColor(s_color), shininess(s)
	{
		diffuseColor = d_color;
		specularColor = s_color;
		shininess = s;
	}

	virtual ~Material()
	{

	}

	virtual Vector3f getDiffuseColor() const 
	{ 
		return  diffuseColor;
	}
    

	Vector3f Shade( const Ray& ray, const Hit& hit,
					const Vector3f& dirToLight, const Vector3f& lightColor ) {
		Vector3f n = hit.getNormal().normalized();
		Vector3f phongDiffuse;
		Vector3f phongSpecular;

		//Diffuse Shading w/ clamping

		float NdotL = Vector3f::dot(n, dirToLight);
		if (NdotL < 0){
			NdotL = 0;
		}
	
		if (t.valid()){
			Vector3f texColor = t(hit.texCoord[0], hit.texCoord[1]);
			phongDiffuse = NdotL*texColor*lightColor;
			diffuseColor = texColor;
		}
		else{
			phongDiffuse = NdotL*diffuseColor*lightColor;
		}

		//Phong Shading w/ clamping
		Vector3f v = ray.getDirection()*-1.0f;
		Vector3f r = ((dirToLight*-1.0f) + 2.0f*n*(NdotL)).normalized();

		float VdotR = Vector3f::dot(v, r);
		if (VdotR < 0){
			VdotR = 0;
		}

		phongSpecular = specularColor*pow(VdotR, shininess)*lightColor;

		return phongDiffuse + phongSpecular;
	}

	void loadTexture(const char * filename){
		t.load(filename);
	}
protected:
	Vector3f diffuseColor;
	Vector3f specularColor;
	float shininess;
	Texture t;
};



#endif // MATERIAL_H
