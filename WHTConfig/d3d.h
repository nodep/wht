#pragma once

#define ThrowD3DException(rslt, msg)		ThrowD3DExceptionFileLine(rslt, msg, WIDEN(__FILE__), __LINE__)
void ThrowD3DExceptionFileLine(HRESULT rslt, const std::wstring& m, const wchar_t* file, const int line_no);

template <class VertexT>
class VertexBuffer;


//
// the direct3D object
//

class Direct3D
{
private:
	IDirect3D9*		_pD3D;

public:
	Direct3D();

	~Direct3D()
	{
		// shutdown Direct3D
		_pD3D->Release();
	}

	std::string GetAdapterName() const;
	void GetAdapterDisplayMode(D3DDISPLAYMODE& d3ddm);
	bool CheckDeviceMultiSampleType(D3DFORMAT format, D3DMULTISAMPLE_TYPE multisample_type);
	IDirect3DDevice9* CreateDevice(const Window& wnd, D3DPRESENT_PARAMETERS& d3d_pp);
};

Direct3D& GetD3D();

//
// 3D device
//

class DeviceD3D
{
private:
	IDirect3DDevice9*		_pDevice;

	D3DPRESENT_PARAMETERS	_d3d_pp;
	
	void DrawVertices(IDirect3DVertexBuffer9* pvb, size_t vcount, size_t vsize, D3DPRIMITIVETYPE primitive_type);
	D3DMULTISAMPLE_TYPE GetBestMultisampleType(D3DFORMAT format);

public:
	DeviceD3D()
		: _pDevice(0)
	{}

	~DeviceD3D();

	void Release()
	{
		if (_pDevice != 0)
			_pDevice->Release();

		_pDevice = 0;
	}
	
	void Init(const Window& d3d_win);
	void Init2D(const Window& win);

	void SetView(const MyVector& camera_pos, const MyVector& look_at, const MyVector& up);
	bool IsValid();
	void Clear(bool clear_zbuffer, D3DCOLOR color = D3DCOLOR_XRGB(0, 0, 85));

	void BeginScene();
	void EndScene();
	void Present();

	void SetProjectionTransform(const MyMatrix& matProjection);
	void GetViewport(D3DVIEWPORT9& viewport);
	void SetViewport(D3DVIEWPORT9& viewport);

	void SetCulling(const size_t cull_mode);

	IDirect3DSurface9* GetBackBuffer();
	IDirect3DVertexBuffer9* CreateVertexBuffer(const int vcount, const int vsize, const size_t vformat);

	template <class VertexT>
	void DrawVertices(VertexBuffer<VertexT>& vbuff, D3DPRIMITIVETYPE primitive_type)
	{
		if (_pDevice == 0)
			ThrowD3DException(0, L"Device not initilized while creating VertexBuffer");

		if (vbuff.IsEmpty())
			return;

		DrawVertices(vbuff._pvb, vbuff.Count(), sizeof(VertexT), primitive_type);
	}
};


//
// the vertex format for the mag calbration dialog
//

struct Vertex3D
{
	enum {fvf_id = D3DFVF_XYZ | D3DFVF_DIFFUSE};

	MyVector	pos;
	D3DCOLOR	diffuse;

	Vertex3D()
	{}

	Vertex3D(float x, float y, float z, D3DCOLOR col)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
		diffuse = col;
	}

	Vertex3D(float x, float y, float z)
	{
		pos.x = x;
		pos.y = y;
		pos.z = z;
	}

	Vertex3D(const MyVector& p, D3DCOLOR col)
	{
		pos = p;
		diffuse = col;
	}
};

//
// the vertex format for the custom curves dialog
//

struct Vertex2D
{
	enum {fvf_id = D3DFVF_XYZRHW | D3DFVF_DIFFUSE};

	float		x, y, z;
	float		rhw;
	D3DCOLOR	diffuse;

	Vertex2D()
	{}

	Vertex2D(float xi, float yi, D3DCOLOR col)
	{
		x = xi;
		y = yi;
		z = 0.5f;
		rhw = 1.0f;
		diffuse = col;
	}

	Vertex2D(float xi, float yi)
	{
		x = xi;
		y = yi;
		z = 0.5f;
		rhw = 1.0f;
	}
};

//
// base class for the objects we'll have in the scene
//

template <class VertexT>
class Object
{
protected:
	// this holds the vertices for the object
	std::vector<VertexT>	_vertices;

public:

	void Clear()
	{
		_vertices.clear();
	}

