#include "TimeStepper.hpp"

///TODO: implement Explicit Euler time integrator here
void ForwardEuler::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> f = particleSystem->evalF(particleSystem->getState());
	vector<Vector3f> newState;
	for (unsigned i = 0; i < particleSystem->getState().size(); i++) {
		newState.push_back( particleSystem->getState()[i] + stepSize*f[i] );
	}
	particleSystem->setState(newState);
	
}

///TODO: implement Trapzoidal rule here
void Trapzoidal::takeStep(ParticleSystem* particleSystem, float stepSize)
{
	vector<Vector3f> f0 = particleSystem->evalF(particleSystem->getState());
	vector<Vector3f> interState;
	for (unsigned i = 0; i < particleSystem->getState().size(); i++) {
		interState.push_back(particleSystem->getState()[i] + stepSize*f0[i]);
	}
	vector<Vector3f> f1 = particleSystem->evalF(interState);
	vector<Vector3f> newState;
	for (unsigned i = 0; i < particleSystem->getState().size(); i++) {
		newState.push_back(particleSystem->getState()[i] + stepSize/2*(f0[i] + f1[i]));
	}
	particleSystem->setState(newState);
}
