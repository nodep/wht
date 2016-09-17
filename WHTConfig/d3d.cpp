#include "stdafx.h"
#pragma hdrstop

#pragma comment(lib, "d3d9.lib")

#include "my_win.h"
#include "my_d3dx.h"
#include "d3d.h"

#define DEFINE_HERROR(err)		case err:	srslt = L#err; break;

void ThrowD3DExceptionFileLine(HRESULT rslt, const std::wstring& m, const wchar_t* file, const int line_no)
{
	// don't want to link dxerr.lib, so I just add manualy do the few most common errors
	//const wchar_t* srslt = DXGetErrorString(rslt);

	const wchar_t* srslt = L"<unknown>";

	switch (rslt)
	{
	DEFINE_HERROR(D3DERR_WRONGTEXTUREFORMAT)
	DEFINE_HERROR(D3DERR_UNSUPPORTEDCOLOROPERATION)
	DEFINE_HERROR(D3DERR_UNSUPPORTEDCOLORARG)
	DEFINE_HERROR(D3DERR_UNSUPPORTEDALPHAOPERATION)
	DEFINE_HERROR(D3DERR_UNSUPPORTEDALPHAARG)
	DEFINE_HERROR(D3DERR_TOOMANYOPERATIONS)
	DEFINE_HERROR(D3DERR_CONFLICTINGTEXTUREFILTER)
	DEFINE_HERROR(D3DERR_UNSUPPORTEDFACTORVALUE)
	DEFINE_HERROR(D3DERR_CONFLICTINGRENDERSTATE)
	DEFINE_HERROR(D3DERR_UNSUPPORTEDTEXTUREFILTER)
	DEFINE_HERROR(D3DERR_CONFLICTINGTEXTUREPALETTE)
	DEFINE_HERROR(D3DERR_DRIVERINTERNALERROR)

	DEFINE_HERROR(D3DERR_NOTFOUND)
	DEFINE_HERROR(D3DERR_MOREDATA)
	DEFINE_HERROR(D3DERR_DEVICELOST)
	DEFINE_HERROR(D3DERR_DEVICENOTRESET)
	DEFINE_HERROR(D3DERR_NOTAVAILABLE)
	DEFINE_HERROR(D3DERR_OUTOFVIDEOMEMORY)
	DEFINE_HERROR(D3DERR_INVALIDDEVICE)
	DEFINE_HERROR(D3DERR_INVALIDCALL)
	DEFINE_HERROR(D3DERR_DRIVERINVALIDCALL)
	DEFINE_HERROR(D3DERR_WASSTILLDRAWING)
	DEFINE_HERROR(D3DOK_NOAUTOGEN)
	}

	debug(std::wstring(file) + L"(" + int2str(line_no) + L") : exception: " + m + L" rslt=" + srslt + L"\n");

	throw std::wstring(m + L"\n" + srslt);
}

Direct3D& GetD3D()
{
	static Direct3D d3d;

	return d3d;
}

Direct3D::Direct3D()
{
	// create the direct3D object
	_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (_pD3D == 0)
		ThrowD3DException(0, L"Direct3DCreate9() returned NULL");
}

std::string Direct3D::GetAdapterName() const
{
	HRESULT rslt;
	D3DADAPTER_IDENTIFIER9 identifier;

	rslt = _pD3D->GetAdapterIdentifier(0, 0, &identifier);
	if (FAILED(rslt))
		ThrowD3DException(rslt, L"GetAdapterIdentifier() failed!");

	/*
	// enumerate the adapters
	UINT adapter_cnt = _pD3D->GetAdapterCount();
	for (UINT c = 0; c < adapter_cnt; c++)
	{
		rslt = _pD3D->GetAdapterIdentifier(c, 0, &identifier);
		if (FAILED(rslt))
			ThrowD3DException(rslt, L"GetAdapterIdentifier() failed!!!\n");
	}
	*/

	return identifier.Description;
}

void Direct3D::GetAdapterDisplayMode(D3DDISPLAYMODE& d3ddm)
{
	HRESULT rslt = _pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);
	if (FAILED(rslt))
		ThrowD3DException(rslt, L"GetAdapterDisplayMode() failed!");
}

bool Direct3D::CheckDeviceMultiSampleType(D3DFORMAT format, D3DMULTISAMPLE_TYPE multisample_type)
{
	return SUCCEEDED(_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, format, TRUE, multisample_type, 0));
}

