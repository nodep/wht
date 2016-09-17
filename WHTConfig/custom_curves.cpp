#include "stdafx.h"

#include "resource.h"

#include "../dongle/reports.h"

#include "my_utils.h"
#include "my_win.h"
#include "my_d3dx.h"
#include "d3d.h"
#include "d3d_objects.h"
#include "gyro_accel_dialog.h"
#include "mag_calib_dialog.h"
#include "custom_curves.h"
#include "raw_input.h"
#include "wht_dongle.h"
#include "gyro_accel_dialog.h"
#include "wht_dialog.h"

#define DMCOL_DEFAULT		D3DCOLOR_XRGB(0x33, 0xB8, 0x3C)
#define DMCOL_HILITE		D3DCOLOR_XRGB(0xED, 0x9A, 0x09)

#define DMCOL_EDGE_BGND		D3DCOLOR_XRGB(0x20, 0x20, 0x20)
#define DMCOL_CURVE_BGND	D3DCOLOR_XRGB(0, 0, 145)
#define DMCOL_COORD_LINES	D3DCOLOR_XRGB(100, 100, 145)

#define DMCOL_CURVE_LINES	D3DCOLOR_XRGB(250, 120, 120)

CustomCurvesDialog::CustomCurvesDialog(WHTDongle& dngl, WHTDialog& dlg)
:	_dongle(dngl), _main_dlg(dlg),
	_dlg_min_width(0),
	_dlg_min_height(0)
{}

void CustomCurvesDialog::OnInit()
{
	WaitCursor wc;

	// remember the size of the D3D window relative to the size of the dialog
	RECT dlgrect, dxrect;
	GetRect(dlgrect);

	// remember the dialog size
	_dlg_min_width = dlgrect.right - dlgrect.left;
	_dlg_min_height = dlgrect.bottom - dlgrect.top;

	Window d3d_win;
	d3d_win.SetHandle(GetCtrl(IDC_D3D));
	d3d_win.GetRect(dxrect);

	std::pair<int, int> d3d_size_diff;
	d3d_size_diff.first = dlgrect.right - dlgrect.left - dxrect.right + dxrect.left;
	d3d_size_diff.second = dlgrect.bottom - dlgrect.top - dxrect.bottom + dxrect.top;

	// screen to client coordinates
	POINT pt;
	pt.x = dxrect.left;
	pt.y = dxrect.top;
	ScreenToClient(this->GetHandle(), &pt);
	_d3d_pos.first = pt.x;
	_d3d_pos.second = pt.y;

	_curve_editor.Init(*this, d3d_size_diff);
}

void CustomCurvesDialog::OnDestroy()
{
}

void CustomCurvesDialog::OnSize(int width, int height, WPARAM wParam)
{
	WaitCursor cwc;

	_curve_editor.Resized(wParam);
}

void CustomCurvesDialog::OnException(const std::wstring& str)
{
	MsgBox(str, L"Exception", MB_OK | MB_ICONERROR);
}

void CustomCurvesDialog::OnControl(int ctrlID, int notifyID, HWND hWndCtrl)
{
	//if (ctrlID == IDOK)
	//	Close();
}

void CustomCurvesDialog::OnMouseMove(int x, int y, WPARAM wParam)
{
	std::pair<int, int> mouse_pos(x - _d3d_pos.first, y - _d3d_pos.second);
	if (mouse_pos.first >= 0  &&  mouse_pos.second >= 0)
		_curve_editor.MouseMoved(mouse_pos, wParam & MK_LBUTTON);
}

void CustomCurvesDialog::OnLButtonDown(int x, int y, WPARAM wParam)
{
	std::pair<int, int> mouse_pos(x - _d3d_pos.first, y - _d3d_pos.second);
	if (mouse_pos.first >= 0  &&  mouse_pos.second >= 0)
		_curve_editor.MouseLDown(mouse_pos);
}

