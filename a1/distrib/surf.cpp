#include "surf.h"
#include "extra.h"
#include <math.h>  
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
	//steps = 4; //for debugging
	vector<Matrix3f> rotationMatrices;
	//compute the rotation matrices for each rotation step
	for (int i = 0; i < steps; i++) {
		float rad = (float)i / (float)steps * 2 * M_PI;
		rotationMatrices.push_back(Matrix3f::rotateY(rad));
	}

	for (int i = 0; i < profile.size(); i++) {
		Vector3f vertex = profile[i].V;
		Vector3f normal = profile[i].N;
		for (int j = 0; j < steps; j++) {
			Vector3f Rvertex = rotationMatrices[j] * vertex;
			Vector3f Rnormal = rotationMatrices[j] * normal;
			surface.VV.push_back(Rvertex);
			surface.VN.push_back(-1*Rnormal);
		}
	}

	cerr << "Profile size: " << profile.size() << endl;
	cerr << "Steps: " << steps << endl;
	cerr << profile.size()*steps << endl;
	cerr << surface.VV.size() << endl;

	for (unsigned i = 0; i < profile.size(); i++) {
		unsigned ii = (i + 1) % profile.size();
		for (unsigned j = 0; j < steps; j++) {
			unsigned jj = (j + 1)%steps;
			Tup3u face1 = { ii*steps + j, i*steps + jj, i*steps + j };
			Tup3u face2 = { ii*steps + j, ii*steps + jj, i*steps + jj };
			surface.VF.push_back(face1);
			surface.VF.push_back(face2);
		}
	}


    //cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

	//u for Curve C(u)
	//v for trajectory T(v)
	for (int u = 0; u < profile.size(); u++) {
		for (int v = 0; v < sweep.size(); v++) {

			Matrix4f transform(sweep[v].N[0], sweep[v].B[0], sweep[v].T[0], sweep[v].V[0],
				sweep[v].N[1], sweep[v].B[1], sweep[v].T[1], sweep[v].V[1],
				sweep[v].N[2], sweep[v].B[2], sweep[v].T[2], sweep[v].V[2],
				0.0f, 0.0f, 0.0f, 1.0f);

			// W.r.t. sweep segment:
			// N is the local x-axis
			// B is the local y-axis
			// T is the local z-axis
			Matrix3f transnorm(sweep[v].N, sweep[v].B, sweep[v].T);

			Vector3f Svertex = (transform * (Vector4f(profile[u].V.xyz(), 1.0f))).xyz();

			Vector3f Snormal = (transnorm * profile[u].N).normalized();

			surface.VV.push_back(Svertex);
			surface.VN.push_back(-1*Snormal);
		}
	}

    //cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

	for (unsigned i = 0; i < profile.size(); i++) {
		unsigned ii = (i + 1) % profile.size();
		for (unsigned j = 0; j < sweep.size(); j++) {
			unsigned jj = (j + 1) % sweep.size();
			Tup3u face1 = { i*sweep.size() + jj, i*sweep.size() + j,ii*sweep.size() + j };
			Tup3u face2 = { i*sweep.size() + jj, ii*sweep.size() + j, ii*sweep.size() + jj };
			surface.VF.push_back(face1);
			surface.VF.push_back(face2);
		}
	}

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
