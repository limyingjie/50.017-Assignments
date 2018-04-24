#include "SkeletalModel.h"

#include <FL/Fl.H>

using namespace std;

void SkeletalModel::load(const char *skeletonFile, const char *meshFile, const char *attachmentsFile)
{
	loadSkeleton(skeletonFile);

	m_mesh.load(meshFile);
	m_mesh.loadAttachments(attachmentsFile, m_joints.size());

	computeBindWorldToJointTransforms();
	updateCurrentJointToWorldTransforms();
}

void SkeletalModel::draw(Matrix4f cameraMatrix, bool skeletonVisible)
{
	// draw() gets called whenever a redraw is required
	// (after an update() occurs, when the camera moves, the window is resized, etc)

	m_matrixStack.clear();
	m_matrixStack.push(cameraMatrix);

	if( skeletonVisible )
	{
		drawJoints();

		drawSkeleton();
	}
	else
	{
		// Clear out any weird matrix we may have been using for drawing the bones and revert to the camera matrix.
		glLoadMatrixf(m_matrixStack.top());

		// Tell the mesh to draw itself.
		m_mesh.draw();
	}
}

void SkeletalModel::loadSkeleton( const char* filename )
{
	// Load the skeleton from file here.
	std::ifstream infile(filename);
	if (infile.is_open()) {
		string line;
		while (getline(infile,line)) {
			//cerr << line << endl;
			stringstream lineS(line);
			float x, y, z;
			int n;
			lineS >> x >> y >> z >> n;
			//cerr << x << " " << y << " " << z << endl;
			Joint *joint = new Joint;
			joint->transform = Matrix4f::translation(x, y, z);
			m_joints.push_back(joint);
			if (n==-1) m_rootJoint = joint;
			else m_joints[n]->children.push_back(joint);
		}
	}
}

void SkeletalModel::drawJoints( )
{
	// Draw a sphere at each joint. You will need to add a recursive helper function to traverse the joint hierarchy.
	//
	// We recommend using glutSolidSphere( 0.025f, 12, 12 )
	// to draw a sphere of reasonable size.
	//
	// You are *not* permitted to use the OpenGL matrix stack commands
	// (glPushMatrix, glPopMatrix, glMultMatrix).
	// You should use your MatrixStack class
	// and use glLoadMatrix() before your drawing call.
	drawJointsHelper(m_rootJoint);

}

void SkeletalModel::drawJointsHelper(Joint* joint) {
	m_matrixStack.push(joint->transform);
	glLoadMatrixf(m_matrixStack.top());
	glutSolidSphere(0.025f, 12, 12);
	for (unsigned i = 0; i < joint->children.size(); i++) {
		drawJointsHelper(joint->children[i]);
	}
	m_matrixStack.pop();
}

void SkeletalModel::drawSkeleton( )
{
	// Draw boxes between the joints. You will need to add a recursive helper function to traverse the joint hierarchy.
	drawSkeletonHelper(m_rootJoint, true);
}

void SkeletalModel::drawSkeletonHelper(Joint* joint, bool isRoot) {

	if (!isRoot) {
		Vector4f vectorDiff(joint->transform[12], joint->transform[13], joint->transform[14], 0);
		Matrix4f translate_z = Matrix4f::translation(0, 0, 0.5);
		float distance = vectorDiff.abs();
		Matrix4f scale = Matrix4f::scaling(0.05f, 0.05f, distance);
		Vector3f rnd(0, 0, 1);
		Vector3f z = vectorDiff.normalized().xyz();
		Vector3f y = Vector3f::cross(z, rnd).normalized();
		Vector3f x = Vector3f::cross(y, z).normalized();
		Matrix3f tempRot = Matrix3f(x, y, z);
		Matrix4f rot = Matrix4f::identity();
		rot.setSubmatrix3x3(0, 0, tempRot);

		m_matrixStack.push(rot*scale*translate_z);
		glLoadMatrixf(m_matrixStack.top());
		glutSolidCube(1.0f);
		m_matrixStack.pop();
	}

	m_matrixStack.push(joint->transform);
	for (unsigned i = 0; i < joint->children.size(); i++) {
		drawSkeletonHelper(joint->children[i], false);
	}
	m_matrixStack.pop();
}

void SkeletalModel::setJointTransform(int jointIndex, float rX, float rY, float rZ)
{
	// Set the rotation part of the joint's transformation matrix based on the passed in Euler angles.

	Matrix4f rotationMatrix = Matrix4f::rotateX(rX)*Matrix4f::rotateY(rY)*Matrix4f::rotateZ(rZ);
	m_joints[jointIndex]->transform.setSubmatrix3x3(0, 0, rotationMatrix.getSubmatrix3x3(0, 0));
}


void SkeletalModel::computeBindWorldToJointTransforms()
{
	// 2.3.1. Implement this method to compute a per-joint transform from
	// world-space to joint space in the BIND POSE.
	//
	// Note that this needs to be computed only once since there is only
	// a single bind pose.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	m_matrixStack.clear();
	BindWorldToJointTransformsHelper(m_rootJoint);
}

void SkeletalModel::BindWorldToJointTransformsHelper(Joint* joint)
{
	m_matrixStack.push(joint->transform);

	//get B inverse
	joint->bindWorldToJointTransform = m_matrixStack.top().inverse();
	for (unsigned i = 0; i < joint->children.size(); i++) {
		BindWorldToJointTransformsHelper(joint->children[i]);
	}

	m_matrixStack.pop();
}

void SkeletalModel::updateCurrentJointToWorldTransforms()
{
	// 2.3.2. Implement this method to compute a per-joint transform from
	// joint space to world space in the CURRENT POSE.
	//
	// The current pose is defined by the rotations you've applied to the
	// joints and hence needs to be *updated* every time the joint angles change.
	//
	// This method should update each joint's bindWorldToJointTransform.
	// You will need to add a recursive helper function to traverse the joint hierarchy.
	m_matrixStack.clear();
	CurrentJointToWorldTransformsHelper(m_rootJoint);
}

void SkeletalModel::CurrentJointToWorldTransformsHelper(Joint* joint)
{
	m_matrixStack.push(joint->transform);

	joint->currentJointToWorldTransform = m_matrixStack.top();
	for (unsigned i = 0; i<(joint->children.size()); i++) {
		CurrentJointToWorldTransformsHelper(joint->children[i]);
	}

	m_matrixStack.pop();
}

void SkeletalModel::updateMesh()
{
	// 2.3.2. This is the core of SSD.
	// Implement this method to update the vertices of the mesh
	// given the current state of the skeleton.
	// You will need both the bind pose world --> joint transforms.
	// and the current joint --> world transforms.


	vector<Vector3f> newVertices;
	for (unsigned i = 0; i < m_mesh.currentVertices.size(); i++) {
		Vector4f vertex = Vector4f();
		for (unsigned j = 0; j < m_mesh.attachments[i].size(); j++) {
			float weight = m_mesh.attachments[i][j];
			if (weight != 0) {
				// +1 because of root joint
				vertex = vertex + weight *
					(m_joints[j + 1]->currentJointToWorldTransform * //T
					m_joints[j + 1]->bindWorldToJointTransform * //B inverse
					Vector4f(m_mesh.bindVertices[i][0], m_mesh.bindVertices[i][1],
					m_mesh.bindVertices[i][2], 1.0f));
			}
		}
		newVertices.push_back(vertex.xyz());
	}
	m_mesh.currentVertices = newVertices;


	//cerr << m_mesh.currentVertices[0][0] << " "<< m_mesh.currentVertices[0][1] << " "<< m_mesh.currentVertices[0][2] << endl;
}

