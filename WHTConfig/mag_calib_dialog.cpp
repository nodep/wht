#include "stdafx.h"
#pragma hdrstop

#include "resource.h"
#include "../dongle/reports.h"

#include "my_utils.h"
#include "my_win.h"
#include "my_d3dx.h"
#include "d3d.h"
#include "d3d_objects.h"
#include "gyro_accel_dialog.h"
#include "mag_calib_dialog.h"
#include "d3d_objects.h"
#include "custom_curves.h"
#include "raw_input.h"
#include "wht_dongle.h"

#include "wht_dialog.h"

#include "EllipsoidFit.h"

#define NUM_SAMPLES_FOR_ELLIPSOID_FIT		400

MagCalibDialog::MagCalibDialog(WHTDongle& dngl, WHTDialog& dlg)
:	_icon_large(IDI_MAGNET, true),
	_icon_small(IDI_MAGNET, false),
	_camera(_d3d_device),
	_dongle(dngl),
	_calib_start_from_point(0),
	_wht_dlg(dlg),
	_is_hmc5883l(false)
{
	_fnt_bold.SetAll(L"Arial", 15, true);
}

void MagCalibDialog::OnInit()
{
	WaitCursor wc;

	SetIcon(_icon_small);
	SetIcon(_icon_large);

	_is_dragging = false;
	_is_collecting_raw_samples = false;
	_is_ellipsoid_fit_valid = false;
	_is_dongle_calib_read = false;

	_mag_offset.x = _mag_offset.y = _mag_offset.z = 0;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
				_mag_matrix[i][j] = j == i ? 1 : 0;

	_lbl_num_samples.SetHandle(GetCtrl(IDC_LBL_NUM_SAMPLES));
	_btn_load_raw_points.SetHandle(GetCtrl(IDC_BTN_LOAD_RAW_POINTS));
	_btn_save_raw_points.SetHandle(GetCtrl(IDC_BTN_SAVE_RAW_POINTS));
	_btn_toggle_sampling.SetHandle(GetCtrl(IDC_BTN_TOGGLE_SAMPLING));
	_btn_clear_all_samples.SetHandle(GetCtrl(IDC_BTN_CLEAR_ALL_SAMPLES));
	_btn_clear_calibrated_points.SetHandle(GetCtrl(IDC_BTN_CLEAR_CALIBRATED_POINTS));
	_chk_show_dongle_calib_points.SetHandle(GetCtrl(IDC_CHK_SHOW_DONGLE_CALIB_POINTS));
	_btn_calc_calibration.SetHandle(GetCtrl(IDC_BTN_CALCULATE_CALIBRATION));
	_btn_send_to_dongle.SetHandle(GetCtrl(IDC_BTN_SEND_TO_DONGLE));
	_btn_reset_camera.SetHandle(GetCtrl(IDC_BTN_RESET_CAMERA));

	_chk_filter_samples.SetHandle(GetCtrl(IDC_CHK_FILTER_SAMPLES));
	_chk_filter_samples.SetState(true);

	_lbl_center_x_calc.SetHandle(GetCtrl(IDC_LBL_CENTER_X_CALC));
	_lbl_center_y_calc.SetHandle(GetCtrl(IDC_LBL_CENTER_Y_CALC));
	_lbl_center_z_calc.SetHandle(GetCtrl(IDC_LBL_CENTER_Z_CALC));

	_lbl_radius_x.SetHandle(GetCtrl(IDC_LBL_RADIUS_X));
	_lbl_radius_y.SetHandle(GetCtrl(IDC_LBL_RADIUS_Y));
	_lbl_radius_z.SetHandle(GetCtrl(IDC_LBL_RADIUS_Z));

	_lbl_center_x_calc.SetHandle(GetCtrl(IDC_LBL_CENTER_X_CALC));
	_lbl_center_y_calc.SetHandle(GetCtrl(IDC_LBL_CENTER_Y_CALC));
	_lbl_center_z_calc.SetHandle(GetCtrl(IDC_LBL_CENTER_Z_CALC));
	_lbl_matrix_1_calc.SetHandle(GetCtrl(IDC_LBL_MATRIX_1_CALC));
	_lbl_matrix_2_calc.SetHandle(GetCtrl(IDC_LBL_MATRIX_2_CALC));
	_lbl_matrix_3_calc.SetHandle(GetCtrl(IDC_LBL_MATRIX_3_CALC));
	_lbl_matrix_4_calc.SetHandle(GetCtrl(IDC_LBL_MATRIX_4_CALC));
	_lbl_matrix_5_calc.SetHandle(GetCtrl(IDC_LBL_MATRIX_5_CALC));
	_lbl_matrix_6_calc.SetHandle(GetCtrl(IDC_LBL_MATRIX_6_CALC));

	_lbl_center_x_dongle.SetHandle(GetCtrl(IDC_LBL_CENTER_X_DONGLE));
	_lbl_center_y_dongle.SetHandle(GetCtrl(IDC_LBL_CENTER_Y_DONGLE));
	_lbl_center_z_dongle.SetHandle(GetCtrl(IDC_LBL_CENTER_Z_DONGLE));
	_lbl_matrix_1_dongle.SetHandle(GetCtrl(IDC_LBL_MATRIX_1_DONGLE));
	_lbl_matrix_2_dongle.SetHandle(GetCtrl(IDC_LBL_MATRIX_2_DONGLE));
	_lbl_matrix_3_dongle.SetHandle(GetCtrl(IDC_LBL_MATRIX_3_DONGLE));
	_lbl_matrix_4_dongle.SetHandle(GetCtrl(IDC_LBL_MATRIX_4_DONGLE));
	_lbl_matrix_5_dongle.SetHandle(GetCtrl(IDC_LBL_MATRIX_5_DONGLE));
	_lbl_matrix_6_dongle.SetHandle(GetCtrl(IDC_LBL_MATRIX_6_DONGLE));

	_lbl_number_of_vertices.SetHandle(GetCtrl(IDC_LBL_NUMBER_OF_VERTICES));
	_lbl_fps.SetHandle(GetCtrl(IDC_LBL_FPS));

	_d3d_window.SetHandle(GetCtrl(IDC_D3D));

	// the fonts
	_lbl_num_samples.SetFont(_fnt_bold);
	_lbl_center_x_calc.SetFont(_fnt_bold);
	_lbl_center_y_calc.SetFont(_fnt_bold);
	_lbl_center_z_calc.SetFont(_fnt_bold);
	_lbl_radius_x.SetFont(_fnt_bold);
	_lbl_radius_y.SetFont(_fnt_bold);
	_lbl_radius_z.SetFont(_fnt_bold);
	_lbl_matrix_1_calc.SetFont(_fnt_bold);
	_lbl_matrix_2_calc.SetFont(_fnt_bold);
	_lbl_matrix_3_calc.SetFont(_fnt_bold);
	_lbl_matrix_4_calc.SetFont(_fnt_bold);
	_lbl_matrix_5_calc.SetFont(_fnt_bold);
	_lbl_matrix_6_calc.SetFont(_fnt_bold);
	_lbl_center_x_dongle.SetFont(_fnt_bold);
	_lbl_center_y_dongle.SetFont(_fnt_bold);
	_lbl_center_z_dongle.SetFont(_fnt_bold);
	_lbl_matrix_1_dongle.SetFont(_fnt_bold);
	_lbl_matrix_2_dongle.SetFont(_fnt_bold);
	_lbl_matrix_3_dongle.SetFont(_fnt_bold);
	_lbl_matrix_4_dongle.SetFont(_fnt_bold);
	_lbl_matrix_5_dongle.SetFont(_fnt_bold);
	_lbl_matrix_6_dongle.SetFont(_fnt_bold);
	_lbl_number_of_vertices.SetFont(_fnt_bold);
	_lbl_fps.SetFont(_fnt_bold);

	RefreshControls();
	
	// set the device adapter name in the title
	SetText("Magnetometer calibration on " + GetD3D().GetAdapterName());

	// remember the size of the D3D window relative to the size of the dialog
	RECT dlgrect, dxrect;
	GetRect(dlgrect);
	_d3d_window.GetRect(dxrect);

	// remember the dialog size
	_dlg_min_width = dlgrect.right - dlgrect.left;
	_dlg_min_height = dlgrect.bottom - dlgrect.top;

	_d3d_width_diff = _dlg_min_width - dxrect.right + dxrect.left;
	_d3d_height_diff = _dlg_min_height - dxrect.bottom + dxrect.top;

	// init the Direct3D device
	_d3d_device.Init(_d3d_window);
	Init3D();

	_camera.Reset();

	// start the refresh timer
	SetTimer(GetHandle(), 1, 100, NULL);

	// reset the FPS calc
	frame_times.clear();

	_btn_toggle_sampling.SetFocus();
}

