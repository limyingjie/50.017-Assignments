#include "curve.h"
#include "extra.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
using namespace std;

namespace
{
    // Approximately equal to.  We don't want to use == because of
    // precision issues with floating point.
    inline bool approx( const Vector3f& lhs, const Vector3f& rhs )
    {
        const float eps = 1e-8f;
        return ( lhs - rhs ).absSquared() < eps;
    }

    
}
    

Curve evalBezier( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 || P.size() % 3 != 1 )
    {
        cerr << "evalBezier must be called with 3n+1 control points." << endl;
        //exit( 0 );
    }

    // TODO:
    // You should implement this function so that it returns a Curve
    // (e.g., a vector< CurvePoint >).  The variable "steps" tells you
    // the number of points to generate on each piece of the spline.
    // At least, that's how the sample solution is implemented and how
    // the SWP files are written.  But you are free to interpret this
    // variable however you want, so long as you can control the
    // "resolution" of the discretized spline curve with it.

    // Make sure that this function computes all the appropriate
    // Vector3fs for each CurvePoint: V,T,N,B.
    // [NBT] should be unit and orthogonal.

    // Also note that you may assume that all Bezier curves that you
    // receive have G1 continuity.  Otherwise, the TNB will not be
    // be defined at points where this does not hold.

    cerr << "\t>>> evalBezier has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

	int segments = (P.size() - 1) / 3;
	cerr << "Segments: " << segments << endl;

	//Cubic Bezier 
	//			        t^0   t^1  t^2   t^3	
	const Matrix4f MatV(1.f, -3.f, 3.f, -1.f,
		0.f, 3.f, -6.f, 3.f,
		0.f, 0.f, 3.f, -3.f,
		0.f, 0.f, 0.f, 1.f);

	//T is dV/dt
	const Matrix4f MatT(-3.f, 6.f, -3.f, 0.f,
		3.f, -12.f, 9.f, 0.f,
		0.f, 6.f, -9.f, 0.f,
		0.f, 0.f, 3.f, 0.f);

	// B is initialized to +ve z-direction, will be updated
	const Vector3f Binit(0.f, 0.f, 1.f);


	// Preallocate a curve with steps+1 CurvePoints
	Curve R((steps + 1)*segments);
	Vector3f Bi = Binit;
	for (int i = 0; i < segments; i++) {
		
		Matrix4f controlPoints(P[3*i + 0][0], P[3*i + 1][0], P[3*i + 2][0], P[3*i + 3][0],
			P[3*i + 0][1], P[3*i + 1][1], P[3*i + 2][1], P[3*i + 3][1],
			P[3*i + 0][2], P[3*i + 1][2], P[3*i + 2][2], P[3*i + 3][2],
			0.f, 0.f, 0.f, 0.f);
		for (int j = 0; j <= steps; j++) {
			float t = float(j) / float(steps);
			Vector4f timeT(1, t, t*t, t*t*t);

			Vector4f V4f = controlPoints*MatV*timeT;
			Vector3f V(V4f[0],V4f[1],V4f[2]);

			Vector4f T4f = controlPoints*MatT*timeT;
			Vector3f T=Vector3f(T4f[0], T4f[1], T4f[2]).normalized();

			Vector3f N = Vector3f::cross(Bi, T).normalized();

			Bi = Vector3f::cross(T, N).normalized();
			
			R[i*(steps + 1) + j].V = V;
			R[i*(steps + 1) + j].T = T;
			R[i*(steps + 1) + j].N = N;
			R[i*(steps + 1) + j].B = Bi;

		}
	}

    //cerr << "\t>>> Returning empty curve." << endl;

    // Right now this will just return this empty curve.
    //return Curve();
	return R;
}



