#include "stdafx.h"

#include "../dongle/reports.h"

#include "my_win.h"
#include "my_d3dx.h"
#include "d3d.h"
#include "d3d_objects.h"

// translates magnetometer coordinates into D3D
template <class T>
Point<float> mag2d3d(const T& p)
{
	Point<float> rv;

	rv.y = (float) -p.z;
	rv.x = (float) p.y;
	rv.z = (float) -p.x;

	return rv;
}

void CoordSys::Build()
{
	_vertices.clear();

	// x - red
	_vertices.push_back(Vertex3D(1000, 0, 0, D3DCOLOR_XRGB(230, 80, 80)));
	_vertices.push_back(Vertex3D(-1000, 0, 0, D3DCOLOR_XRGB(60, 80, 80)));

	// y - green
	_vertices.push_back(Vertex3D(0, 1000, 0, D3DCOLOR_XRGB(80, 230, 80)));
	_vertices.push_back(Vertex3D(0, -1000, 0, D3DCOLOR_XRGB(80, 60, 80)));

	// z - blue
	_vertices.push_back(Vertex3D(0, 0, 1000, D3DCOLOR_XRGB(80, 80, 230)));
	_vertices.push_back(Vertex3D(0, 0, -1000, D3DCOLOR_XRGB(80, 80, 60)));
}

void EllipsoidAxes::Build(const Point<double>& center, const double radii[3], const double evecs[3][3])
{
	Point<float> centert(mag2d3d(center));
	Point<float> vec1(mag2d3d(Point<double>(evecs[0][0], evecs[0][1], evecs[0][2])));
	Point<float> vec2(mag2d3d(Point<double>(evecs[1][0], evecs[1][1], evecs[1][2])));
	Point<float> vec3(mag2d3d(Point<double>(evecs[2][0], evecs[2][1], evecs[2][2])));

	_vertices.clear();

	// create the ellipsoid principal axes
	MyVector vs(0, 0, 0);
	MyVector vx(vec1.x, vec1.y, vec1.z);
	MyVector vy(vec2.x, vec2.y, vec2.z);
	MyVector vz(vec3.x, vec3.y, vec3.z);

	vx = vx * (float) radii[0];
	vy = vy * (float) radii[1];
	vz = vz * (float) radii[2];

	_vertices.push_back(Vertex3D(vs, D3DCOLOR_XRGB(255, 200, 200)));
	_vertices.push_back(Vertex3D(vx, D3DCOLOR_XRGB(255, 200, 200)));

	_vertices.push_back(Vertex3D(vs, D3DCOLOR_XRGB(200, 255, 200)));
	_vertices.push_back(Vertex3D(vy, D3DCOLOR_XRGB(200, 255, 200)));

	_vertices.push_back(Vertex3D(vs, D3DCOLOR_XRGB(200, 200, 255)));
	_vertices.push_back(Vertex3D(vz, D3DCOLOR_XRGB(200, 200, 255)));

	// move them to the ellipsoid center
	MyMatrix translate(MyMatrix::Translation((float) centert.x, (float) centert.y, (float) centert.z));
	
	std::for_each(_vertices.begin(), _vertices.end(), [translate](Vertex3D& v) { v.pos = v.pos.Transform(translate); } );
}

void MagPoint::Build(const mag_point_t& mp, bool large_dots)
{
	Point<float> p(mag2d3d(mp));

	_vertices.clear();

	float edge_size = large_dots ? 2.0f : 1.0f;

	BuildCube(_vertices, edge_size, edge_size, edge_size, D3DCOLOR_XRGB(190, 190, 190));

	MyMatrix translate(MyMatrix::Translation(p.x, p.y, p.z));

	std::for_each(_vertices.begin(), _vertices.end(), [translate](Vertex3D& v) { v.pos = v.pos.Transform(translate); } );
}