void MagCalibDialog::OnException(const std::wstring& str)
{
	MsgBox(str, L"Exception", MB_OK | MB_ICONERROR);
}

void MagCalibDialog::OnKeyDown(size_t vkey_code)
{
	debug(vkey_code);
}

void MagCalibDialog::OnDestroy()
{
	// in case the mouse is still trapped
	_mouse_trap.Release();

	// release the vertices and the D3D device
	// the Direct3D object stays alive
	ClearSamples();
	_d3d_device.Release();
}

void MagCalibDialog::ClearSamples()
{
	_mag_raw_points.clear();

	// clear the vertex buffers
	_vb_mag_points[0].clear();
	_vb_mag_points[1].clear();

	// and default empty vertex buffers
	_vb_mag_points[0].push_back(VertexBuffer<Vertex3D>());
	_vb_mag_points[0].back().Alloc(_d3d_device, NUM_VERTICES_PER_VBUFFER);

	_vb_mag_points[1].push_back(VertexBuffer<Vertex3D>());
	_vb_mag_points[1].back().Alloc(_d3d_device, NUM_VERTICES_PER_VBUFFER);

	_is_ellipsoid_fit_valid = false;
	_calib_start_from_point = 0;
}

void MagCalibDialog::OnSize(int width, int height, WPARAM wParam)
{
	WaitCursor cwc;

	UpdateD3DSize();

	// re-init Direct3D
	_vb_mag_points[0].clear();
	_vb_mag_points[1].clear();

	_vb_coord_sys.Release();
	_vb_ellipsoid_axes.Release();

	_d3d_device.Release();

	// init d3d if we're not minimized
	if (wParam != SIZE_MINIMIZED)
	{
		_d3d_device.Init(_d3d_window);

		Init3D();
	}
}

