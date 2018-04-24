
#include "simpleSystem.h"

using namespace std;

SimpleSystem::SimpleSystem()
{
	m_numParticles = 4;
	m_vVecState.push_back(Vector3f(1, 0, 0));
	m_vVecState.push_back(Vector3f(-1, 0, 0));
	m_vVecState.push_back(Vector3f(0, 1, 0));
	m_vVecState.push_back(Vector3f(0, -1, 0));
}

// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> SimpleSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	// YOUR CODE HERE
	for (unsigned i = 0; i < state.size(); i++) {
		f.push_back(Vector3f(-state[i].y(), state[i].x(), 0));
	}

	return f;
}

// render the system (ie draw the particles)
void SimpleSystem::draw()
{

	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = m_vVecState[i];//YOUR PARTICLE POSITION
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2]);
		glutSolidSphere(0.075f, 10.0f, 10.0f);
		glPopMatrix();
	}
}