	const std::vector<VertexT>& GetVertices() const
	{
		return _vertices;
	}
};


//
// the vertex buffer
//

template <class VertexT>
class VertexBuffer
{
	friend class DeviceD3D;

private:
	// interface pointer
	IDirect3DVertexBuffer9*		_pvb;

	// while the buffer is locked, this is the pointer to the first locked vertex
	VertexT*				_pVertex;

	size_t	_vcount;
	size_t	_vcapacity;

public:
	VertexBuffer::VertexBuffer()
	:	_pvb(0),
		_pVertex(0),
		_vcount(0),
		_vcapacity(0)
	{}

	VertexBuffer(const VertexBuffer& c)
	:	_pvb(c._pvb),
		_pVertex(c._pVertex),
		_vcount(c._vcount),
		_vcapacity(c._vcapacity)
	{
		if (_pvb)
			_pvb->AddRef();
	}

	~VertexBuffer()
	{
		Release();
	}

	bool IsEmpty() const
	{
		return _pvb == 0  ||  _vcount == 0;
	}

	bool IsLocked() const
	{
		return _pVertex != 0;
	}

	void Clear()
	{
		_vcount = 0;
	}

	void Alloc(DeviceD3D& dev, const size_t vcap)
	{
		if (vcap == 0)
			ThrowD3DException(0, L"VertexBuffer->Lock() failed");

		// release the buffer if we already have one
		Release();

		_pvb = dev.CreateVertexBuffer(vcap, sizeof(VertexT), VertexT::fvf_id);

		_vcapacity = vcap;
	}

	void Lock()
	{
		// are we already locked?
		if (_pVertex)
		{
			assert(false);
			return;
		}

		if (_pvb == 0)
			ThrowD3DException(0, L"VertexBuffer->Lock(), but buffer not allocated");

		// lock
		LPVOID ret_val;
		HRESULT rslt = _pvb->Lock(0, _vcapacity * sizeof(Vertex3D), &ret_val, 0);
		if (FAILED(rslt))		ThrowD3DException(rslt, L"VertexBuffer->Lock() failed");

		// set the vertex pointer pointer
		_pVertex = (VertexT*) ret_val;
	}

	void Unlock()
	{
		HRESULT rslt = _pvb->Unlock();
		if (FAILED(rslt))		ThrowD3DException(rslt, L"VertexBuffer->Unlock() failed");

		_pVertex = 0;
	}

	size_t Count() const		{ return _vcount; }
	size_t Capacity() const		{ return _vcapacity; }

	void Release()
	{
		// is the buffer locked?
		if (_pVertex)
			Unlock();

		if (_pvb)
		{
			int ref_cnt = _pvb->Release();
			// assert(ref_cnt == 0);
		}

		_vcapacity = 0;
		_vcount = 0;
		_pvb = 0;
	}

	void AddObject(const Object<VertexT>& obj)
	{
		if (_pVertex == 0)
			ThrowD3DException(0, L"VertexBuffer::AddObject() on unlocked buffer");

		// number of vertices to add to the buffer
		size_t vert2add = obj.GetVertices().size();

		assert(vert2add > 0);

		// do we have enough room in the buffer?
		if (_vcapacity - _vcount < vert2add)
			ThrowD3DException(0, L"VertexBuffer::AddObject() vertex buffer overflow");

		// copy the vertices
		// I would rather use std::copy() here, but VC++ starts panicking and spits out a C4996
		// so, I'll just do the ugly thing, and solve the problem :)
		memcpy(_pVertex + _vcount, &obj.GetVertices().front(), sizeof(VertexT) * vert2add);

		// adjust the number of vertices we have
		_vcount += vert2add;
	}
};


//
// the camera class
//

class Camera
{
private:
	MyVector		_up;
	MyVector		_camera_pos;

	float			_rotY;		// the current camera rotation and scale
	float			_rotX;
	float			_scale;

	DeviceD3D&		_dev;

	void CalcCamera();

public:
	Camera(DeviceD3D& d);

	void Reset();

	// sets the view transformation on the device
	void RefreshPos();

	void SetRotation(float deltaY, float deltaX);
	void Zoom(const int zoom);
};

// builds a cube of given dimensions around (0,0,0)
void BuildCube(std::vector<Vertex3D>& v, float Width, float Height, float Depth, D3DCOLOR col);

// builds a cube of given dimensions at given coordinates
void BuildCube(std::vector<Vertex3D>& v, float Width, float Height, float Depth, float x, float y, float z, D3DCOLOR col);
