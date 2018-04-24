#include "ClothSystem.h"
#include <vector>
#include <math.h>

//TODO: Initialize here
ClothSystem::ClothSystem(int m, int n, float s)
{
	m_numParticles = m*n;
	length = m;
	width = n;
	spacing = s;
	isWireframe = true;
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			// for this system, we care about the position and the velocity
			//pos
			m_vVecState.push_back(Vector3f(1 + spacing*j, -spacing*i, -2));
			//vel
			m_vVecState.push_back(Vector3f(0, 0, 0));

			vector<vector<int>> springIndex;
			vector<int> structIndex;
			vector<int> shearIndex;
			vector<int> flexIndex;

			/*
			0		1		2		...		n-1
			n		n+1		n+2		...		2n-1
			2n		2n+1	2n+2	...		3n-1
			:
			:
			(m-1)n	(m-1)n+1		...		mn-1
			*/

			bool leftSpring = j - 1 >= 0;
			bool rightSpring = j + 1 < n;
			bool upSpring = i - 1 >= 0;
			bool downSpring = i + 1 < m;

			//structural springs
			if (leftSpring) structIndex.push_back(i*n + j - 1);
			if (rightSpring) structIndex.push_back(i*n + j + 1);
			if (upSpring) structIndex.push_back((i - 1)*n + j);
			if (downSpring) structIndex.push_back((i + 1)*n + j);

			//shear springs
			if (leftSpring & upSpring) shearIndex.push_back((i - 1)*n + j - 1);
			if (rightSpring & upSpring) shearIndex.push_back((i - 1)*n + j + 1);
			if (leftSpring & downSpring) shearIndex.push_back((i + 1)*n + j - 1);
			if (rightSpring & downSpring) shearIndex.push_back((i + 1)*n + j + 1);

			//flex springs
			if (j - 2 >= 0) flexIndex.push_back(i*n + j - 2);
			if (j + 2 < n) flexIndex.push_back(i*n + j + 2);
			if (i - 2 >= 0) flexIndex.push_back((i - 2)*n + j);
			if (i + 2 < m) flexIndex.push_back((i + 2)*n + j);

			springIndex.push_back(structIndex);
			springIndex.push_back(shearIndex);
			springIndex.push_back(flexIndex);
			springIndices.push_back(springIndex);
		}
	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> ClothSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	float mass = 0.5f;
	float g = 10.f;
	float dragK = 2.f;

	for (unsigned i = 0; i < state.size(); i += 2) {
		if (i == 0 || i==(width-1)*2) {
			f.push_back(state[i + 1]);
			Vector3f d = state[i] - Vector3f(state[i].x(), 0, 2);
			Vector3f springF = -1 * d.abs()*d / d.abs();
			f.push_back(springF);
		}
		else {
			//velocity
			f.push_back(state[i + 1]);
			//acceleration
			Vector3f totalF(0.f, 0.f, 0.f);

			totalF += Vector3f(0, -1, 0)*g*mass; //gravity
			totalF += state[i + 1] * -dragK; //drag

			vector<vector<int>> springIndex = springIndices[i / 2];
			/*
			0		1		2		...		n-1
			n		n+1		n+2		...		2n-1
			2n		2n+1	2n+2	...		3n-1
			:
			:
			(m-1)n	(m-1)n+1		...		mn-1
			*/

			vector<int> structIndex = springIndex[0];
			vector<int> shearIndex = springIndex[1];
			vector<int> flexIndex = springIndex[2];
			for (int n = 0; n < structIndex.size(); n++) {
				float rest = spacing;
				float springK = 200.f;
				Vector3f d = state[i] - state[2*structIndex[n]];
				Vector3f springF = -springK * (d.abs() - rest)*d / d.abs();
				totalF += springF;
			}
			for (int n = 0; n < shearIndex.size(); n++) {
				float rest = spacing/sqrt(2);
				float springK = 40.f;
				Vector3f d = state[i] - state[2 * shearIndex[n]];
				Vector3f springF = -springK * (d.abs() - rest)*d / d.abs();
				totalF += springF;
			}
			for (int n = 0; n < flexIndex.size(); n++) {
				float rest = spacing*2;
				float springK = 200.f;
				Vector3f d = state[i] - state[2 * flexIndex[n]];
				Vector3f springF = -springK * (d.abs() - rest)*d / d.abs();
				totalF += springF;
			}

			f.push_back(totalF);
		}
	}

	return f;
}