Curve evalBspline( const vector< Vector3f >& P, unsigned steps )
{
    // Check
    if( P.size() < 4 )
    {
        cerr << "evalBspline must be called with 4 or more control points." << endl;
        exit( 0 );
    }

    // TODO:
    // It is suggested that you implement this function by changing
    // basis from B-spline to Bezier.  That way, you can just call
    // your evalBezier function.

    cerr << "\t>>> evalBSpline has been called with the following input:" << endl;

    cerr << "\t>>> Control points (type vector< Vector3f >): "<< endl;
    for( unsigned i = 0; i < P.size(); ++i )
    {
        cerr << "\t>>> " << P[i] << endl;
    }

    cerr << "\t>>> Steps (type steps): " << steps << endl;

	const Matrix4f MatBbezier(1.f, -3.f, 3.f, -1.f,
		0.f, 3.f, -6.f, 3.f,
		0.f, 0.f, 3.f, -3.f,
		0.f, 0.f, 0.f, 1.f);

	const Matrix4f MatBbSpline(1.f/6, -3.f/6, 3.f/6, -1.f/6,
		4.f/6, 0.f/6, -6.f/6, 3.f/6,
		1.f/6, 3.f/6, 3.f/6, -3.f/6,
		0.f/6, 0.f/6, 0.f/6, 1.f/6);

	vector<Vector3f> bezierPoints;
	const Matrix4f basisConversion(1.f / 6, 0.f / 6, 0.f / 6, 0.f / 6,
		4.f / 6, 4.f / 6, 2.f / 6, 1.f / 6,
		1.f / 6, 2.f / 6, 4.f / 6, 4.f / 6,
		0.f / 6, 0.f / 6, 0.f / 6, 1.f / 6);	

	int segments = P.size() - 3;

	for (int i = 0; i < segments; i++) {
		Matrix4f controlPoints(P[ i + 0][0], P[ i + 1][0], P[ i + 2][0], P[ i + 3][0],
			P[ i + 0][1], P[ i + 1][1], P[ i + 2][1], P[ i + 3][1],
			P[ i + 0][2], P[ i + 1][2], P[ i + 2][2], P[ i + 3][2],
			0.f, 0.f, 0.f, 0.f);
		Matrix4f GBB = controlPoints*basisConversion;
		for (int j = 0; j < 4; j++) {
			if (j < 3 || i == segments - 1) {
				Vector3f bezierSegment(GBB[4 * j + 0], GBB[4 * j + 1], GBB[4 * j + 2]);
				bezierPoints.push_back(bezierSegment);
			}
		}
	}
	Curve R = evalBezier(bezierPoints, steps);
	return R;
}



Curve evalCircle( float radius, unsigned steps )
{
    // This is a sample function on how to properly initialize a Curve
    // (which is a vector< CurvePoint >).
    
    // Preallocate a curve with steps+1 CurvePoints
    Curve R( steps+1 );

    // Fill it in counterclockwise
    for( unsigned i = 0; i <= steps; ++i )
    {
        // step from 0 to 2pi
        float t = 2.0f * M_PI * float( i ) / steps;

        // Initialize position
        // We're pivoting counterclockwise around the y-axis
        R[i].V = radius * Vector3f( cos(t), sin(t), 0 );
        
        // Tangent vector is first derivative
        R[i].T = Vector3f( -sin(t), cos(t), 0 );
        
        // Normal vector is second derivative
        R[i].N = Vector3f( -cos(t), -sin(t), 0 );

        // Finally, binormal is facing up.
        R[i].B = Vector3f( 0, 0, 1 );
    }

    return R;
}

void drawCurve( const Curve& curve, float framesize )
{
    // Save current state of OpenGL
    glPushAttrib( GL_ALL_ATTRIB_BITS );

    // Setup for line drawing
    glDisable( GL_LIGHTING ); 
    glColor4f( 1, 1, 1, 1 );
    glLineWidth( 1 );
    
    // Draw curve
    glBegin( GL_LINE_STRIP );
    for( unsigned i = 0; i < curve.size(); ++i )
    {
        glVertex( curve[ i ].V );
    }
    glEnd();

    glLineWidth( 1 );

    // Draw coordinate frames if framesize nonzero
    if( framesize != 0.0f )
    {
        Matrix4f M;

        for( unsigned i = 0; i < curve.size(); ++i )
        {
            M.setCol( 0, Vector4f( curve[i].N, 0 ) );
            M.setCol( 1, Vector4f( curve[i].B, 0 ) );
            M.setCol( 2, Vector4f( curve[i].T, 0 ) );
            M.setCol( 3, Vector4f( curve[i].V, 1 ) );

            glPushMatrix();
            glMultMatrixf( M );
            glScaled( framesize, framesize, framesize );
            glBegin( GL_LINES );
            glColor3f( 1, 0, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 1, 0, 0 );
            glColor3f( 0, 1, 0 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 1, 0 );
            glColor3f( 0, 0, 1 ); glVertex3d( 0, 0, 0 ); glVertex3d( 0, 0, 1 );
            glEnd();
            glPopMatrix();
        }
    }
    
    // Pop state
    glPopAttrib();
}

