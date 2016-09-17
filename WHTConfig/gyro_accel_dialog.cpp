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

GyroAccelDialog::GyroAccelDialog(WHTDongle& dngl, WHTDialog& main_dlg)
:	_dongle(dngl),
	_main_dlg(main_dlg),
	_icon_large(IDI_GYRO, true),
	_icon_small(IDI_GYRO, false),
	_fnt_bold(L"Arial", 15, true)
{}

void GyroAccelDialog::OnInit()
{
	SetIcon(_icon_small);
	SetIcon(_icon_large);

	_read_calibration_cnt = 0;
	_is_tracker_settings_read = false;
	_is_tracker_found = false;

    _lbl_applied_drift_comp.SetHandle(GetCtrl(IDC_LBL_APPLIED_DRIFT_COMP));
    _lbl_new_drift_comp.SetHandle(GetCtrl(IDC_LBL_NEW_DRIFT_COMP));
    _lbl_yaw_value.SetHandle(GetCtrl(IDC_LBL_YAW_VALUE));
    _lbl_sample_count.SetHandle(GetCtrl(IDC_LBL_SAMPLE_COUNT));

    _lbl_calib_status.SetHandle(GetCtrl(IDC_LBL_CALIB_STATUS));
    _lbl_gyro_bias_x.SetHandle(GetCtrl(IDC_LBL_GYRO_BIAS_X));
    _lbl_gyro_bias_y.SetHandle(GetCtrl(IDC_LBL_GYRO_BIAS_Y));
    _lbl_gyro_bias_z.SetHandle(GetCtrl(IDC_LBL_GYRO_BIAS_Z));
    _lbl_accel_bias_x.SetHandle(GetCtrl(IDC_LBL_ACCEL_BIAS_X));
    _lbl_accel_bias_y.SetHandle(GetCtrl(IDC_LBL_ACCEL_BIAS_Y));
    _lbl_accel_bias_z.SetHandle(GetCtrl(IDC_LBL_ACCEL_BIAS_Z));

	_lbl_applied_drift_comp.SetFont(_fnt_bold);
    _lbl_new_drift_comp.SetFont(_fnt_bold);
    _lbl_yaw_value.SetFont(_fnt_bold);
    _lbl_sample_count.SetFont(_fnt_bold);

    _lbl_calib_status.SetFont(_fnt_bold);
    _lbl_gyro_bias_x.SetFont(_fnt_bold);
    _lbl_gyro_bias_y.SetFont(_fnt_bold);
    _lbl_gyro_bias_z.SetFont(_fnt_bold);
    _lbl_accel_bias_x.SetFont(_fnt_bold);
    _lbl_accel_bias_y.SetFont(_fnt_bold);
    _lbl_accel_bias_z.SetFont(_fnt_bold);

	// start the refresh timer
	SetTimer(GetHandle(), 1, 150, NULL);	// 100ms == 10 Hz

	// refresh now
	OnTimer(1);
}

void GyroAccelDialog::OnTimer(int timerID)
{
	if (_dongle.IsOpen())
	{
		// applied drift
		FeatRep_DongleSettings repSettings;
		repSettings.report_id = DONGLE_SETTINGS_REPORT_ID;
		_dongle.GetFeatureReport(repSettings);

		_lbl_applied_drift_comp.SetText(repSettings.drift_per_1k);

		// get the current status
		FeatRep_Status repStatus;
		repStatus.report_id = STATUS_REPORT_ID;
		_dongle.GetFeatureReport(repStatus);

		if (repStatus.sample_cnt)
			_lbl_new_drift_comp.SetText(repStatus.yaw_value * 1024 / repStatus.sample_cnt);
		else
			_lbl_new_drift_comp.SetText(0);

		_lbl_sample_count.SetText(repStatus.sample_cnt);
		_lbl_yaw_value.SetText(repStatus.yaw_value);

		// force a new tracker settings read if we now have a tracker connection
		if (repStatus.num_packets > 0  &&  !_is_tracker_found)
			_is_tracker_settings_read = false;

		// read the calibration if needed
		if (_read_calibration_cnt)
		{
			if (_read_calibration_cnt == 1)
			{
				ReadTrackerSettings();
				_read_calibration_cnt = 0;
			} else {
				--_read_calibration_cnt;
			}

		} else if (!_is_tracker_settings_read) {
			ReadTrackerSettings();
		}

	} else {

		_lbl_applied_drift_comp.ClearText();
		_lbl_new_drift_comp.ClearText();
		_lbl_yaw_value.ClearText();
		_lbl_sample_count.ClearText();

		_lbl_calib_status.ClearText();

		_lbl_gyro_bias_x.ClearText();
		_lbl_gyro_bias_y.ClearText();
		_lbl_gyro_bias_z.ClearText();

		_lbl_accel_bias_x.ClearText();
		_lbl_accel_bias_y.ClearText();
		_lbl_accel_bias_z.ClearText();

		_is_tracker_found = false;
		_is_tracker_settings_read = false;
		_read_calibration_cnt = 0;
	}
}