IDirect3DDevice9* Direct3D::CreateDevice(const Window& win, D3DPRESENT_PARAMETERS& d3d_pp)
{
	IDirect3DDevice9* ret_val;
	HRESULT rslt = _pD3D->CreateDevice(	D3DADAPTER_DEFAULT,
										D3DDEVTYPE_HAL,
										win.GetHandle(),
										D3DCREATE_HARDWARE_VERTEXPROCESSING,
										&d3d_pp,
										&ret_val);

	if (FAILED(rslt))
		ThrowD3DException(rslt, L"CreateDevice() failed!!!\n");

	return ret_val;
}

void DeviceD3D::Init2D(const Window& win)
{
	// PresentParams struct to hold info about the rendering method
	::ZeroMemory(&_d3d_pp, sizeof(_d3d_pp));

	// Get the settings for the current display mode. This gives us hints on how to setup our
	// Present Parameters struct
	D3DDISPLAYMODE d3ddm;
	GetD3D().GetAdapterDisplayMode(d3ddm);

	// the width & height of the back buffer in pixels
	RECT r;
	::GetWindowRect(win.GetHandle(), &r);
	_d3d_pp.BackBufferWidth = r.right - r.left;
	_d3d_pp.BackBufferHeight = r.bottom - r.top;

    _d3d_pp.Windowed = TRUE;
    _d3d_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    _d3d_pp.hDeviceWindow = win.GetHandle();

	// the format of the backbuffer is the same as our current desktop
	_d3d_pp.BackBufferFormat = d3ddm.Format;

	// try the best type of multisampling (anti-aliasing)
	_d3d_pp.MultiSampleType = GetBestMultisampleType(d3ddm.Format);

	// Get a pointer to the IDirect3DDevice9 interface
	Release();
	_pDevice = GetD3D().CreateDevice(win, _d3d_pp);

	// set the vector format
	HRESULT rslt = _pDevice->SetFVF(Vertex2D::fvf_id);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"SetFVF() failed");

	// kill the lights
	_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"SetRenderState(D3DRS_LIGHTING, FALSE) failed");
}

D3DMULTISAMPLE_TYPE DeviceD3D::GetBestMultisampleType(D3DFORMAT format)
{
	D3DMULTISAMPLE_TYPE retval = D3DMULTISAMPLE_NONE;
	Direct3D& d3d(GetD3D());

	if (d3d.CheckDeviceMultiSampleType(format, D3DMULTISAMPLE_16_SAMPLES))
		retval = D3DMULTISAMPLE_16_SAMPLES;
	else if (d3d.CheckDeviceMultiSampleType(format, D3DMULTISAMPLE_8_SAMPLES))
		retval = D3DMULTISAMPLE_8_SAMPLES;
	else if ((d3d.CheckDeviceMultiSampleType(format, D3DMULTISAMPLE_4_SAMPLES)))
		retval = D3DMULTISAMPLE_4_SAMPLES;
	else if ((d3d.CheckDeviceMultiSampleType(format, D3DMULTISAMPLE_2_SAMPLES)))
		retval = D3DMULTISAMPLE_2_SAMPLES;

	return retval;
}

void DeviceD3D::Init(const Window& win)
{
	// PresentParams struct to hold info about the rendering method
	::ZeroMemory(&_d3d_pp, sizeof(_d3d_pp));

	Direct3D& d3d(GetD3D());

	// Get the settings for the current display mode. This gives us hints on how to setup our
	// Present Parameters struct
	D3DDISPLAYMODE d3ddm;
	d3d.GetAdapterDisplayMode(d3ddm);

	// the width & height of the back buffer in pixels
	RECT r;
	::GetWindowRect(win.GetHandle(), &r);
	_d3d_pp.BackBufferWidth = r.right - r.left;
	_d3d_pp.BackBufferHeight = r.bottom - r.top;
	
	// the format of the backbuffer is the same as our current desktop
	_d3d_pp.BackBufferFormat = d3ddm.Format;

	// handle to render target window
	_d3d_pp.Windowed = TRUE;
	_d3d_pp.hDeviceWindow = win.GetHandle();
	
	// number of back buffers
	_d3d_pp.BackBufferCount = 1;

	// swap method
	_d3d_pp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	// try the best type of multisampling (anti-aliasing)
	_d3d_pp.MultiSampleType = GetBestMultisampleType(d3ddm.Format);

	// Let D3D manage the depth buffer
	_d3d_pp.EnableAutoDepthStencil = TRUE;

	// Set the depth buffer format to 16bits
	_d3d_pp.AutoDepthStencilFormat = D3DFMT_D16;
	
	// Use default refresh rate
	_d3d_pp.FullScreen_RefreshRateInHz = 0;

	// present the information as fast as possible.
	_d3d_pp.PresentationInterval = 0;

	// Get a pointer to the IDirect3DDevice9 interface
	Release();
	_pDevice = d3d.CreateDevice(win, _d3d_pp);

	// set the vector format
	HRESULT rslt = _pDevice->SetFVF(Vertex3D::fvf_id);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"SetFVF() failed");

	// kill the lights
	_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"SetRenderState(D3DRS_LIGHTING, FALSE) failed");
}

