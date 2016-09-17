#pragma once

// I don't want to have to make the user install DirectX runtime, and I don't want the person who compiles this
// to have to install the DirectX 9 SDK. So I implemented the things I would otherwise use import from d3dx9.dll.
//
// With this, the runtime or the SDK is not needed.
//
// I know this might look like I am reinventing the wheel, but the DirectX wheel comes with too many strings
// attached for my taste.

class MyMatrix;

class MyVector: public D3DVECTOR
{
public:
	MyVector() {}

	MyVector(float xx, float yy, float zz)
	{
		x = xx;
		y = yy;
		z = zz;
	}

	MyVector operator + (const MyVector& rhs) const
	{
		return MyVector(x + rhs.x, y + rhs.y, z + rhs.z);
	}

	MyVector operator - (const MyVector& rhs) const
	{
		return MyVector(x - rhs.x, y - rhs.y, z - rhs.z);
	}

	MyVector operator * (const float f) const
	{
		return MyVector(x * f, y * f, z * f);
	}

	MyVector operator / (const float f) const
	{
		return MyVector(x / f, y / f, z / f);
	}

	MyVector Normal(const float normal_mag = 1.0F) const;
	MyVector Cross(const MyVector& rhs) const;

	float Dot(const MyVector& rhs) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}

	MyVector Transform(const MyMatrix& m) const;

	double Length() const
	{
		return sqrt(x*x + y*y + z*z);
	}
};

class MyMatrix: public D3DMATRIX
{
public:
	MyMatrix operator * (const MyMatrix& rhs) const;

	static MyMatrix LookAtLH(const MyVector& vEye, const MyVector& vAt, const MyVector& vUp);
	static MyMatrix PerspectiveFovLH(const float fovy, const float aspect, const float zn, const float zf);
	static MyMatrix Identity();
	static MyMatrix RotationY(const float angle);
	static MyMatrix RotationX(const float angle);
	static MyMatrix RotationZ(const float angle);
	static MyMatrix RotationYawPitchRoll(const float yaw, const float pitch, const float roll);
	static MyMatrix Translation(const float x, const float y, const float z);
	static MyMatrix Scaling(const float sx, const float sy, const float sz);
};
