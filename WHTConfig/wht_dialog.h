#pragma once

#include "my_win.h"

// undefine to enable minimize to tray
//#define MINIMIZE_TO_TRAY

class WHTDialog: public Dialog
{
private:
	Icon		_icon_large;
	Icon		_icon_small;

	ProgressBar	_prg_axis_x;
	ProgressBar	_prg_axis_y;
	ProgressBar	_prg_axis_z;

	Label		_lbl_axis_num_x;
	Label		_lbl_axis_num_y;
	Label		_lbl_axis_num_z;

	Label		_lbl_axis_deg_x;
	Label		_lbl_axis_deg_y;
	Label		_lbl_axis_deg_z;

	ComboBox	_cmb_axis_response;
	ComboBox	_cmb_yaw_correct_method;
	ComboBox	_cmb_active_devices;

	IndexedSliderBar	_sld_mouse_sens_x;
	IndexedSliderBar	_sld_mouse_sens_y;
	IndexedSliderBar	_sld_mouse_sens_wheel;

	IndexedSliderBar	_sld_joystick_sens_x;
	IndexedSliderBar	_sld_joystick_sens_y;
	IndexedSliderBar	_sld_joystick_sens_z;

	IndexedSliderBar	_sld_magnetometer_sens;
	ComboBox			_cmb_tracker_orientation;
	ComboBox			_cmb_sleep_mode;

	Button		_btn_custom_curves;

	Button		_btn_dongle_settings;

	Label		_lbl_magnetometer_calibration_status;
	Label		_lbl_gyro_accel_calibration_status;

	Button		_btn_gyro_accel_calibration;
	Button		_btn_mag_calibration;

	Label		_lbl_recenter_bound_status;
	Label		_lbl_recenter_bound_to_device;
	Label		_lbl_recenter_bound_to_details;
	Button		_btn_recenter_bind;
	Button		_btn_recenter_unbind;

	Label		_lbl_disable_tgl_bound_status;
	Label		_lbl_disable_tgl_bound_to_device;
	Label		_lbl_disable_tgl_bound_to_details;
	Label		_lbl_is_enabled;
	Button		_btn_disable_tgl_bind;
	Button		_btn_disable_tgl_unbind;

	CheckBox	_chk_audio_alert;
	EditBox		_edt_low_voltage;

	StatusBar	_status_bar;

	Font		_fnt_bold;
	int			_batt_log_counter;

	WHTDongle	_dongle;

	bool		_is_config_changed;
	bool		_is_tracker_found;
	bool		_ignore_notifications;
	bool		_in_exception;
	bool		_protocol_version_mismatch;

	size_t		_low_battery_cnt;

	void ReadConfigFromDevice();
	void SendConfigToDevice();
	void DoBattery(int voltage, int num_packets);
	void LogBattery(const FeatRep_Status& repStatus);

#ifdef MINIMIZE_TO_TRAY
	void CreateTrayIcon();
	void RemoveTrayIcon();
	virtual void OnTrayNotify(LPARAM lParam);
	virtual bool OnSysCommand(WPARAM wParam);
#endif

	bool ConnectDongle();
	void ChangeConnectedStateUI();

	virtual void OnTimer(int timerID);
	virtual void OnControl(int ctrlID, int notifyID, HWND hWndCtrl);

	// the magnetometer calibration dialog
	MagCalibDialog		_mag_calig_dlg;

	// the gyro/accel calibration
	GyroAccelDialog		_gyro_accel_dlg;

	// the gyro/accel calibration
	CustomCurvesDialog	_custom_curves;

	// takes case of keyboard and joystick recenter binding
	RawInputHandler		_raw_input;

public:
	WHTDialog();
	virtual ~WHTDialog()		{}

	virtual void OnInit();
	virtual void OnDestroy();
	virtual void OnException(const std::wstring& str);
	virtual void OnRawInput(size_t input_code, HRAWINPUT hRawInput);

	void RefreshTrackerStatus(FeatRep_DongleSettings* pRepSettings = 0, FeatRep_TrackerSettings* pRepTracker = 0);

	MagCalibDialog& GetMagCompDialog()
	{
		return _mag_calig_dlg;
	}

	CustomCurvesDialog& GetCustomCurvesDialog()
	{
		return _custom_curves;
	}
};