void DeviceD3D::SetView(const MyVector& camera_pos, const MyVector& look_at, const MyVector& up)
{
	// create a look at matrix from eye, lookat and up vectors.
	MyMatrix mMatView(MyMatrix::LookAtLH(camera_pos, look_at, up));

	// tell D3D to use our View matrix for the view transformation stage
	HRESULT rslt = _pDevice->SetTransform(D3DTS_VIEW, &mMatView);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"Failed to set view transform.");
}

DeviceD3D::~DeviceD3D()
{
	Release();
}

bool DeviceD3D::IsValid()
{
	if (_pDevice == 0)
		return false;

	// test the current state of the device
	HRESULT rslt = _pDevice->TestCooperativeLevel();
	if (FAILED(rslt))
	{
		// if the device is lost, return a false
		if (rslt == D3DERR_DEVICELOST)
			return false;

		// if the device is ready to be reset
		if (rslt == D3DERR_DEVICENOTRESET)
		{
			// reset the device
			rslt = _pDevice->Reset(&_d3d_pp);
			if (FAILED(rslt))
				return false;	//ThrowD3DException(rslt, L"Unable to reset device");
		}
	}

	return true;
}

void DeviceD3D::Clear(bool clear_zbuffer, D3DCOLOR color)
{
	DWORD flags = D3DCLEAR_TARGET;
	
	if (clear_zbuffer)
		flags |= D3DCLEAR_ZBUFFER;

	HRESULT rslt = _pDevice->Clear(0,0, flags, color, 1.0f, 0);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"dev->Clear() failed");
}

void DeviceD3D::BeginScene()
{
	HRESULT rslt = _pDevice->BeginScene();
	if (FAILED(rslt))		ThrowD3DException(rslt, L"BeginScene() failed");
}

void DeviceD3D::EndScene()
{
	HRESULT rslt = _pDevice->EndScene();
	if (FAILED(rslt))		ThrowD3DException(rslt, L"EndScene() failed");
}

void DeviceD3D::Present()
{
    HRESULT rslt = _pDevice->Present(NULL, NULL, NULL, NULL);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"Present() failed");
}

void DeviceD3D::SetProjectionTransform(const MyMatrix& matProjection)
{
	HRESULT rslt = _pDevice->SetTransform(D3DTS_PROJECTION, &matProjection);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"SetTransform(D3DTS_PROJECTION, ...) failed");
}

void DeviceD3D::SetCulling(const size_t cull_mode)
{
	HRESULT rslt = _pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"SetRenderState(D3DRS_CULLMODE, ...) failed");
}

IDirect3DSurface9* DeviceD3D::GetBackBuffer()
{
	LPDIRECT3DSURFACE9 pbs;
	HRESULT rslt = _pDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pbs);
	if (FAILED(rslt))
		ThrowD3DException(rslt, L"GetBackBuffer() failed !!!");

	return pbs;
}

void DeviceD3D::GetViewport(D3DVIEWPORT9& viewport)
{
	HRESULT rslt = _pDevice->GetViewport(&viewport);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"GetViewport() failed");
}

void DeviceD3D::SetViewport(D3DVIEWPORT9& viewport)
{
	HRESULT rslt = _pDevice->SetViewport(&viewport);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"SetViewport() failed");
}

IDirect3DVertexBuffer9* DeviceD3D::CreateVertexBuffer(const int vcount, const int vsize, const size_t vformat)
{
	if (_pDevice == 0)
		ThrowD3DException(0, L"Device not initilized while creating VertexBuffer");

	IDirect3DVertexBuffer9* ret_val;
	HRESULT rslt = _pDevice->CreateVertexBuffer(	vcount * vsize,
													D3DUSAGE_WRITEONLY,
													vformat,
													D3DPOOL_DEFAULT,	//D3DPOOL_MANAGED,
													&ret_val,
													0);

	if (FAILED(rslt))
		ThrowD3DException(rslt, L"CreateVertexBuffer() failed");

	return ret_val;
}

