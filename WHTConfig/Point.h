#pragma once

template <class T>
struct Point
{
	T		x, y, z;

	Point()
		: x(0), y(0), z(0)
	{}

	Point(T nx, T ny, T nz)
		: x(nx), y(ny), z(nz)
	{}

	bool operator < (const Point& rhs) const
	{
		return x == rhs.x ? (y == rhs.y ? z < rhs.z : y < rhs.y) : x < rhs.x;
	}

	void Normalize(const T normal_mag = 1)
	{
		double m = sqrt(double(x*x + y*y + z*z));
		x = T(x / m * normal_mag);
		y = T(y / m * normal_mag);
		z = T(z / m * normal_mag);
	}
};
