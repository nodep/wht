#pragma once

#include "Point.h"

//
// the coordinate system
//

class CoordSys: public Object<Vertex3D>
{
public:
	void Build();
};

//
// the ellipsoid axes calculated from the eigen vectors
//

class EllipsoidAxes: public Object<Vertex3D>
{
public:
	void Build(const Point<double>& center, const double radii[3], const double evecs[3][3]);
};

//
// one magnetometer measurement - raw or calibrated, only the colour is different
//

class MagPoint: public Object<Vertex3D>
{
public:
	enum { CALIBRATED_SCALE = 1000 };

	void Build(const mag_point_t& mp, bool large_dots);
	void BuildCalibrated(const mag_point_t& mp, const Point<double>& center, const double calibMatrix[3][3]);
};

//
// Custom Curves objects
//

class DotMarker: public Object<Vertex2D>
{
private:
	float	_x, _y, _size;

public:
	DotMarker()
		: _x(0), _y(0), _size(0)
	{}

	void Build(float nx, float ny, D3DCOLOR color, int size = 8, int thick = 2);
};

class Rectangle2D: public Object<Vertex2D>
{
public:
	void Build(int nx, int ny, int sx, int sy, D3DCOLOR color);
};

class LineList: public Object<Vertex2D>
{
public:
	void Add(const Vertex2D& begin, const Vertex2D& end)
	{
		_vertices.push_back(begin);
		_vertices.push_back(end);
	}
};