void DeviceD3D::DrawVertices(IDirect3DVertexBuffer9* pvb, size_t vcount, size_t vsize, D3DPRIMITIVETYPE primitive_type)
{
	HRESULT rslt = _pDevice->SetStreamSource(0, pvb, 0, vsize);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"SetStreamSource() failed");

	UINT num_primitives = vcount / (primitive_type == D3DPT_LINELIST ? 2 : 3);

	rslt = _pDevice->DrawPrimitive(primitive_type, 0, num_primitives);
	if (FAILED(rslt))		ThrowD3DException(rslt, L"DrawPrimitive() failed");
}

// ****************************************************************************************************************

Camera::Camera(DeviceD3D& d)
:	_dev(d)
{
	Reset();
}

void Camera::Reset()
{
	_rotX = 22;
	_rotY = 135;
	_scale = 1;

	CalcCamera();
}

void Camera::RefreshPos()
{
	MyVector look_at(0, 0, 0);
	_dev.SetView(_camera_pos, look_at, _up);
}

void Camera::CalcCamera()
{
	MyMatrix mrotx(MyMatrix::RotationX(float(_rotX/180*M_PI)));
	MyMatrix mroty(MyMatrix::RotationY(float(_rotY/180*M_PI)));
	MyMatrix mscale(MyMatrix::Scaling(_scale, _scale, _scale));
	MyMatrix mfinal(mrotx * mroty * mscale);

	MyVector cpos(0, 0, -2500);
	_camera_pos = cpos.Transform(mfinal);

	MyVector up(0,1,0);
	_up = up.Transform(mfinal);
}

void Camera::SetRotation(float deltaY, float deltaX)
{
	_rotY += deltaY / 3;
	_rotX += deltaX / 3;

	CalcCamera();
}

void Camera::Zoom(const int zoom)
{
	_scale *= float(zoom > 0 ? 0.9 : 1.1);

	CalcCamera();
}

// ****************************************************************************************************************

void BuildCube(std::vector<Vertex3D>& v, float Width, float Height, float Depth, float x, float y, float z, D3DCOLOR col)
{
	MyVector p[8] = {	MyVector( x,       y+Height,        z),
						MyVector( x+Width, y+Height,        z),
						MyVector( x,              y,        z),
						MyVector( x+Width,        y,        z),
						MyVector( x,       y+Height,  z+Depth),
						MyVector( x+Width, y+Height,  z+Depth),
						MyVector( x,              y,  z+Depth),
						MyVector( x+Width,        y,  z+Depth) };

	v.insert(v.end(), 36, Vertex3D());

	std::vector<Vertex3D>::iterator vp(v.end() - 36);

	// front
	vp[ 0].pos=p[0];		vp[ 1].pos=p[1];		vp[ 2].pos=p[2];
	vp[ 3].pos=p[2];		vp[ 4].pos=p[1];		vp[ 5].pos=p[3];
	// back
	vp[ 6].pos=p[5];		vp[ 7].pos=p[4];		vp[ 8].pos=p[7];
	vp[ 9].pos=p[7];		vp[10].pos=p[4];		vp[11].pos=p[6];
	// top
	vp[12].pos=p[4];		vp[13].pos=p[5];		vp[14].pos=p[0];
	vp[15].pos=p[0];		vp[16].pos=p[5];		vp[17].pos=p[1];
	// bottom
	vp[18].pos=p[2];		vp[19].pos=p[3];		vp[20].pos=p[6];
	vp[21].pos=p[6];		vp[22].pos=p[3];		vp[23].pos=p[7];
	// left
	vp[24].pos=p[4];		vp[25].pos=p[0];		vp[26].pos=p[6];
	vp[27].pos=p[6];		vp[28].pos=p[0];		vp[29].pos=p[2];
	// right
	vp[30].pos=p[1];		vp[31].pos=p[5];		vp[32].pos=p[3];
	vp[33].pos=p[3];		vp[34].pos=p[5];		vp[35].pos=p[7];

	std::for_each(vp, v.end(), [&](Vertex3D& v) { v.diffuse = col; } );
}

void BuildCube(std::vector<Vertex3D>& v, float Width, float Height, float Depth, D3DCOLOR col)
{
	float x = -Width/2;
	float y = -Height/2;
	float z = -Depth/2;

	BuildCube(v, Width, Height, Depth, x, y, z, col);
}
