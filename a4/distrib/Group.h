#ifndef GROUP_H
#define GROUP_H


#include "Object3D.h"
#include "Ray.h"
#include "Hit.h"
#include <iostream>
#include <vector>

using  namespace std;

///TODO: 
///Implement Group
///Add data structure to store a list of Object* 
class Group:public Object3D
{
public:

	Group(){

	}
	
	Group( int num_objects ){
		objects = vector<Object3D *>(num_objects);
	}

	~Group(){
   
	}

	bool intersect( const Ray& r , Hit& h , float tmin ) {
		bool intersect = false;
		for (int i = 0; i < objects.size(); i++) {
			if (objects[i]->intersect(r, h, tmin)) intersect = true;
		}
		return intersect;
	}
	
	void addObject( int index , Object3D* obj ){
		objects[index] = obj;
	}

	int getGroupSize(){ 
		return objects.size();
	}

	private:
		vector<Object3D *> objects;
};

#endif
	