void MagCalibDialog::OnSizing(int which_edge, LPRECT lpRect)
{
	const int WIDTH_MARGIN = 200;

	if (lpRect->right - lpRect->left < _dlg_min_width - WIDTH_MARGIN)
	{
		if (which_edge == WMSZ_BOTTOMRIGHT
					||  which_edge == WMSZ_TOPRIGHT
					||  which_edge == WMSZ_RIGHT)
			lpRect->right = lpRect->left + _dlg_min_width - WIDTH_MARGIN;
		else
			lpRect->left = lpRect->right - _dlg_min_width - WIDTH_MARGIN;
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

void MagCalibDialog::UpdateD3DSize()
{
	RECT dlgrect;
	int dlgw, dlgh, dxw, dxh;
	
	GetRect(dlgrect);
	dlgw = dlgrect.right - dlgrect.left;
	dlgh = dlgrect.bottom - dlgrect.top;

	dxw = dlgw - _d3d_width_diff;
	dxh = dlgh - _d3d_height_diff;

	// set the D3D window size
	SetWindowPos(_d3d_window.GetHandle(), HWND_TOP, 0, 0, dxw, dxh, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

void MagCalibDialog::UnlockVertexBuffers()
{
	if (_vb_mag_points[0].back().IsLocked())
		_vb_mag_points[0].back().Unlock();

	if (_vb_mag_points[1].back().IsLocked())
		_vb_mag_points[1].back().Unlock();

	if (_vb_coord_sys.IsLocked())
		_vb_coord_sys.Unlock();

	if (_vb_ellipsoid_axes.IsLocked())
		_vb_ellipsoid_axes.Unlock();
}

void MagCalibDialog::Render()
{
    // return if the device is not ready
    if (!_d3d_device.IsValid())
		return;

	// unlock all the locked vertex buffers
	UnlockVertexBuffers();

    // Clear the back buffer
	_d3d_device.Clear(true);

	_d3d_device.BeginScene();

	// should we use light?
	//if (UseLighting())
	//	_d3d_device.EnableLight();
	//else
	//	_d3d_device.DisableLight();

	// set the current view transformation
	_camera.RefreshPos();

	// render our line based objects (the coordinate axes and the ellipsoid axes)
	_d3d_device.DrawVertices(_vb_coord_sys, D3DPT_LINELIST);

	if (_is_ellipsoid_fit_valid)
		_d3d_device.DrawVertices(_vb_ellipsoid_axes, D3DPT_LINELIST);

	// render our triangle based objects

	// the raw mag
	std::for_each(_vb_mag_points[0].begin(), _vb_mag_points[0].end(), [&] (VertexBuffer<Vertex3D>& vb) { _d3d_device.DrawVertices(vb, D3DPT_TRIANGLELIST); } );

	// the calibrated mag
	std::for_each(_vb_mag_points[1].begin(), _vb_mag_points[1].end(), [&] (VertexBuffer<Vertex3D>& vb) { _d3d_device.DrawVertices(vb, D3DPT_TRIANGLELIST); } );

	_d3d_device.EndScene();

    // present the back buffer to the display adapter to be drawn
    _d3d_device.Present();

	// the FPS calculation
	AddFrame();
}

void MagCalibDialog::Init3D()
{
	// we need the aspect ratio of the window to calculate the perspective
	RECT dxrect;
	_d3d_window.GetRect(dxrect);

	// create a left handed cartesian Field Of View transform
	MyMatrix matProjection(MyMatrix::PerspectiveFovLH(	float(M_PI) / 4,
														(float) (dxrect.right - dxrect.left) / (dxrect.bottom - dxrect.top),
														1.0,
														100000.0));

	// tell D3D to use our Projection matrix for the projection transformation stage
	_d3d_device.SetProjectionTransform(matProjection);

	_d3d_device.SetCulling(D3DCULL_NONE);

	// and 3D points for the cubes
	_vb_mag_points[0].push_back(VertexBuffer<Vertex3D>());
	_vb_mag_points[0].back().Alloc(_d3d_device, NUM_VERTICES_PER_VBUFFER);

	_vb_mag_points[1].push_back(VertexBuffer<Vertex3D>());
	_vb_mag_points[1].back().Alloc(_d3d_device, NUM_VERTICES_PER_VBUFFER);

	// re-create the raw mag points
	std::for_each(_mag_raw_points.begin(), _mag_raw_points.end(), [&] (const mag_point_t& p) { AddPoint(p, true); } );

	// and the calibrated points too
	if (_is_ellipsoid_fit_valid  ||  HasLoadedCalibration())
		std::for_each(_mag_raw_points.begin() + _calib_start_from_point, _mag_raw_points.end(), [&] (const mag_point_t& p) { AddPoint(p, false); } );

	// add the coordinate system to the vertex buffer
	_coord_sys.Build();
	_vb_coord_sys.Alloc(_d3d_device, 32);
	_vb_coord_sys.Lock();
	_vb_coord_sys.AddObject(_coord_sys);
	_vb_coord_sys.Unlock();

	// create the vertex buffers for the ellipsoid axes
	_vb_ellipsoid_axes.Alloc(_d3d_device, 32);
	if (_is_ellipsoid_fit_valid)
	{
		_vb_ellipsoid_axes.Lock();
		_vb_ellipsoid_axes.AddObject(_ellipsoid_axes);
		_vb_ellipsoid_axes.Unlock();
	}
}

void MagCalibDialog::RefreshControls()
{
	if (!_dongle.IsOpen())
	{
		_is_collecting_raw_samples = false;
		_is_dongle_calib_read = false;
	}

	_lbl_num_samples.SetText(int2str_th(_mag_raw_points.size()));

	_btn_calc_calibration.Enable(_mag_raw_points.size() >= NUM_SAMPLES_FOR_ELLIPSOID_FIT);

	if (_is_collecting_raw_samples)
		_btn_toggle_sampling.SetText(L"Stop sampling");
	else if (!_dongle.IsOpen())
		_btn_toggle_sampling.SetText(L"Dongle not found");
	else
		_btn_toggle_sampling.SetText(L"Start sampling");

	if (_dongle.IsOpen())
	{
		_btn_toggle_sampling.Enable(true);
		_btn_send_to_dongle.Enable(_is_ellipsoid_fit_valid);

		ReadMagCalibration();
	} else {
		_btn_toggle_sampling.Enable(false);
		_btn_send_to_dongle.Enable(false);
	}

	if (_is_ellipsoid_fit_valid)
	{
		_lbl_center_x_calc.SetText(round(_ellipsoid_fit.center.x));
		_lbl_center_y_calc.SetText(round(_ellipsoid_fit.center.y));
		_lbl_center_z_calc.SetText(round(_ellipsoid_fit.center.z));

		_lbl_radius_x.SetText(_ellipsoid_fit.radii[0]);
		_lbl_radius_y.SetText(_ellipsoid_fit.radii[1]);
		_lbl_radius_z.SetText(_ellipsoid_fit.radii[2]);

		_lbl_matrix_1_calc.SetText(round(_ellipsoid_fit.calibMatrix[0][0] * (1 << MAG_MATRIX_SCALE_BITS)));
		_lbl_matrix_2_calc.SetText(round(_ellipsoid_fit.calibMatrix[1][1] * (1 << MAG_MATRIX_SCALE_BITS)));
		_lbl_matrix_3_calc.SetText(round(_ellipsoid_fit.calibMatrix[2][2] * (1 << MAG_MATRIX_SCALE_BITS)));
		_lbl_matrix_4_calc.SetText(round(_ellipsoid_fit.calibMatrix[0][1] * (1 << MAG_MATRIX_SCALE_BITS)));
		_lbl_matrix_5_calc.SetText(round(_ellipsoid_fit.calibMatrix[0][2] * (1 << MAG_MATRIX_SCALE_BITS)));
		_lbl_matrix_6_calc.SetText(round(_ellipsoid_fit.calibMatrix[1][2] * (1 << MAG_MATRIX_SCALE_BITS)));

	} else {

		_lbl_center_x_calc.ClearText();
		_lbl_center_y_calc.ClearText();
		_lbl_center_z_calc.ClearText();

		_lbl_radius_x.ClearText();
		_lbl_radius_y.ClearText();
		_lbl_radius_z.ClearText();

		_lbl_matrix_1_calc.ClearText();
		_lbl_matrix_2_calc.ClearText();
		_lbl_matrix_3_calc.ClearText();
		_lbl_matrix_4_calc.ClearText();
		_lbl_matrix_5_calc.ClearText();
		_lbl_matrix_6_calc.ClearText();
	}

	_lbl_number_of_vertices.SetText(int2str_th(CalcVertexCount()));
	_lbl_fps.SetText(CalcFPS());
}

void MagCalibDialog::ReadMagCalibration()
{
	if (_is_dongle_calib_read)
		return;

	FeatRep_DongleSettings rep;

	// get the current settings
	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.GetFeatureReport(rep);

	_lbl_center_x_dongle.SetText(rep.mag_offset[0]);
	_lbl_center_y_dongle.SetText(rep.mag_offset[1]);
	_lbl_center_z_dongle.SetText(rep.mag_offset[2]);

	_lbl_matrix_1_dongle.SetText(rep.mag_matrix[0][0]);
	_lbl_matrix_2_dongle.SetText(rep.mag_matrix[1][1]);
	_lbl_matrix_3_dongle.SetText(rep.mag_matrix[2][2]);
	_lbl_matrix_4_dongle.SetText(rep.mag_matrix[0][1]);
	_lbl_matrix_5_dongle.SetText(rep.mag_matrix[0][2]);
	_lbl_matrix_6_dongle.SetText(rep.mag_matrix[1][2]);

	// remember the calibration data
	_mag_offset.x = rep.mag_offset[0];
	_mag_offset.y = rep.mag_offset[1];
	_mag_offset.z = rep.mag_offset[2];
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
			_mag_matrix[i][j] = rep.mag_matrix[i][j] / double(1 << MAG_MATRIX_SCALE_BITS);
	}

	_is_dongle_calib_read = true;
}


size_t MagCalibDialog::CalcVertexCount()
{
	size_t ret_val = _vb_coord_sys.Count() + _vb_ellipsoid_axes.Count();

	for (size_t c = 0; c < 2; c++)
	{
		for (std::vector<VertexBuffer<Vertex3D>>::const_iterator vbi(_vb_mag_points[c].begin()); vbi != _vb_mag_points[c].end(); ++vbi)
			ret_val += vbi->Count();
	}

	return ret_val;
}

void MagCalibDialog::AddFrame()
{
	// remove the frames that are too old to keep around
	const size_t now = GetTickCount();
	const size_t max_age = FPS_CALC_SECONDS * 1000;
	std::vector<size_t>::iterator
			fti(std::remove_if(frame_times.begin(), frame_times.end(),
				[now, max_age](const size_t& ft) { return now - ft > max_age; }));

	frame_times.erase(fti, frame_times.end());

	// add the new frame
	frame_times.push_back(now);
}

int MagCalibDialog::CalcFPS()
{
	// count how may frames we have that are younger than FPS_CALC_SECONDS
	// and get the oldest frame
	const size_t now = GetTickCount();
	const size_t max_age = FPS_CALC_SECONDS * 1000;
	size_t oldest = 0, young_cnt = 0;
	for (std::vector<size_t>::const_iterator fti(frame_times.begin()); fti != frame_times.end(); ++fti)
	{
		const size_t age = now - *fti;

		if (age < max_age)
			++young_cnt;

		if (age > oldest)
			oldest = age;
	}

	// if we have no frame log - avoid div by zero
	if (oldest == 0)
		return 0;

	return (young_cnt * 1000) / (oldest < max_age ? oldest : max_age);
}

void MagCalibDialog::ProcessRawMagSamples()
{
	FeatRep_RawMagSamples repMagSamples;

	repMagSamples.report_id = RAW_MAG_REPORT_ID;
	_dongle.GetFeatureReport(repMagSamples);

	const bool do_filtering = _chk_filter_samples.IsChecked();

	_is_hmc5883l = (repMagSamples.samples_desc & 0x80) == 0x80;
	const int num_samples = repMagSamples.samples_desc & 0x7f;

	for (int i = 0; i < num_samples; ++i)
	{
		const mag_point_t& mps(repMagSamples.mag[i]);

		bool add_point = true;

		if (!_mag_raw_points.empty())
		{
			// do we already have this one (the exact same point)?
			if (std::find(_mag_raw_points.begin(), _mag_raw_points.end(), mps) == _mag_raw_points.end())
			{
				// no we don't have this exact one - try with the ones close by
				if (do_filtering)
				{
					// don't add vectors that are too close to other vectors
					for (std::vector<mag_point_t>::const_iterator
								mi(_mag_raw_points.begin());
								add_point  &&  mi != _mag_raw_points.end();
								mi++)
					{
						// normalize vector
						double dist = (mps.x - mi->x)*(mps.x - mi->x) + (mps.y - mi->y)*(mps.y - mi->y) + (mps.z - mi->z)*(mps.z - mi->z);

						// distance of the points
						dist = sqrt(dist);

						// the cut-off distance for filtering
						add_point = dist > (_is_hmc5883l ? 45 : 16);
					}
				}
			} else {
				// we already have this point - no need to add it again
				add_point = false;
			}
		}

		if (add_point)
		{
			_mag_raw_points.push_back(mps);

			AddPoint(mps, true);
			if (_is_ellipsoid_fit_valid  ||  (HasLoadedCalibration()  &&  _chk_show_dongle_calib_points.IsChecked()))
				AddPoint(mps, false);
		}
	}
}

void MagCalibDialog::OnTimer(int timerID)
{
	RefreshControls();

	if (!_dongle.IsOpen())
		return;

	// get raw the mag data
	if (_is_collecting_raw_samples)
		ProcessRawMagSamples();

	_btn_load_raw_points.Enable(!_is_collecting_raw_samples);
	_btn_save_raw_points.Enable(!_is_collecting_raw_samples  &&  !_mag_raw_points.empty());
}

void MagCalibDialog::ToggleSampling()
{
	if (_dongle.IsOpen())
		_is_collecting_raw_samples = !_is_collecting_raw_samples;
	else
		_is_collecting_raw_samples = false;

	// if we are starting a new sampling session, flush the old mag samples from the buffer on the dongle
	if (_is_collecting_raw_samples)
	{
		FeatRep_RawMagSamples repMagSamples;
		repMagSamples.report_id = RAW_MAG_REPORT_ID;
		_dongle.GetFeatureReport(repMagSamples);
	}

	RefreshControls();
}

void MagCalibDialog::OnControl(int ctrlID, int notifyID, HWND hWndCtrl)
{
	if (ctrlID == IDC_BTN_CLEAR_ALL_SAMPLES)
		ClearSamples();
	else if (ctrlID == IDC_BTN_SAVE_RAW_POINTS)
		SaveData();
	else if (ctrlID == IDC_BTN_LOAD_RAW_POINTS)
		LoadData();
	else if (ctrlID == IDC_BTN_CALCULATE_CALIBRATION)
		CalcEllipsoidFit();
	else if (ctrlID == IDC_BTN_CLEAR_CALIBRATED_POINTS)
		ClearCalibratedPoints();
	else if (ctrlID == IDC_BTN_TOGGLE_SAMPLING)
		ToggleSampling();
	else if (ctrlID == IDC_BTN_RESET_CAMERA)
		_camera.Reset();
	else if (ctrlID == IDC_BTN_SEND_TO_DONGLE)
		SendCalibToDongle();
	else if (ctrlID == IDC_CHK_SHOW_DONGLE_CALIB_POINTS)
		ToggleShowDongleCalibPoints();
}

void MagCalibDialog::OnLButtonDown(int x, int y, WPARAM wParam)
{
	// get the D3D window's screen rect
	RECT d3dr;
	_d3d_window.GetRect(d3dr);

	// mouse pointer from client to screen coordinates
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(GetHandle(), &pt);

	// was the pointer in the D3D window?
	if (pt.x >= d3dr.left  &&  pt.x <= d3dr.right
			&&  pt.y >= d3dr.top  &&  pt.y <= d3dr.bottom)
	{
		_last_x = x;
		_last_y = y;

		BeginDragging();
	}
}

void MagCalibDialog::OnAccelerator(int acceleratorID)
{
	if (acceleratorID == IDC_ACC_GENERATE_TEST_DATA)
		GenerateTestData();
	else if (acceleratorID == IDC_ACC_CLEAR_MAG_CALIBRATION)
		ResetMagCalibration();
}

void MagCalibDialog::BeginDragging()
{
	if (!_is_dragging)
	{
		_is_dragging = true;
		_mouse_trap.Capture(*this);
	}
}

void MagCalibDialog::EndDragging()
{
	if (_is_dragging)
	{
		// release the mouse
		_mouse_trap.Release();

		_is_dragging = false;
	}
}

void MagCalibDialog::OnLButtonUp(int x, int y, WPARAM wParam)
{
	EndDragging();
}

void MagCalibDialog::OnMouseMove(int x, int y, WPARAM wParam)
{
	if (_is_dragging)
	{
		_camera.SetRotation(float(x - _last_x), float(y - _last_y));

		_last_x = x;
		_last_y = y;
	}
}

void MagCalibDialog::OnMouseWheel(int x, int y, int delta, WPARAM wParam)
{
	_camera.Zoom(delta);
}

void MagCalibDialog::SaveData()
{
	// The open file dialog is modal, so we lose the main message loop,
	// which means we don't render any more, so the FPS calc is off.
	frame_times.clear();

	OpenSaveFileDialog saveFile;
	saveFile.AddFilter(L"CSV file (*.csv)", L"*.csv");
	saveFile.AddFilter(L"All files (*.*)", L"*.*");
	saveFile.SetDefaultFileName(L"magdata.csv");

	if (saveFile.GetSaveFile(L"Save magnetometer samples", *this))
	{
		std::wstring full_fname(saveFile.GetFullFileName());

		// no file extension?
		if (saveFile.GetFileName().find(L'.') == std::wstring::npos)
			full_fname += L".csv";

		SimpleFile f;
		std::wstring error_str;
		if (f.Open(full_fname, true, true))
		{
			char line[128];
			for (std::vector<mag_point_t>::iterator mi(_mag_raw_points.begin()); mi != _mag_raw_points.end(); ++mi)
			{
				sprintf_s(line, sizeof(line), "%i,%i,%i\n", mi->x, mi->y, mi->z);
				if (!f.Write(line, strlen(line)))
				{
					DWORD errcode = GetLastError();
					error_str = L"Unable to write to output file.\n\n" + GetErrorDescription(errcode);
					break;
				}
			}

		} else {
			DWORD errcode = GetLastError();
			error_str = L"Unable to open to output file for writing\n\nFile name: " + saveFile.GetFullFileName() + L"\n\n" + GetErrorDescription(errcode);
		}

		if (!error_str.empty())
			MsgBox(error_str.c_str(), L"Error", MB_OK | MB_ICONERROR);
	}
}

// splits a string using the provided delimiter
void split_record(const std::string& in_str, std::vector<std::string>& out_vector, const char delim)
{
	out_vector.clear();

	std::string::const_iterator riter(in_str.begin());
	std::string field;
	while (riter != in_str.end())
	{
		if (*riter == delim)
		{
			out_vector.push_back(field);
			field.clear();
		} else {
			field += *riter;
		}

		++riter;
	}

	out_vector.push_back(field);
}

void MagCalibDialog::AddPoint(const mag_point_t& p, bool is_raw)
{
	if (!_d3d_device.IsValid())
		return;

	const size_t ndx = is_raw ? 0 : 1;

	// enough space in the last vertex buffer?
	if (_vb_mag_points[ndx].back().Capacity() - _vb_mag_points[ndx].back().Count() < NUM_VERTICES_PER_CUBE)
	{
		// unlock the last buffer and make a new one
		if (_vb_mag_points[ndx].back().IsLocked())
			_vb_mag_points[ndx].back().Unlock();

		_vb_mag_points[ndx].push_back(VertexBuffer<Vertex3D>());
		_vb_mag_points[ndx].back().Alloc(_d3d_device, NUM_VERTICES_PER_VBUFFER);
	}

	if (!_vb_mag_points[ndx].back().IsLocked())
		_vb_mag_points[ndx].back().Lock();

	MagPoint mp;
	if (is_raw)
		mp.Build(p, _is_hmc5883l);
	else if (_is_ellipsoid_fit_valid)
		mp.BuildCalibrated(p, _ellipsoid_fit.center, _ellipsoid_fit.calibMatrix);
	else if (HasLoadedCalibration())
		mp.BuildCalibrated(p, _mag_offset, _mag_matrix);

	_vb_mag_points[ndx].back().AddObject(mp);
}

void MagCalibDialog::LoadData()
{
	// The open file dialog is modal, so we lose the main message loop,
	// which means we don't render any more, so the FPS calc is off.
	frame_times.clear();

	OpenSaveFileDialog openFile;
	openFile.AddFilter(L"CSV file (*.csv)", L"*.csv");
	openFile.AddFilter(L"All files (*.*)", L"*.*");

	if (openFile.GetOpenFile(L"Load raw magnetometer samples", *this))
	{
		WaitCursor wc;
		LoadRawDataFromFile(openFile.GetFullFileName());
	}
}

void MagCalibDialog::LoadRawDataFromFile(const std::wstring& fname)
{
	SimpleFile f;

	if (f.Open(fname, false))
	{
		// ellipsoid fit becomes invalid
		_is_ellipsoid_fit_valid = false;

		// read the entire file into a string (yeah, nasty, i know...)
		const int BUFF_SIZE = 1000;
		char buff[BUFF_SIZE];
		DWORD bytes_read;
		std::string file_str;
		do {
			bytes_read = f.Read(buff, BUFF_SIZE - 1);
			buff[bytes_read] = '\0';
			file_str += buff;
		} while (bytes_read == BUFF_SIZE - 1);

		ClearSamples();

		// parse and handle the lines, make points
		std::vector<std::string> lines, record;
		split_record(file_str, lines, '\n');

		for (std::vector<std::string>::iterator li(lines.begin()); li != lines.end(); ++li)
		{
			split_record(*li, record, ',');
			if (record.size() == 3)
			{
				mag_point_t mps(atoi(record[0].c_str()), atoi(record[1].c_str()), atoi(record[2].c_str()));

				if (std::find(_mag_raw_points.begin(), _mag_raw_points.end(), mps) == _mag_raw_points.end())
				{
					_mag_raw_points.push_back(mps);
					AddPoint(mps, true);
				}
			}
		}
	}
}

void MagCalibDialog::ClearCalibratedPoints()
{
	// and default empty vertex buffers
	_vb_mag_points[1].clear();
	_vb_mag_points[1].push_back(VertexBuffer<Vertex3D>());
	_vb_mag_points[1].back().Alloc(_d3d_device, NUM_VERTICES_PER_VBUFFER);

	_calib_start_from_point = _mag_raw_points.size();
}

void MagCalibDialog::SendCalibToDongle()
{
	// save the offsets and the matrix
	FeatRep_DongleSettings rep;

	// get the current settings
	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.GetFeatureReport(rep);

	// set the offsets
	rep.mag_offset[0] = round(_ellipsoid_fit.center.x);
	rep.mag_offset[1] = round(_ellipsoid_fit.center.y);
	rep.mag_offset[2] = round(_ellipsoid_fit.center.z);

	// scale the matrix for fixed point calc and then save
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			rep.mag_matrix[i][j] = round(_ellipsoid_fit.calibMatrix[i][j] * (1 << MAG_MATRIX_SCALE_BITS));

	// output the calibration 
	debug(L"\t{" + int2str(rep.mag_offset[0]) + L"," + int2str(rep.mag_offset[1]) + L"," + int2str(rep.mag_offset[2]) + L"},\t\t// mag_offset");
	debug(L"\t{\t// matrix");
	for (int i = 0; i < 3; ++i)
		debug(L"\t\t{" + int2str(rep.mag_matrix[i][0]) + L"," + int2str(rep.mag_matrix[i][1]) + L"," + int2str(rep.mag_matrix[i][2]) + L"},");
	debug(L"\t}");

	// send it back to the dongle
	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.SetFeatureReport(rep);

	_is_dongle_calib_read = false;

	ReadMagCalibration();

	// tell the main dialog to refresh the calibration info
	_wht_dlg.RefreshTrackerStatus();
}

void MagCalibDialog::GenerateTestData()
{
	ClearSamples();

	// make a random scale/rotate/translate matrix
	MyMatrix scale(MyMatrix::Scaling(	0.2f + float(rand()) / RAND_MAX * 2,
										0.2f + float(rand()) / RAND_MAX * 2,
										0.2f + float(rand()) / RAND_MAX * 2));
	
	MyMatrix rotate(MyMatrix::RotationYawPitchRoll(	float(M_PI * rand() / RAND_MAX * 2 - M_PI),
													float(M_PI * rand() / RAND_MAX * 2 - M_PI),
													float(M_PI * rand() / RAND_MAX * 2 - M_PI)));

	MyMatrix translate(MyMatrix::Translation(	float(rand() % 1000 - 500),
												float(rand() % 1000 - 500),
												float(rand() % 1000 - 500)));

	// the final matrix
	MyMatrix srt(scale * rotate * translate);

	for (int i = 0; i < 500; ++i)
	{
		// generate a random vector and normalize it
		// this creates random points on a sphere
		MyVector v(float(rand() % 400 - 200), float(rand() % 400 - 200), float(rand() % 400 - 200));

		float m = float(v.Length()) / 200;
		v = v / m;

		// now translate and scale the sphere creating an ellipsoid
		MyVector vout(v.Transform(srt));

		mag_point_t p(round(vout.x), round(vout.y), round(vout.z));

		// add the vector
		if (std::find(_mag_raw_points.begin(), _mag_raw_points.end(), p) == _mag_raw_points.end())
		{
			_mag_raw_points.push_back(p);

			// make the 3D represantations
			AddPoint(p, true);
		}
	}

	CalcEllipsoidFit();
}

void MagCalibDialog::ResetMagCalibration()
{
	// save the offsets and the matrix
	FeatRep_DongleSettings rep;

	// get the current settings
	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.GetFeatureReport(rep);

	// reset the offsets and the matrix
	for (int i = 0; i < 3; ++i)
	{
		rep.mag_offset[i] = 0;
		for (int j = 0; j < 3; ++j)
			rep.mag_matrix[i][j] = (i == j ? (1 << MAG_MATRIX_SCALE_BITS) : 0);
	}

	// send it back to the dongle
	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.SetFeatureReport(rep);

	_is_dongle_calib_read = false;

	ReadMagCalibration();

	// tell the main dialog to refresh the calibration info
	_wht_dlg.RefreshTrackerStatus();
}

void MagCalibDialog::ToggleShowDongleCalibPoints()
{
	//if (_chk_show_dongle_calib_points.IsChecked())
	//{
	//}
}

void MagCalibDialog::CalcEllipsoidFit()
{
	if (_mag_raw_points.size() < NUM_SAMPLES_FOR_ELLIPSOID_FIT)
	{
		MsgBox(L"Please record more points.", L"Error", MB_OK | MB_ICONERROR);
		return;
	}

	WaitCursor wc;

	// try to make an ellipsoid out of the points
	_ellipsoid_fit.fitEllipsoid(_mag_raw_points);
	_ellipsoid_fit.calcCalibMatrix(MagPoint::CALIBRATED_SCALE);

	// the fit is valid now
	_is_ellipsoid_fit_valid = true;

	// draw the ellipsoid axes
	_ellipsoid_axes.Build(_ellipsoid_fit.center, _ellipsoid_fit.radii, _ellipsoid_fit.evecs);

	_vb_ellipsoid_axes.Lock();
	_vb_ellipsoid_axes.Clear();
	_vb_ellipsoid_axes.AddObject(_ellipsoid_axes);
	_vb_ellipsoid_axes.Unlock();

	// clear the calibrated vertex buffer
	_vb_mag_points[1].clear();
	_vb_mag_points[1].push_back(VertexBuffer<Vertex3D>());
	_vb_mag_points[1].back().Alloc(_d3d_device, NUM_VERTICES_PER_VBUFFER);

	// re-draw the calibrated points
	for (std::vector<mag_point_t>::const_iterator piter(_mag_raw_points.begin()); piter != _mag_raw_points.end(); ++piter)
		AddPoint(*piter, false);
}
