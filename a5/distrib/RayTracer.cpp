#include "RayTracer.h"
#include "Camera.h"
#include "Ray.h"
#include "Hit.h"
#include "Group.h"
#include "Material.h"
#include "Light.h"

#define EPSILON 0.001

//IMPLEMENT THESE FUNCTIONS
//These function definitions are mere suggestions. Change them as you like.
Vector3f mirrorDirection( const Vector3f& normal, const Vector3f& incoming)
{
	return incoming - 2 * Vector3f::dot(incoming, normal)*normal;
}

bool transmittedDirection( const Vector3f& normal, const Vector3f& incoming, 
        float index_n, float index_nt, 
        Vector3f& transmitted)
{	
	float dDotN = Vector3f::dot(incoming, normal);
	float det = (1 - (index_n*index_n * (1 - dDotN*dDotN) / index_nt / index_nt));
	if (det <= 0) return false;

	transmitted = (index_n * (incoming - normal*dDotN) / index_nt) 
		- normal * sqrtf(det);

	return true;
}

RayTracer::RayTracer( SceneParser * scene, int max_bounces 
  //more arguments if you need...
                      ) :
  m_scene(scene),m_maxBounces(max_bounces)

{
//CHANGE: added identifier group*
  Group* g=scene->getGroup();
  m_maxBounces = max_bounces;
}

RayTracer::~RayTracer()
{
}

Vector3f RayTracer::traceRay( Ray& ray, float tmin, int bounces,
        float refr_index, Hit& hit ) const
{
    hit = Hit( FLT_MAX, NULL, Vector3f( 0, 0, 0 ) );

	if (m_scene->getGroup()->intersect(ray, hit, m_scene->getCamera()->getTMin())) {
		Vector3f pixCol = Vector3f(0.0f, 0.0f, 0.0f);
		Vector3f intersectCoord = ray.getOrigin() + ray.getDirection()*hit.getT();

		//Diffuse + Specular color
		for (int indL = 0; indL < m_scene->getNumLights(); indL++) {
			Light* light = m_scene->getLight(indL);
			Vector3f lightDirection;
			Vector3f litCol;
			float distToLight;

			light->getIllumination(ray.pointAtParameter(hit.getT()), lightDirection, litCol, distToLight);

			//Shadows
			
			Ray ray2(intersectCoord+lightDirection*EPSILON, lightDirection);
			Hit hit2(distToLight, NULL, NULL);

			if (!m_scene->getGroup()->intersect(ray2, hit2, tmin)) {
				Vector3f shadingCol = hit.getMaterial()->Shade(ray, hit, lightDirection, litCol);
				pixCol = pixCol + shadingCol;
			}

		}
		//Ambient color
		pixCol = pixCol + hit.getMaterial()->getDiffuseColor()*m_scene->getAmbientLight();	

		if (bounces > 0) {
			//Reflect
			Vector3f reflectDirection = mirrorDirection(hit.getNormal().normalized(), ray.getDirection());
			Ray ray3(intersectCoord + reflectDirection*EPSILON, reflectDirection);
			Hit hit3(FLT_MAX, NULL, Vector3f(0, 0, 0));
			Vector3f reflectColor = traceRay(ray3, 0, bounces - 1, refr_index, hit3);
			//pixCol = pixCol + reflectColor*hit.getMaterial()->getSpecularColor();

			//Refract
			float refr_index_new = hit.getMaterial()->getRefractionIndex();
			Vector3f normal = hit.getNormal().normalized();
			if (Vector3f::dot(ray.getDirection(), normal) > 0) {
				refr_index_new = 1.0f;
				normal = normal*-1.0f;
			}
			Vector3f refractDirection(0, 0, 0);
			bool isRefracted = transmittedDirection(normal, ray.getDirection(),
				refr_index, refr_index_new, refractDirection);
			if (isRefracted) {
				Ray ray4(intersectCoord + refractDirection*EPSILON, refractDirection);
				Hit hit4(FLT_MAX, NULL, Vector3f(0, 0, 0));
				Vector3f refractColor = traceRay(ray4, 0, bounces - 1, refr_index_new, hit4);

				float c;
				if (refr_index <= refr_index_new) {
					c = abs(Vector3f::dot(ray.getDirection(), normal));
				}
				else {
					c = abs(Vector3f::dot(refractDirection, normal));
				}
				float R_0 = pow((refr_index_new - refr_index) / (refr_index_new + refr_index), 2);
				float R = R_0 + (1 - R_0)*pow(1 - c, 5);

				pixCol = pixCol + (1 - R)*hit.getMaterial()->getSpecularColor()*refractColor + (R)*reflectColor*hit.getMaterial()->getSpecularColor();
			}
			else {
				pixCol = pixCol + reflectColor*hit.getMaterial()->getSpecularColor();
			}


		}

		return pixCol;
	}
    else return m_scene->getBackgroundColor(ray.getDirection());
}
