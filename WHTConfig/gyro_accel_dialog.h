#pragma once

class WHTDialog;
class WHTDongle;

class GyroAccelDialog: public Dialog
{
private:
	Icon		_icon_large;
	Icon		_icon_small;

	Font		_fnt_bold;

    Label		_lbl_applied_drift_comp;
    Label		_lbl_new_drift_comp;
    Label		_lbl_yaw_value;
    Label		_lbl_sample_count;

    Label		_lbl_calib_status;
    Label		_lbl_gyro_bias_x;
    Label		_lbl_gyro_bias_y;
    Label		_lbl_gyro_bias_z;
    Label		_lbl_accel_bias_x;
    Label		_lbl_accel_bias_y;
    Label		_lbl_accel_bias_z;

	bool		_is_tracker_settings_read;
	bool		_is_tracker_found;
	int			_read_calibration_cnt;

	WHTDongle&	_dongle;
	WHTDialog&	_main_dlg;

	void SaveDriftCompensation();
	void RunGyroCalibration();
	void ResetDriftComp();
	void ReadTrackerSettings();

public:
	GyroAccelDialog(WHTDongle& dngl, WHTDialog& main_dlg);

	virtual void OnInit();
	virtual void OnTimer(int timerID);
	virtual void OnControl(int ctrlID, int notifyID, HWND hWndCtrl);
	virtual void OnException(const std::wstring& str);
};
