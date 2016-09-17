#pragma once

#include "my_win.h"
#include "d3d.h"

#include "EllipsoidFit.h"

class WHTDialog;
class WHTDongle;

class MagCalibDialog: public Dialog
{
private:
	// samples group
	Label		_lbl_num_samples;
	CheckBox	_chk_filter_samples;
	Button		_btn_toggle_sampling;
	Button		_btn_load_raw_points;
	Button		_btn_save_raw_points;
	Button		_btn_clear_all_samples;

	// calibration group
	CheckBox	_chk_show_dongle_calib_points;
	Button		_btn_calc_calibration;
	Label		_lbl_center_x_calc;
	Label		_lbl_center_y_calc;
	Label		_lbl_center_z_calc;
	Label		_lbl_radius_x;
	Label		_lbl_radius_y;
	Label		_lbl_radius_z;
	Label		_lbl_matrix_1_calc;
	Label		_lbl_matrix_2_calc;
	Label		_lbl_matrix_3_calc;
	Label		_lbl_matrix_4_calc;
	Label		_lbl_matrix_5_calc;
	Label		_lbl_matrix_6_calc;

	Button		_btn_clear_calibrated_points;
	Button		_btn_send_to_dongle;

	Label		_lbl_center_x_dongle;
	Label		_lbl_center_y_dongle;
	Label		_lbl_center_z_dongle;
	Label		_lbl_matrix_1_dongle;
	Label		_lbl_matrix_2_dongle;
	Label		_lbl_matrix_3_dongle;
	Label		_lbl_matrix_4_dongle;
	Label		_lbl_matrix_5_dongle;
	Label		_lbl_matrix_6_dongle;

	// the view group
	Button		_btn_reset_camera;

	Label		_lbl_number_of_vertices;
	Label		_lbl_fps;


	Window		_d3d_window;

	Icon		_icon_large;
	Icon		_icon_small;

	Font		_fnt_bold;

	DeviceD3D	_d3d_device;
	Camera		_camera;

	// difference between D3D window size and the size of the dialog window
	size_t		_d3d_width_diff;
	size_t		_d3d_height_diff;

	// the minimal window size
	int			_dlg_min_width;
	int			_dlg_min_height;

	// how many cubes do we store in one vertex buffer?
	enum { NUM_VERTICES_PER_CUBE = 36};
	enum { NUM_VERTICES_PER_VBUFFER = 256 * NUM_VERTICES_PER_CUBE};

	// these are the vertex buffers for the three ttypes of objects
	std::vector<VertexBuffer<Vertex3D>>	_vb_mag_points[2];		// for the raw and calibrated mag measurement points
	VertexBuffer<Vertex3D>				_vb_coord_sys;			// for the lines - coordinate system and the ellipsoid axes
	VertexBuffer<Vertex3D>				_vb_ellipsoid_axes;		// for the lines - coordinate system and the ellipsoid axes

	// the D3D objects
	CoordSys					_coord_sys;			// the coordinate axes
	EllipsoidAxes				_ellipsoid_axes;

	std::vector<mag_point_t>	_mag_raw_points;
	int							_calib_start_from_point;

	EllipsoidFit	_ellipsoid_fit;
	bool			_is_ellipsoid_fit_valid;		// true if the members of _ellipsoid_fit are valid

	bool			_is_dongle_calib_read;
	bool			_is_hmc5883l;

	bool			_is_collecting_raw_samples;		// true if we're collecting raw samples

	int				_last_x, _last_y;				// used to calculate mouse movement delta
	bool			_is_dragging;

	// loaded magnetometer calibration settings from the dongle
	Point<double>	_mag_offset;
	double			_mag_matrix[3][3];

	WHTDongle&		_dongle;
	WHTDialog&		_wht_dlg;

	MouseCapture	_mouse_trap;

	// adds the point to the set if needed, and creates a 3D cube with
	void AddPoint(const mag_point_t& p, const bool is_raw);
	void UnlockVertexBuffers();

	size_t CalcVertexCount();

	// called after the window size has been changed and we need to reset D3D
	void UpdateD3DSize();

	void ClearSamples();
	void ClearCalibratedPoints();

	void SaveData();		// save/load the raw sampled data to/from a CSV file
	void LoadData();

	void LoadRawDataFromFile(const std::wstring& filename);

	void CalcEllipsoidFit();

	// called from the timer to refresh the data controls
	void RefreshControls();
	void ReadMagCalibration();

	// accelerator triggered stuff
	void GenerateTestData();
	void ResetMagCalibration();

	// turn raw sampling on or off
	void ToggleSampling();

	// the checkbox event
	void ToggleShowDongleCalibPoints();

	// rotating the D3D view
	void BeginDragging();
	void EndDragging();

	// send the calibration values to the dongle
	// the center offset and the transformation matrix
	void SendCalibToDongle();

	// reads the raw mag samples from the dongle
	void ProcessRawMagSamples();

	enum { FPS_CALC_SECONDS = 2 };

	std::vector<size_t>	frame_times;
	void AddFrame();
	int CalcFPS();

	bool HasLoadedCalibration() const
	{
		return _mag_matrix[0][0] != 1  ||  _mag_matrix[0][1] != 0  ||  _mag_matrix[0][2] != 0;
	}

public:

	MagCalibDialog(WHTDongle& dngl, WHTDialog& dlg);

	virtual ~MagCalibDialog()		{}

	virtual void OnInit();
	virtual void OnDestroy();
	virtual void OnSize(int width, int height, WPARAM wParam);
	virtual void OnSizing(int which_edge, LPRECT lpRect);
	virtual void OnException(const std::wstring& str);
	virtual void OnTimer(int timerID);
	virtual void OnControl(int ctrlID, int notifyID, HWND hWndCtrl);

	// view transformation
	virtual void OnLButtonDown(int x, int y, WPARAM wParam);
	virtual void OnLButtonUp(int x, int y, WPARAM wParam);
	virtual void OnMouseMove(int x, int y, WPARAM wParam);
	virtual void OnMouseWheel(int x, int y, int delta, WPARAM wParam);
	virtual void OnKeyDown(size_t vkey_code);
	virtual void OnAccelerator(int acceleratorID);

	void Init3D();
	void Render();
};