void CustomCurvesDialog::OnLButtonUp(int x, int y, WPARAM wParam)
{
	std::pair<int, int> mouse_pos(x - _d3d_pos.first, y - _d3d_pos.second);
	if (mouse_pos.first >= 0  &&  mouse_pos.second >= 0)
		_curve_editor.MouseLUp(mouse_pos);
}

void CustomCurvesDialog::OnSizing(int which_edge, LPRECT lpRect)
{
	if (lpRect->right - lpRect->left < _dlg_min_width)
	{
		if (which_edge == WMSZ_BOTTOMRIGHT
					||  which_edge == WMSZ_TOPRIGHT
					||  which_edge == WMSZ_RIGHT)
			lpRect->right = lpRect->left + _dlg_min_width;
		else
			lpRect->left = lpRect->right - _dlg_min_width;
	}

	if (lpRect->bottom - lpRect->top < _dlg_min_height)
	{
		if (which_edge == WMSZ_BOTTOMRIGHT
					||  which_edge == WMSZ_BOTTOMLEFT
					||  which_edge == WMSZ_BOTTOM)
			lpRect->bottom = lpRect->top + _dlg_min_height;
		else
			lpRect->top = lpRect->bottom - _dlg_min_height;
	}
}

// *********************************************************************************

std::pair<float, float> CurveDot::CalcWindowCoord(const std::pair<int, int>& d3d_size, const int edge_pixels)
{
	std::pair<float, float> ret_val;

	int width	= d3d_size.first - edge_pixels*2;
	int height	= d3d_size.second - edge_pixels*2;

	ret_val.first = float(edge_pixels + width*(x + 32767) / 65535.0);
	ret_val.second = float(edge_pixels + height*(32767 - y) / 65535.0);

	return ret_val;
}

// *********************************************************************************

void CurveEditor::Init(CustomCurvesDialog& parent_dlg, const std::pair<int, int>& d3d_size_diff)
{
	_parent_dialog = parent_dlg;
	_d3d_window.SetHandle(parent_dlg.GetCtrl(IDC_D3D));
	_d3d_size_diff = d3d_size_diff;

	const float POINT_START = -32767;
	const float POINT_STEP = 65534.0F / (NUM_OF_DOTS - 1);

	// make some test curve dots
	_dots.clear();
	for (int c = 0; c < NUM_OF_DOTS; ++c)
		_dots.push_back(CurveDot(int16_t(POINT_START + c*POINT_STEP), int16_t(POINT_START + c*POINT_STEP)));

	RestartD3D();
}

void CurveEditor::CreateRectArea()
{
	// recreate the work rect
	_rect.Build(EDGE_PIXELS, EDGE_PIXELS, _d3d_size.first - EDGE_PIXELS*2, _d3d_size.second - EDGE_PIXELS*2, DMCOL_CURVE_BGND);

	_rect_vbuff.Alloc(_d3d_device, 6);
	_rect_vbuff.Lock();
	_rect_vbuff.AddObject(_rect);
	_rect_vbuff.Unlock();

	// the coordinate lines
	_coord_lines.Clear();

	_coord_lines.Add(	Vertex2D(_d3d_size.first / float(2), EDGE_PIXELS, DMCOL_COORD_LINES),
						Vertex2D(_d3d_size.first / float(2), _d3d_size.second - float(EDGE_PIXELS), DMCOL_COORD_LINES));

	_coord_lines.Add(	Vertex2D(EDGE_PIXELS, _d3d_size.second / float(2), DMCOL_COORD_LINES),
						Vertex2D(_d3d_size.first - float(EDGE_PIXELS), _d3d_size.second / float(2), DMCOL_COORD_LINES));

	_coord_lines_vbuff.Alloc(_d3d_device, _coord_lines.GetVertices().size());
	_coord_lines_vbuff.Lock();
	_coord_lines_vbuff.AddObject(_coord_lines);
	_coord_lines_vbuff.Unlock();
}

