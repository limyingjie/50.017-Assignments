#include "Mesh.h"

using namespace std;

void Mesh::load( const char* filename )
{
	// 2.1.1. load() should populate bindVertices, currentVertices, and faces

	// Add your code here.
	std::ifstream infile(filename);

	cout << "Loading object\n" << endl;
	std::string line;
	while (getline(infile, line)) {
		//std::cout << line << "\n";
		stringstream ss(line);
		Vector3f v;
		string s;
		ss >> s;
		if (s == "v") {
			ss >> v[0] >> v[1] >> v[2];
			bindVertices.push_back(v);
		}
		else if (s == "vn") {
			ss >> v[0] >> v[1] >> v[2];
			//vecn.push_back(v);
		}
		else if (s == "f") {
			unsigned f1, f2, f3;
			ss >> f1 >> f2 >> f3;
			Tuple3u f(f1, f2, f3);

			faces.push_back(f);

		}
	}

	// make a copy of the bind vertices as the current vertices
	currentVertices = bindVertices;

}

void Mesh::draw()
{
	// Since these meshes don't have normals
	// be sure to generate a normal per triangle.
	// Notice that since we have per-triangle normals
	// rather than the analytical normals from
	// assignment 1, the appearance is "faceted".

	for (unsigned i = 0; i<faces.size(); i++) {

		int f1 = faces[i][0];
		int f2 = faces[i][1];
		int f3 = faces[i][2];

		Vector3f normal = Vector3f::cross((currentVertices[faces[i][1] - 1] - currentVertices[faces[i][0] - 1]), (currentVertices[faces[i][2] - 1] - currentVertices[faces[i][0] - 1])).normalized();
		glBegin(GL_TRIANGLES);
		glNormal3f(normal[0], normal[1], normal[2]);
		glVertex3f(currentVertices[f1 - 1][0], currentVertices[f1 - 1][1], currentVertices[f1 - 1][2]);
		glVertex3f(currentVertices[f2 - 1][0], currentVertices[f2 - 1][1], currentVertices[f2 - 1][2]);
		glVertex3f(currentVertices[f3 - 1][0], currentVertices[f3 - 1][1], currentVertices[f3 - 1][2]);
		glEnd();

	}


}

void Mesh::loadAttachments( const char* filename, int numJoints )
{
	// 2.2. Implement this method to load the per-vertex attachment weights
	// this method should update m_mesh.attachments

	std::ifstream infile(filename);
	if (infile.is_open()) {
		string line;
		while (getline(infile, line)) {
			std::vector<float> weights;
			std::istringstream iss(line);
			float weight;
			for (std::string s; iss >> weight; )
				weights.push_back(weight);
			//cerr << weights[0] << endl;
			attachments.push_back(weights);
		}
	}

}
