
#include "pendulumSystem.h"

PendulumSystem::PendulumSystem(int numParticles):ParticleSystem(numParticles)
{
	m_numParticles = numParticles;
	
	// fill in code for initializing the state based on the number of particles
	for (int i = 0; i < m_numParticles; i++) {
		
		// for this system, we care about the position and the velocity
		//pos
		m_vVecState.push_back(Vector3f(1+0.5*i, -0.5*i, 0));
		//vel
		m_vVecState.push_back(Vector3f(0, 0, 0));

	}
}


// TODO: implement evalF
// for a given state, evaluate f(X,t)
vector<Vector3f> PendulumSystem::evalF(vector<Vector3f> state)
{
	vector<Vector3f> f;

	//float mass = 1.f;
	float g = 10.f;
	float dragK = 2.f;
	float springK = 200.f;
	float rest = 0.8f;

	// YOUR CODE HERE
	for (unsigned i = 0; i < state.size(); i+=2) {
		if (i == 0) {
			f.push_back(Vector3f());
			f.push_back(Vector3f());
		}
		else {
			//velocity
			f.push_back(state[i + 1]);
			//acceleration
			Vector3f totalF(0.f, 0.f, 0.f);

			totalF += Vector3f(0, -1, 0)*g; //gravity
			totalF += state[i + 1] * -dragK; //drag

			Vector3f d = state[i] - state[i - 2];
			Vector3f springF = -springK * (d.abs() - rest)*d / d.abs();
			state[i - 1] -= springF; //Newton's 3rd law
			totalF += springF;

			f.push_back(totalF);
		}
	}

	return f;
}

// render the system (ie draw the particles)
void PendulumSystem::draw()
{
	for (int i = 0; i < m_numParticles; i++) {
		Vector3f pos = m_vVecState[i*2];//  position of particle i. YOUR CODE HERE
		glPushMatrix();
		glTranslatef(pos[0], pos[1], pos[2] );
		glutSolidSphere(0.075f,10.0f,10.0f);
		glPopMatrix();
	}
}