void CurveEditor::DrawCurve()
{
	// the lines
	_curve_lines.Clear();

	_curve_lines_vbuff.Clear();
	_curve_lines_vbuff.Alloc(_d3d_device, NUM_OF_DOTS*2);
	_curve_lines_vbuff.Lock();

	// the markers
	_dot_markers_vbuff.Clear();
	_dot_markers_vbuff.Alloc(_d3d_device, 24*NUM_OF_DOTS + 6);
	_dot_markers_vbuff.Lock();

	_dot_markers.insert(_dot_markers.end(), _dots.size() - _dot_markers.size(), DotMarker());

	// make the markers and the lines
	std::pair<float, float> last_coord;
	for (size_t i = 0; i < _dots.size(); ++i)
	{
		std::pair<float, float> coord(_dots[i].CalcWindowCoord(_d3d_size, EDGE_PIXELS));

		if (i > 0)
		{
			_curve_lines.Add(	Vertex2D(last_coord.first, last_coord.second, DMCOL_CURVE_LINES),
								Vertex2D(coord.first, coord.second, DMCOL_CURVE_LINES));
		}

		last_coord = coord;

		if (_dot_markers.size() < i)
			_dot_markers.push_back(DotMarker());

		_dot_markers[i].Build(coord.first, coord.second, DMCOL_DEFAULT, 6, 2);

		_dot_markers_vbuff.AddObject(_dot_markers[i]);
	}

	_curve_lines_vbuff.AddObject(_curve_lines);

	_dot_markers_vbuff.Unlock();
	_curve_lines_vbuff.Unlock();
}

void CurveEditor::Render()
{
    // return if the device is not ready
    if (!_d3d_device.IsValid())
		return;

    // Clear the back buffer
	_d3d_device.Clear(false, DMCOL_EDGE_BGND);

	_d3d_device.BeginScene();

	_d3d_device.DrawVertices(_rect_vbuff, D3DPT_TRIANGLELIST);
	_d3d_device.DrawVertices(_curve_lines_vbuff, D3DPT_LINELIST);
	_d3d_device.DrawVertices(_dot_markers_vbuff, D3DPT_TRIANGLELIST);
	_d3d_device.DrawVertices(_coord_lines_vbuff, D3DPT_LINELIST);

	_d3d_device.EndScene();

    // present the back buffer to the display adapter to be drawn
    _d3d_device.Present();
}

void CurveEditor::RestartD3D()
{
	_d3d_device.Init2D(_d3d_window);

	UpdateD3DSize();
	CreateRectArea();

	DrawCurve();
}

void CurveEditor::UpdateD3DSize()
{
	RECT dlgrect;
	int dlgw, dlgh;
	
	_parent_dialog.GetRect(dlgrect);
	dlgw = dlgrect.right - dlgrect.left;
	dlgh = dlgrect.bottom - dlgrect.top;

	_d3d_size.first = dlgw - _d3d_size_diff.first;
	_d3d_size.second = dlgh - _d3d_size_diff.second;

	// set the D3D window size
	SetWindowPos(_d3d_window.GetHandle(), HWND_TOP, 0, 0, _d3d_size.first, _d3d_size.second, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void CurveEditor::Resized(WPARAM wParam)
{
	UpdateD3DSize();

	// re-init Direct3D
	_dot_markers_vbuff.Release();
	_rect_vbuff.Release();
	_d3d_device.Release();

	// init d3d if we're not minimized
	if (wParam != SIZE_MINIMIZED)
		RestartD3D();
}

void CurveEditor::MouseMoved(const std::pair<int, int>& mouse_pos, const bool is_ldown)
{
}

void CurveEditor::MouseLDown(const std::pair<int, int>& mouse_pos)
{
}

void CurveEditor::MouseLUp(const std::pair<int, int>& mouse_pos)
{
}