void GyroAccelDialog::OnException(const std::wstring& str)
{
}

void GyroAccelDialog::ReadTrackerSettings()
{
	FeatRep_TrackerSettings repTracker;
	repTracker.report_id = TRACKER_SETTINGS_REPORT_ID;
	_dongle.GetFeatureReport(repTracker);

	if (repTracker.has_tracker_responded == 0)
	{
		_lbl_calib_status.SetText(L"Tracker not found");

		_lbl_gyro_bias_x.ClearText();
		_lbl_gyro_bias_y.ClearText();
		_lbl_gyro_bias_z.ClearText();

		_lbl_accel_bias_x.ClearText();
		_lbl_accel_bias_y.ClearText();
		_lbl_accel_bias_z.ClearText();

	} else {
		_lbl_calib_status.SetText(repTracker.is_calibrated ? L"Calibrated" : L"Not calibrated");

		_lbl_gyro_bias_x.SetText(repTracker.gyro_bias[0]);
		_lbl_gyro_bias_y.SetText(repTracker.gyro_bias[1]);
		_lbl_gyro_bias_z.SetText(repTracker.gyro_bias[2]);

		_lbl_accel_bias_x.SetText(repTracker.accel_bias[0]);
		_lbl_accel_bias_y.SetText(repTracker.accel_bias[1]);
		_lbl_accel_bias_z.SetText(repTracker.accel_bias[2]);

		_is_tracker_found = true;
	}

	// refresh the tracker status on the main dialog
	_main_dlg.RefreshTrackerStatus(0, &repTracker);

	_is_tracker_settings_read = true;
}

void GyroAccelDialog::OnControl(int ctrlID, int notifyID, HWND hWndCtrl)
{
	if (ctrlID == IDC_BTN_SAVE_DRIFT_COMP)
		SaveDriftCompensation();
	else if (ctrlID == IDC_BTN_RESET_DRIFT_COMP)
		ResetDriftComp();
	else if (ctrlID == IDC_BTN_RUN_GYRO_CALIBRATE)
		RunGyroCalibration();
}

void GyroAccelDialog::RunGyroCalibration()
{
	FeatRep_Command rep;
	rep.report_id = COMMAND_REPORT_ID;
	rep.command = CMD_CALIBRATE;
	_dongle.SetFeatureReport(rep);

	// clear the calibration fields
	_lbl_calib_status.SetText(L"Calibrating...");
	_lbl_gyro_bias_x.ClearText();
	_lbl_gyro_bias_y.ClearText();
	_lbl_gyro_bias_z.ClearText();
	_lbl_accel_bias_x.ClearText();
	_lbl_accel_bias_y.ClearText();
	_lbl_accel_bias_z.ClearText();

	_read_calibration_cnt = 8;
}

void GyroAccelDialog::SaveDriftCompensation()
{
	// send a save drift command
	FeatRep_Command rep;
	rep.report_id = COMMAND_REPORT_ID;
	rep.command = CMD_SAVE_DRIFT;
	_dongle.SetFeatureReport(rep);

	// read the new drift compensation value back
	FeatRep_DongleSettings repSettings;
	repSettings.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.GetFeatureReport(repSettings);

	_lbl_applied_drift_comp.SetText(repSettings.drift_per_1k);

	// wait a little
	Sleep(200);

	// reset the drift calculation
	ResetDriftComp();

	// refresh the calibration status on the main dialog
	_main_dlg.RefreshTrackerStatus();
}

void GyroAccelDialog::ResetDriftComp()
{
	FeatRep_Command rep;
	rep.report_id = COMMAND_REPORT_ID;
	rep.command = CMD_RECENTER;
	_dongle.SetFeatureReport(rep);
}