void MagPoint::BuildCalibrated(const mag_point_t& mp, const Point<double>& center, const double calibMatrix[3][3])
{
	_vertices.clear();

	// point moved to center
	MyVector vpoint(float(mp.x - center.x), float(mp.y - center.y), float(mp.z - center.z));

	// the calibration matrix
	MyMatrix calib(MyMatrix::Identity());

	calib._11 = (float) calibMatrix[0][0];
	calib._12 = (float) calibMatrix[0][1];
	calib._13 = (float) calibMatrix[0][2];

	calib._21 = (float) calibMatrix[1][0];
	calib._22 = (float) calibMatrix[1][1];
	calib._23 = (float) calibMatrix[1][2];

	calib._31 = (float) calibMatrix[2][0];
	calib._32 = (float) calibMatrix[2][1];
	calib._33 = (float) calibMatrix[2][2];

	// calc the calibrated vector length
	MyVector vout(vpoint.Transform(calib));
	double vlen = vout.Length();
	D3DCOLOR col;
	const int BASE_COL = 0x60;
	if (vlen < CALIBRATED_SCALE)
	{
		size_t c = BASE_COL + int((CALIBRATED_SCALE - vlen)*1.5);
		if (c > 0xff)
			c = 0xff;

		col = D3DCOLOR_XRGB(c, BASE_COL, BASE_COL);
	} else {
		size_t c = BASE_COL + int((vlen - CALIBRATED_SCALE)*1.5);
		if (c > 0xff)
			c = 0xff;

		col = D3DCOLOR_XRGB(BASE_COL, BASE_COL, c);
	}

	// put the cube to its calibrated position
	BuildCube(_vertices, 6, 6, 6, col);

	Point<float> trans(mag2d3d(vout));

	MyMatrix translate(MyMatrix::Translation(trans.x, trans.y, trans.z));
	std::for_each(_vertices.begin(), _vertices.end(), [translate](Vertex3D& v) { v.pos = v.pos.Transform(translate); } );
}

void DotMarker::Build(float x, float y, D3DCOLOR color, int size, int thick)
{
	const int INNER_SIZE = size - thick;
	const int OUTER_SIZE = size;

	float otop, obottom, oleft, oright;
	otop	= y - OUTER_SIZE / 2.0f;
	obottom	= y + OUTER_SIZE / 2.0f;
	oleft	= x - OUTER_SIZE / 2.0f;
	oright	= x + OUTER_SIZE / 2.0f;

	float itop, ibottom, ileft, iright;
	itop	= y - INNER_SIZE / 2.0f;
	ibottom	= y + INNER_SIZE / 2.0f;
	ileft	= x - INNER_SIZE / 2.0f;
	iright	= x + INNER_SIZE / 2.0f;

	_vertices.clear();

	// up
	_vertices.push_back(Vertex2D(   oleft,    otop, color));
	_vertices.push_back(Vertex2D(  oright,    otop, color));
	_vertices.push_back(Vertex2D(  oright,    itop, color));

	_vertices.push_back(Vertex2D(   oleft,    otop, color));
	_vertices.push_back(Vertex2D(  oright,    itop, color));
	_vertices.push_back(Vertex2D(   oleft,    itop, color));

	// left
	_vertices.push_back(Vertex2D(   oleft,    itop, color));
	_vertices.push_back(Vertex2D(   ileft,    itop, color));
	_vertices.push_back(Vertex2D(   ileft, ibottom, color));

	_vertices.push_back(Vertex2D(   oleft,    itop, color));
	_vertices.push_back(Vertex2D(   ileft, ibottom, color));
	_vertices.push_back(Vertex2D(   oleft, ibottom, color));

	// right
	_vertices.push_back(Vertex2D(  iright,    itop, color));
	_vertices.push_back(Vertex2D(  oright,    itop, color));
	_vertices.push_back(Vertex2D(  oright, ibottom, color));

	_vertices.push_back(Vertex2D(  iright,    itop, color));
	_vertices.push_back(Vertex2D(  oright, ibottom, color));
	_vertices.push_back(Vertex2D(  iright, ibottom, color));

	// down
	_vertices.push_back(Vertex2D(   oleft, ibottom, color));
	_vertices.push_back(Vertex2D(  oright, obottom, color));
	_vertices.push_back(Vertex2D(   oleft, obottom, color));

	_vertices.push_back(Vertex2D(   oleft, ibottom, color));
	_vertices.push_back(Vertex2D(  oright, ibottom, color));
	_vertices.push_back(Vertex2D(  oright, obottom, color));

	_x = oleft;
	_y = otop;
	_size = float(size);
}

void Rectangle2D::Build(int nx, int ny, int sx, int sy, D3DCOLOR color)
{
	float top, bottom, left, right;

	top		= float(ny);
	bottom	= float(ny + sy);
	left	= float(nx);
	right	= float(nx + sx);

	_vertices.clear();

	_vertices.push_back(Vertex2D(   left,    top, color));
	_vertices.push_back(Vertex2D(  right,    top, color));
	_vertices.push_back(Vertex2D(  right, bottom, color));

	_vertices.push_back(Vertex2D(   left,    top, color));
	_vertices.push_back(Vertex2D(  right, bottom, color));
	_vertices.push_back(Vertex2D(   left, bottom, color));
}
