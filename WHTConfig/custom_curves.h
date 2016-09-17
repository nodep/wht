#pragma once

struct CurveDot
{
	int16_t		x;		// reading from the sensor
	int16_t		y;		// output

	float		win_x;
	float		win_y;

	CurveDot(int16_t nx, int16_t ny)
		: x(nx), y(ny), win_x(0), win_y(0)
	{}

	std::pair<float, float> CalcWindowCoord(const std::pair<int, int>& d3d_size, const int edge_pixels);
};

class CustomCurvesDialog;

class CurveEditor
{
private:
	Window		_parent_dialog;
	Window		_d3d_window;

	DeviceD3D	_d3d_device;

	std::pair<int, int>			_d3d_size;

	// difference between D3D window size and the size of it's parent dialog
	std::pair<int, int>			_d3d_size_diff;

	Rectangle2D					_rect;
	VertexBuffer<Vertex2D>		_rect_vbuff;

	LineList					_coord_lines;
	VertexBuffer<Vertex2D>		_coord_lines_vbuff;

	LineList					_curve_lines;
	VertexBuffer<Vertex2D>		_curve_lines_vbuff;

	std::vector<DotMarker>		_dot_markers;
	VertexBuffer<Vertex2D>		_dot_markers_vbuff;

	// the actual data we're editing displaying and editing
	std::vector<CurveDot>		_dots;

	void DrawCurve();
	void UpdateD3DSize();
	void RestartD3D();
	void CreateRectArea();

	enum {
		EDGE_PIXELS = 8,
		NUM_OF_DOTS = 16,
	};

public:
	CurveEditor()
	{}

	void Init(CustomCurvesDialog& parent_dlg, const std::pair<int, int>& d3d_size_diff);
	void Resized(WPARAM wParam);
	void MouseMoved(const std::pair<int, int>& mouse_pos, const bool is_ldown);
	void MouseLDown(const std::pair<int, int>& mouse_pos);
	void MouseLUp(const std::pair<int, int>& mouse_pos);
	void Render();
};

class WHTDialog;

class CustomCurvesDialog: public Dialog
{
private:
	Button			_btn_ok;
	Button			_btn_cancel;

	WHTDongle&		_dongle;
	WHTDialog&		_main_dlg;

	CurveEditor		_curve_editor;

	// the position of the D3D window
	std::pair<int, int>	_d3d_pos;

	// the minimal window size
	int			_dlg_min_width;
	int			_dlg_min_height;

public:
	CustomCurvesDialog(WHTDongle& dngl, WHTDialog& dlg);

	virtual ~CustomCurvesDialog()		{}

	virtual void OnInit();
	virtual void OnDestroy();
	virtual void OnSize(int width, int height, WPARAM wParam);
	virtual void OnException(const std::wstring& str);
	virtual void OnControl(int ctrlID, int notifyID, HWND hWndCtrl);
	virtual void OnMouseMove(int x, int y, WPARAM wParam);
	virtual void OnLButtonDown(int x, int y, WPARAM wParam);
	virtual void OnLButtonUp(int x, int y, WPARAM wParam);
	virtual void OnSizing(int which_edge, LPRECT lpRect);

	void Render()
	{
		_curve_editor.Render();
	}
};