///TODO: render the system (ie draw the particles)
void ClothSystem::draw()
{
	//for (int i = 0; i < m_numParticles; i++) {
	//	Vector3f pos = m_vVecState[i * 2];//  position of particle i. YOUR CODE HERE
	//	glPushMatrix();
	//	glTranslatef(pos[0], pos[1], pos[2]);
	//	glutSolidSphere(0.075f, 10.0f, 10.0f);
	//	glPopMatrix();
	//}

	if (!isWireframe) {
		vector<Vector3f> vertexNormals;
		for (int i = 0; i < length; i++) {
			for (int j = 0; j < width; j++) {
				//smooth shading
				Vector3f up = m_vVecState[(i*width + j) * 2];
				Vector3f down = m_vVecState[(i*width + j) * 2];
				Vector3f left = m_vVecState[(i*width + j) * 2];
				Vector3f right = m_vVecState[(i*width + j) * 2];
				if (i>0) up = m_vVecState[((i - 1)*width + j) * 2];
				if (i<length-1) down = m_vVecState[((i + 1)*width + j) * 2];
				if (j>0) left = m_vVecState[(i*width + j-1) * 2];
				if (j<width-1) right = m_vVecState[(i*width +j+1) * 2];
				Vector3f normal = Vector3f::cross(up - down, left - right).normalized();
				vertexNormals.push_back(normal);
			}
		}
		for (int i = 0; i < length - 1; i++) {
			for (int j = 0; j < width - 1; j++) {

				Vector3f v11 = m_vVecState[(i*width + j) * 2];
				Vector3f v12 = m_vVecState[((i + 1)*width + j) * 2];
				Vector3f v13 = m_vVecState[((i + 1)*width + j + 1) * 2];
				Vector3f nUL = vertexNormals[i*width + j];
				Vector3f nLL = vertexNormals[(i + 1)*width + j];
				Vector3f nLR = vertexNormals[(i + 1)*width + j + 1];
				Vector3f nUR = vertexNormals[i*width + j + 1];

				Vector3f n1 = Vector3f::cross(v12 - v11, v13 - v11).normalized();
				Vector3f n2 = n1*-1;

				glBegin(GL_TRIANGLES);
				//glNormal3f(n1[0], n1[1], n1[2]);
				glNormal3f(nUL[0], nUL[1], nUL[2]);
				glVertex3f(v11[0], v11[1], v11[2]);
				glNormal3f(nLL[0], nLL[1], nLL[2]);
				glVertex3f(v12[0], v12[1], v12[2]);
				glNormal3f(nLR[0], nLR[1], nLR[2]);
				glVertex3f(v13[0], v13[1], v13[2]);

				glNormal3f(n2[0], n2[1], n2[2]);
				glVertex3f(v13[0], v13[1], v13[2]);
				glVertex3f(v12[0], v12[1], v12[2]);
				glVertex3f(v11[0], v11[1], v11[2]);


				Vector3f v21 = m_vVecState[(i*width + j) * 2];
				Vector3f v22 = m_vVecState[((i + 1)*width + j + 1) * 2];
				Vector3f v23 = m_vVecState[(i*width + j + 1) * 2];

				//glNormal3f(n1[0], n1[1], n1[2]);
				glNormal3f(nUL[0], nUL[1], nUL[2]);
				glVertex3f(v21[0], v21[1], v21[2]);
				glNormal3f(nUR[0], nUR[1], nUR[2]);
				glVertex3f(v22[0], v22[1], v22[2]);
				glNormal3f(nLR[0], nLR[1], nLR[2]);
				glVertex3f(v23[0], v23[1], v23[2]);

				glNormal3f(n2[0], n2[1], n2[2]);
				glVertex3f(v23[0], v23[1], v23[2]);
				glVertex3f(v22[0], v22[1], v22[2]);
				glVertex3f(v21[0], v21[1], v21[2]);
				glEnd();


			}
		}
	}
	else {
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glDisable(GL_LIGHTING);
		glLineWidth(2);
		for (int i = 0; i < length; i++) {
			for (int j = 0; j < width; j++) {
				Vector3f v1 = m_vVecState[(i*width + j) * 2];
				if (i < length - 1) {
					Vector3f v2 = m_vVecState[((i + 1)*width + j) * 2];
					glBegin(GL_LINES);
					glColor4f(1, 1, 1, 1);
					glVertex3f(v1[0], v1[1], v1[2]);
					glVertex3f(v2[0], v2[1], v2[2]);
					glEnd();
				}
				if (j < width - 1) {
					Vector3f v3 = m_vVecState[(i*width + j + 1) * 2];
					glBegin(GL_LINES);
					glColor4f(1, 1, 1, 1);
					glVertex3f(v1[0], v1[1], v1[2]);
					glVertex3f(v3[0], v3[1], v3[2]);
					glEnd();
				}
			}
		}
		glPopAttrib();
	}
}

void ClothSystem::toggleWireframe() {
	isWireframe = !isWireframe;
}