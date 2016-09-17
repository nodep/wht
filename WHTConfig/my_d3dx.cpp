#include "stdafx.h"

#include "my_d3dx.h"

MyVector MyVector::Normal(const float normal_mag) const
{
	MyVector rv;

	float m = sqrt(float(x*x + y*y + z*z));

	rv.x = x / m * normal_mag;
	rv.y = y / m * normal_mag;
	rv.z = z / m * normal_mag;

	return rv;
}

MyVector MyVector::Cross(const MyVector& rhs) const
{
	MyVector rv;
    
	rv.x = y * rhs.z - z * rhs.y;
	rv.y = z * rhs.x - x * rhs.z;
	rv.z = x * rhs.y - y * rhs.x;
    
	return rv;
}

MyVector MyVector::Transform(const MyMatrix& m) const
{
	MyVector rv;

    rv.x = m.m[0][0] * x + m.m[1][0] * y + m.m[2][0] * z + m.m[3][0];
    rv.y = m.m[0][1] * x + m.m[1][1] * y + m.m[2][1] * z + m.m[3][1];
    rv.z = m.m[0][2] * x + m.m[1][2] * y + m.m[2][2] * z + m.m[3][2];
    //rv.w = m.m[0][3] * x + m.m[1][3] * y + m.m[2][3] * z + m.m[3][3];

	return rv;
}

// **************************************************************************************************

MyMatrix MyMatrix::LookAtLH(const MyVector& vEye, const MyVector& vAt, const MyVector& vUp)
{
	MyVector vZ((vAt - vEye).Normal());
	MyVector vX((vUp.Cross(vZ)).Normal());
	MyVector vY(vZ.Cross(vX));

	MyMatrix rv;

	rv._11 = vX.x;			rv._12 = vY.x;			rv._13 = vZ.x;			rv._14 = 0.0f;
	rv._21 = vX.y;			rv._22 = vY.y;			rv._23 = vZ.y;			rv._24 = 0.0f;
	rv._31 = vX.z;			rv._32 = vY.z;			rv._33 = vZ.z;			rv._34 = 0.0f;
	rv._41 = -vX.Dot(vEye);	rv._42 = -vY.Dot(vEye);	rv._43 = -vZ.Dot(vEye);	rv._44 = 1.0f;

	return rv;
}

MyMatrix MyMatrix::PerspectiveFovLH(const float fovy, const float aspect, const float zn, const float zf)
{
	float yScale = 1 / tan(fovy / 2);
	float xScale = yScale / aspect;
	float m33 = zf/(zf-zn);
	float m43 = -zn*zf/(zf-zn);

	MyMatrix rv;

	rv._11 = xScale;		rv._12 = 0.0f;			rv._13 = 0.0f;				rv._14 = 0.0f;
	rv._21 = 0.0f;			rv._22 = yScale;		rv._23 = 0.0f;				rv._24 = 0.0f;
	rv._31 = 0.0f;			rv._32 = 0.0f;			rv._33 = zf/(zf-zn);		rv._34 = 1.0f;
	rv._41 = 0.0f;			rv._42 = 0.0f;			rv._43 = (zn*zf)/(zn-zf);	rv._44 = 0.0f;

	return rv;
}

MyMatrix MyMatrix::Identity()
{
	MyMatrix rv;

	rv._11 = 1.0f;	rv._12 = 0.0f;	rv._13 = 0.0f;	rv._14 = 0.0f;
	rv._21 = 0.0f;	rv._22 = 1.0f;	rv._23 = 0.0f;	rv._24 = 0.0f;
	rv._31 = 0.0f;	rv._32 = 0.0f;	rv._33 = 1.0f;	rv._34 = 0.0f;
	rv._41 = 0.0f;	rv._42 = 0.0f;	rv._43 = 0.0f;	rv._44 = 1.0f;

	return rv;
}

MyMatrix MyMatrix::RotationY(const float angle)
{
	MyMatrix rv(Identity());

	rv.m[0][0] = cosf(angle);
    rv.m[2][2] = cosf(angle);
    rv.m[0][2] = -sinf(angle);
    rv.m[2][0] = sinf(angle);

    return rv;
}

MyMatrix MyMatrix::RotationX(const float angle)
{
	MyMatrix rv(Identity());

    rv.m[1][1] = cosf(angle);
    rv.m[2][2] = cosf(angle);
    rv.m[1][2] = sinf(angle);
    rv.m[2][1] = -sinf(angle);

    return rv;
}

MyMatrix MyMatrix::RotationZ(const float angle)
{
	MyMatrix rv(Identity());

    rv.m[0][0] = cosf(angle);
    rv.m[1][1] = cosf(angle);
    rv.m[0][1] = sinf(angle);
    rv.m[1][0] = -sinf(angle);

	return rv;
}

MyMatrix MyMatrix::Scaling(const float sx, const float sy, const float sz)
{
	MyMatrix rv(Identity());

    rv.m[0][0] = sx;
    rv.m[1][1] = sy;
    rv.m[2][2] = sz;

    return rv;
}

MyMatrix MyMatrix::Translation(const float x, const float y, const float z)
{
	MyMatrix rv(Identity());

    rv.m[3][0] = x;
    rv.m[3][1] = y;
    rv.m[3][2] = z;

    return rv;
}

MyMatrix MyMatrix::RotationYawPitchRoll(const float yaw, const float pitch, const float roll)
{
    const float sroll = sinf(roll);
    const float croll = cosf(roll);
    const float spitch = sinf(pitch);
    const float cpitch = cosf(pitch);
    const float syaw = sinf(yaw);
    const float cyaw = cosf(yaw);

	MyMatrix rv;

    rv.m[0][0] = sroll * spitch * syaw + croll * cyaw;
    rv.m[0][1] = sroll * cpitch;
    rv.m[0][2] = sroll * spitch * cyaw - croll * syaw;
    rv.m[0][3] = 0.0f;
    rv.m[1][0] = croll * spitch * syaw - sroll * cyaw;
    rv.m[1][1] = croll * cpitch;
    rv.m[1][2] = croll * spitch * cyaw + sroll * syaw;
    rv.m[1][3] = 0.0f;
    rv.m[2][0] = cpitch * syaw;
    rv.m[2][1] = -spitch;
    rv.m[2][2] = cpitch * cyaw;
    rv.m[2][3] = 0.0f;
    rv.m[3][0] = 0.0f;
    rv.m[3][1] = 0.0f;
    rv.m[3][2] = 0.0f;
    rv.m[3][3] = 1.0f;

    return rv;
}

MyMatrix MyMatrix::operator * (const MyMatrix& rhs) const
{
	MyMatrix rv;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            rv.m[i][j] =  m[i][0] * rhs.m[0][j]
						+ m[i][1] * rhs.m[1][j]
						+ m[i][2] * rhs.m[2][j]
						+ m[i][3] * rhs.m[3][j];
        }
    }

	return rv;
}

