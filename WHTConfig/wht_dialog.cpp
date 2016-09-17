#include "stdafx.h"

#include "resource.h"

#include "../dongle/reports.h"

#include "my_win.h"
#include "my_d3dx.h"
#include "d3d.h"
#include "d3d_objects.h"
#include "mag_calib_dialog.h"
#include "gyro_accel_dialog.h"
#include "custom_curves.h"
#include "raw_input.h"
#include "wht_dongle.h"
#include "wht_dialog.h"

// status bar "parts"
#define STATBAR_DONGLE_STATUS	0
#define STATBAR_RF_STATUS		1
#define STATBAR_VOLTAGE			2
#define STATBAR_SLEEP			3
#define STATBAR_TEMPERATURE		4
#define STATBAR_VERSION			5

#define STATBAR_NUM_PARTS		6

// disable warning 'this' : used in base member initializer list
#pragma warning ( disable : 4355 )

WHTDialog::WHTDialog()
:	_icon_large(IDI_ICON, true),
	_icon_small(IDI_ICON, false),
	_is_config_changed(false),
	_ignore_notifications(false),
	_gyro_accel_dlg(_dongle, *this),
	_mag_calig_dlg(_dongle, *this),
	_custom_curves(_dongle, *this),
	_in_exception(false),
	_is_tracker_found(false),
	_fnt_bold(L"Arial", 15, true),
	_batt_log_counter(0),
	_protocol_version_mismatch(false)
{}

void WHTDialog::OnInit()
{
	SetIcon(_icon_small);
	SetIcon(_icon_large);

	// setup our controls
	_cmb_axis_response.SetHandle(GetCtrl(IDC_CMB_AXIS_RESPONSE));
	_cmb_yaw_correct_method.SetHandle(GetCtrl(IDC_CMB_YAW_CORRECT_METHOD));
	_cmb_active_devices.SetHandle(GetCtrl(IDC_CMB_ACTIVE_DEVICES));

	_prg_axis_x.SetHandle(GetCtrl(IDC_PRG_AXIS_X));
	_prg_axis_y.SetHandle(GetCtrl(IDC_PRG_AXIS_Y));
	_prg_axis_z.SetHandle(GetCtrl(IDC_PRG_AXIS_Z));

	_lbl_axis_num_x.SetHandle(GetCtrl(IDC_LBL_AXIS_NUM_X));
	_lbl_axis_num_y.SetHandle(GetCtrl(IDC_LBL_AXIS_NUM_Y));
	_lbl_axis_num_z.SetHandle(GetCtrl(IDC_LBL_AXIS_NUM_Z));

	_lbl_axis_deg_x.SetHandle(GetCtrl(IDC_LBL_AXIS_DEG_X));
	_lbl_axis_deg_y.SetHandle(GetCtrl(IDC_LBL_AXIS_DEG_Y));
	_lbl_axis_deg_z.SetHandle(GetCtrl(IDC_LBL_AXIS_DEG_Z));

	_sld_mouse_sens_x.SetHandle(GetCtrl(IDC_SLD_MOUSE_SENS_X));
	_sld_mouse_sens_y.SetHandle(GetCtrl(IDC_SLD_MOUSE_SENS_Y));
	_sld_mouse_sens_wheel.SetHandle(GetCtrl(IDC_SLD_MOUSE_SENS_WHEEL));

	_sld_joystick_sens_x.SetHandle(GetCtrl(IDC_SLD_JOYSTICK_SENS_X));
	_sld_joystick_sens_y.SetHandle(GetCtrl(IDC_SLD_JOYSTICK_SENS_Y));
	_sld_joystick_sens_z.SetHandle(GetCtrl(IDC_SLD_JOYSTICK_SENS_Z));

	_sld_magnetometer_sens.SetHandle(GetCtrl(IDC_SLD_MAGNETOMETER_SENS));
	_cmb_sleep_mode.SetHandle(GetCtrl(IDC_CMB_SLEEP_MODE));

	_btn_custom_curves.SetHandle(GetCtrl(IDC_BTN_CUSTOM_CURVE));

	_btn_dongle_settings.SetHandle(GetCtrl(IDC_BTN_DONGLE_SETTINGS));

	_lbl_recenter_bound_status.SetHandle(GetCtrl(IDC_LBL_RECENTER_BOUND_STATUS));
	_lbl_recenter_bound_to_device.SetHandle(GetCtrl(IDC_LBL_RECENTER_BOUND_TO_DEVICE));
	_lbl_recenter_bound_to_details.SetHandle(GetCtrl(IDC_LBL_RECENTER_BOUND_TO_DETAILS));
	_btn_recenter_bind.SetHandle(GetCtrl(IDC_BTN_RECENTER_BIND));
	_btn_recenter_unbind.SetHandle(GetCtrl(IDC_BTN_RECENTER_UNBIND));

	_lbl_disable_tgl_bound_status.SetHandle(GetCtrl(IDC_LBL_DISABLE_TGL_BOUND_STATUS));
	_lbl_disable_tgl_bound_to_device.SetHandle(GetCtrl(IDC_LBL_DISABLE_TGL_BOUND_TO_DEVICE));
	_lbl_disable_tgl_bound_to_details.SetHandle(GetCtrl(IDC_LBL_DISABLE_TGL_BOUND_TO_DETAILS));
	_lbl_is_enabled.SetHandle(GetCtrl(IDC_LBL_IS_ENABLED));
	_btn_disable_tgl_bind.SetHandle(GetCtrl(IDC_BTN_DISABLE_TGL_BIND));
	_btn_disable_tgl_unbind.SetHandle(GetCtrl(IDC_BTN_DISABLE_TGL_UNBIND));

	_lbl_magnetometer_calibration_status.SetHandle(GetCtrl(IDC_LBL_MAGNETOMETER_CALIBRATION_STATUS));
	_lbl_gyro_accel_calibration_status.SetHandle(GetCtrl(IDC_LBL_GYRO_ACCEL_CALIBRATION_STATUS));

	_btn_gyro_accel_calibration.SetHandle(GetCtrl(IDC_BTN_GYRO_ACCEL_CALIBRATION));
	_btn_mag_calibration.SetHandle(GetCtrl(IDC_BTN_MAG_CALIBRATION));

	_chk_audio_alert.SetHandle(GetCtrl(IDC_CHK_AUDIO_ALERT));
	_edt_low_voltage.SetHandle(GetCtrl(IDC_EDT_LOW_VOLTAGE));
	_cmb_tracker_orientation.SetHandle(GetCtrl(IDC_CMB_TRACKER_ORIENTATION));

	_status_bar.SetHandle(GetCtrl(IDC_STATUS_BAR));

	// bold the values
	_lbl_axis_num_x.SetFont(_fnt_bold);
	_lbl_axis_num_y.SetFont(_fnt_bold);
	_lbl_axis_num_z.SetFont(_fnt_bold);

	_lbl_axis_deg_x.SetFont(_fnt_bold);
	_lbl_axis_deg_y.SetFont(_fnt_bold);
	_lbl_axis_deg_z.SetFont(_fnt_bold);

	_cmb_axis_response.SetFont(_fnt_bold);
	_cmb_yaw_correct_method.SetFont(_fnt_bold);
	_cmb_active_devices.SetFont(_fnt_bold);

	_lbl_magnetometer_calibration_status.SetFont(_fnt_bold);
	_lbl_gyro_accel_calibration_status.SetFont(_fnt_bold);

	_lbl_recenter_bound_status.SetFont(_fnt_bold);
	_lbl_recenter_bound_to_device.SetFont(_fnt_bold);
	_lbl_recenter_bound_to_details.SetFont(_fnt_bold);

	_lbl_disable_tgl_bound_status.SetFont(_fnt_bold);
	_lbl_disable_tgl_bound_to_device.SetFont(_fnt_bold);
	_lbl_disable_tgl_bound_to_details.SetFont(_fnt_bold);
	_lbl_is_enabled.SetFont(_fnt_bold);

	_edt_low_voltage.SetFont(_fnt_bold);
	_cmb_sleep_mode.SetFont(_fnt_bold);
	_cmb_tracker_orientation.SetFont(_fnt_bold);

	// setup the progress bar ranges
	_prg_axis_x.SetRange(0, 0xffff);
	_prg_axis_y.SetRange(0, 0xffff);
	_prg_axis_z.SetRange(0, 0xffff);

	// and the sliders
	std::vector<int> mouse_slider_values;
	int c = 0;
	std::generate_n(std::back_inserter(mouse_slider_values), NUM_MOUSE_SENS_VALUES, [&] () { return c++; });
	_sld_mouse_sens_x.SetValues(mouse_slider_values);
	_sld_mouse_sens_y.SetValues(mouse_slider_values);
	_sld_mouse_sens_wheel.SetValues(mouse_slider_values);

	// add numbers from 0 to 20
	std::vector<int> joystick_slider_values;
	c = 0;
	std::generate_n(std::back_inserter(joystick_slider_values), 21, [&] () { return c++; });
	_sld_joystick_sens_x.SetValues(joystick_slider_values);
	_sld_joystick_sens_y.SetValues(joystick_slider_values);
	_sld_joystick_sens_z.SetValues(joystick_slider_values);

	// the mouse sense
	std::vector<int> mag_sense_slider_values;
	c = 0;
	std::generate_n(std::back_inserter(mag_sense_slider_values), NUM_MAG_SENS_VALUES, [&] () { return c++; });
	_sld_magnetometer_sens.SetValues(mag_sense_slider_values);

#ifdef _DEBUG
	_btn_custom_curves.Show();
#endif

	// init the combo boxes
	_cmb_axis_response.AddString(L"Exponential");
	_cmb_axis_response.AddString(L"Linear");

	_cmb_yaw_correct_method.AddString(L"Auto");
	_cmb_yaw_correct_method.AddString(L"Jump to center");
	_cmb_yaw_correct_method.AddString(L"Magnetometer");
	_cmb_yaw_correct_method.AddString(L"None");

	_cmb_active_devices.AddString(L"Joystick only");
	_cmb_active_devices.AddString(L"Mouse only");
	_cmb_active_devices.AddString(L"Both");

	_cmb_sleep_mode.AddString(L"Sleep disabled");
	_cmb_sleep_mode.AddString(L"After 10 seconds");
	_cmb_sleep_mode.AddString(L"After 30 seconds");
	_cmb_sleep_mode.AddString(L"After 1 minute");
	_cmb_sleep_mode.AddString(L"After 2 minutes");
	_cmb_sleep_mode.AddString(L"After 5 minutes");
	_cmb_sleep_mode.AddString(L"After 10 minutes");
	_cmb_sleep_mode.AddString(L"After 30 minutes");
	_cmb_sleep_mode.AddString(L"After 60 minutes");

	_cmb_tracker_orientation.AddString(L"On/Off switch is on the right");
	_cmb_tracker_orientation.AddString(L"On/Off switch is on the back");

	// disable the controls since we're not connected to the dongle yet
	ChangeConnectedStateUI();

	// setup the status bar parts
	int parts[STATBAR_NUM_PARTS];

	parts[STATBAR_DONGLE_STATUS]	= 140;
	parts[STATBAR_RF_STATUS]		= 295;
	parts[STATBAR_VOLTAGE]			= 410;
	parts[STATBAR_SLEEP]			= 530;
	parts[STATBAR_TEMPERATURE]		= 692;
	parts[STATBAR_VERSION]			= -1;

	_status_bar.SetParts(parts, STATBAR_NUM_PARTS);

	_status_bar.SetPartText(STATBAR_VERSION, L"v1.3a  (" + get_date_str() + L")");

	// start the timer (10Hz)
	SetTimer(GetHandle(), 1, 100, NULL);

	// load the recenter key binding
	_raw_input.LoadBinding(*this);

	// load the values from the ini file
	double low_voltage = IniFile::GetIni().ReadDouble(L"BatteryMonitor", L"LowVoltage", -1);
	if (low_voltage != -1)
		_edt_low_voltage.SetText(dbl2str(low_voltage));

	int should_alert = IniFile::GetIni().ReadInt(L"BatteryMonitor", L"ShouldAlert", 0);
	_chk_audio_alert.SetState(should_alert != 0);
}

void WHTDialog::OnDestroy()
{
	// close the child dialogs too
	_mag_calig_dlg.Destroy();
	_gyro_accel_dlg.Destroy();

	PostQuitMessage(0);
}

void WHTDialog::OnControl(int ctrlID, int notifyID, HWND hWndCtrl)
{
	if (ctrlID == IDC_BTN_DONGLE_SETTINGS)
	{
		SendConfigToDevice();
		_is_config_changed = false;
		ChangeConnectedStateUI();

	} else if (ctrlID == IDC_SLD_MOUSE_SENS_X
			|| ctrlID == IDC_SLD_MOUSE_SENS_Y
			|| ctrlID == IDC_SLD_MOUSE_SENS_WHEEL
			|| ctrlID == IDC_SLD_JOYSTICK_SENS_X
			|| ctrlID == IDC_SLD_JOYSTICK_SENS_Y
			|| ctrlID == IDC_SLD_JOYSTICK_SENS_Z
			|| ctrlID == IDC_SLD_MAGNETOMETER_SENS) {

		// if the position of the sliders changed
		if (!_ignore_notifications)
		{
			_is_config_changed = true;
			ChangeConnectedStateUI();
		}

	} else if (ctrlID == IDC_CMB_AXIS_RESPONSE
			|| ctrlID == IDC_CMB_YAW_CORRECT_METHOD
			|| ctrlID == IDC_CMB_ACTIVE_DEVICES
			|| ctrlID == IDC_CMB_SLEEP_MODE
			|| ctrlID == IDC_CMB_TRACKER_ORIENTATION) {

		// if the selection of the combo box has changed
		if (notifyID == CBN_SELCHANGE  &&  	!_ignore_notifications)
		{
			_is_config_changed = true;
			ChangeConnectedStateUI();
		}

	} else if (ctrlID == IDC_BTN_MAG_CALIBRATION) {
		
		if (_mag_calig_dlg.IsValid())
			_mag_calig_dlg.BringToTop();
		else
			_mag_calig_dlg.CreateDlg(IDD_MAGNETOMETER_CALIB, 0);

	} else if (ctrlID == IDC_BTN_GYRO_ACCEL_CALIBRATION) {
		
		if (_gyro_accel_dlg.IsValid())
			_gyro_accel_dlg.BringToTop();
		else
			_gyro_accel_dlg.CreateDlg(IDD_GYRO_ACCEL_CONFIG, 0);

	} else if (ctrlID == IDC_BTN_RECENTER_BIND) {

		_raw_input.StartBinding(bf_recenter, *this);

		// move focus from the bind button in case the user wants to bind space or return
		SetFocus();

	} else if (ctrlID == IDC_BTN_RECENTER_UNBIND) {

		_raw_input.Unbind(bf_recenter);

	} else if (ctrlID == IDC_BTN_DISABLE_TGL_BIND) {

		_raw_input.StartBinding(bf_disable_toggle, *this);

		// move focus from the bind button in case the user wants to bind space or return
		SetFocus();

	} else if (ctrlID == IDC_BTN_DISABLE_TGL_UNBIND) {

		_raw_input.Unbind(bf_disable_toggle);

	} else if (ctrlID == IDC_BTN_CUSTOM_CURVE) {
		if (_custom_curves.IsValid())
			_custom_curves.BringToTop();
		else
			_custom_curves.CreateDlg(IDD_CUSTOM_CURVES, 0);
	} else if (ctrlID == IDC_EDT_LOW_VOLTAGE) {

		IniFile::GetIni().WriteDouble(L"BatteryMonitor", L"LowVoltage", _wtof(_edt_low_voltage.GetText().c_str()));

	} else if (ctrlID == IDC_CHK_AUDIO_ALERT) {
		bool is_checked = _chk_audio_alert.IsChecked();
		IniFile::GetIni().WriteInt(L"BatteryMonitor", L"ShouldAlert", is_checked ? 1 : 0);
	}
}

void WHTDialog::DoBattery(int voltage, int num_packets)
{
	// log battery
	//LogBattery(repStatus);

	// alert battery
	if (_chk_audio_alert.IsChecked()  &&  num_packets > 0)
	{
		uint16_t low_voltage = uint16_t(_wtof(_edt_low_voltage.GetText().c_str()) * 100);

		if (voltage <= low_voltage)
		{
			++_low_battery_cnt;

			if ((_low_battery_cnt % 100) == 0)
				::PlaySound(MAKEINTRESOURCE(IDR_WAV_BEEP), ::GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC);
		}
	} else {
		_low_battery_cnt = 0;
	}

	// default the low battery voltage
	if (_edt_low_voltage.GetText().empty())
	{
		if (voltage > 180)
			_edt_low_voltage.SetText(L"2.45");
		else
			_edt_low_voltage.SetText(L"1.03");
	}
}

void WHTDialog::OnTimer(int timerID)
{
	if (_in_exception)
		return;

	if (_dongle.IsOpen())
	{
		_status_bar.SetPartText(STATBAR_DONGLE_STATUS, L"Dongle: connected");

		// get the axis states
		hid_joystick_report_t rep;
		rep.report_id = JOYSTICK_REPORT_ID;
		_dongle.GetInputReport(rep);

		_prg_axis_x.SetPos(rep.x + 0x8000);
		_prg_axis_y.SetPos(rep.y + 0x8000);
		_prg_axis_z.SetPos(rep.z + 0x8000);

		_lbl_axis_num_x.SetText(rep.x);
		_lbl_axis_num_y.SetText(rep.y);
		_lbl_axis_num_z.SetText(rep.z);

		_lbl_axis_deg_x.SetText(dbl2str(rep.x / 65536.0 * 360, 2) + L"°");
		_lbl_axis_deg_y.SetText(dbl2str(rep.y / 65536.0 * 360, 2) + L"°");
		_lbl_axis_deg_z.SetText(dbl2str(rep.z / 65536.0 * 360, 2) + L"°");

		// get the current status
		FeatRep_Status repStatus;
		repStatus.report_id = STATUS_REPORT_ID;
		_dongle.GetFeatureReport(repStatus);

		std::wstring res;
		if (repStatus.num_packets >= 48)
			res = L"100";
		else
			res = int2str(repStatus.num_packets * 2);

		// status bar
		std::wstring rf_desc;
		if (repStatus.num_packets >= 48)
			rf_desc = L"Perfect";
		else if (repStatus.num_packets >= 40)
			rf_desc = L"Excelent";
		else if (repStatus.num_packets >= 30)
			rf_desc = L"Good";
		else if (repStatus.num_packets >= 20)
			rf_desc = L"Poor";
		else if (repStatus.num_packets >= 10)
			rf_desc = L"Bad";
		else if (repStatus.num_packets > 0)
			rf_desc = L"Awful";
		else if (repStatus.num_packets == 0)
			rf_desc = repStatus.shutdown_counter <= 2  &&  repStatus.shutdown_counter >= 0 ? L"Sleeping" : L"Disconnected";

		_status_bar.SetPartText(STATBAR_RF_STATUS, L"Tracker: " + rf_desc + (repStatus.num_packets > 0 ? L" " + res + L"%" : L""));

		const int BUFF_SIZE = 256;
		wchar_t buff[BUFF_SIZE];
		swprintf_s(buff, BUFF_SIZE, L"Battery: %2.2fV", repStatus.battery_voltage / 100.0);
		_status_bar.SetPartText(STATBAR_VOLTAGE, buff);

		if (repStatus.shutdown_counter != -1)
		{
			if (repStatus.shutdown_counter > 1)
			{
				swprintf_s(buff, BUFF_SIZE, L"Sleep in: %dsec", repStatus.shutdown_counter);
				_status_bar.SetPartText(STATBAR_SLEEP, buff);
			} else {
				_status_bar.SetPartText(STATBAR_SLEEP, L"Sleeping");
			}
		} else {
			_status_bar.SetPartText(STATBAR_SLEEP, L"Sleep disabled");
		}

		swprintf_s(buff, BUFF_SIZE, L"Temperature: %2.1f°C", repStatus.temperature / 10.0);
		_status_bar.SetPartText(STATBAR_TEMPERATURE, buff);

		_lbl_recenter_bound_status.SetText(_raw_input.BindingStatus(bf_recenter));
		_lbl_disable_tgl_bound_status.SetText(_raw_input.BindingStatus(bf_disable_toggle));
		_lbl_is_enabled.SetText(repStatus.is_enabled ? L"Enabled" : L"Disabled");

		if (_raw_input.IsBinding(bf_recenter))
		{
			_lbl_recenter_bound_to_device.SetText(L"Press a key/button on the keyboard or");
			_lbl_recenter_bound_to_details.SetText(L"joystick to bind the recenter to");
		} else {
			_lbl_recenter_bound_to_device.SetText(_raw_input.BoundDevice(bf_recenter));
			_lbl_recenter_bound_to_details.SetText(_raw_input.BoundButton(bf_recenter));
		}

		if (_raw_input.IsBinding(bf_disable_toggle))
		{
			_lbl_disable_tgl_bound_to_device.SetText(L"Press a key/button on the keyboard or");
			_lbl_disable_tgl_bound_to_details.SetText(L"joystick to bind the disable to");
		} else {
			_lbl_disable_tgl_bound_to_device.SetText(_raw_input.BoundDevice(bf_disable_toggle));
			_lbl_disable_tgl_bound_to_details.SetText(_raw_input.BoundButton(bf_disable_toggle));
		}

		_btn_recenter_unbind.Enable(_raw_input.IsBound(bf_recenter));
		_btn_disable_tgl_unbind.Enable(_raw_input.IsBound(bf_disable_toggle));

		_btn_gyro_accel_calibration.Enable(true);
		_btn_mag_calibration.Enable(true);

		// read the tracker calibration status again if the tracker re-appeared
		if (!_is_tracker_found  &&  repStatus.num_packets > 40)
			RefreshTrackerStatus();

		DoBattery(repStatus.battery_voltage, repStatus.num_packets);

	} else {

		bool was_procol_mismatch = _protocol_version_mismatch;

		if (!ConnectDongle())
		{
			_prg_axis_x.SetPos(0);
			_prg_axis_y.SetPos(0);
			_prg_axis_z.SetPos(0);

			_lbl_axis_num_x.ClearText();
			_lbl_axis_num_y.ClearText();
			_lbl_axis_num_z.ClearText();

			if (_protocol_version_mismatch)
				_status_bar.SetPartText(STATBAR_DONGLE_STATUS, L"Version mismatch!");
			else
				_status_bar.SetPartText(STATBAR_DONGLE_STATUS, L"Dongle: disconnected!");

			_status_bar.SetPartText(STATBAR_RF_STATUS, L"Tracker:");
			_status_bar.SetPartText(STATBAR_VOLTAGE, L"Battery:");
			_status_bar.SetPartText(STATBAR_SLEEP, L"Sleeping in:");
			_status_bar.SetPartText(STATBAR_TEMPERATURE, L"Temperature:");

			_lbl_magnetometer_calibration_status.ClearText();
			_lbl_gyro_accel_calibration_status.ClearText();

			_btn_gyro_accel_calibration.Enable(false);
#ifdef NDEBUG
			_btn_mag_calibration.Enable(false);
#endif

			_lbl_recenter_bound_status.ClearText();
			_lbl_recenter_bound_to_device.ClearText();
			_lbl_recenter_bound_to_details.ClearText();

			_lbl_disable_tgl_bound_status.ClearText();
			_lbl_disable_tgl_bound_to_device.ClearText();
			_lbl_disable_tgl_bound_to_details.ClearText();

			_lbl_is_enabled.ClearText();

			if (!was_procol_mismatch  &&  _protocol_version_mismatch)
			{
				MsgBox(L"The version of the config program you are running is different than the firmware version on the dongle or the tracker.\n"
								L"Please download and flash the latest version.",
						L"Version mismatch", MB_OK | MB_ICONERROR);
			}
		}
	}
}

void WHTDialog::LogBattery(const FeatRep_Status& repStatus)
{
	if ((_batt_log_counter % 100) == 0)
	{
		SimpleFile log;
		if (log.Open(GetExePath() + L"battlog.txt", true, false))
		{
			log.SeekToEnd();

			if (_batt_log_counter == 0)
			{
				const char* line = "-------------------------------\n";
				log.Write(line, strlen(line));
			}

			SYSTEMTIME systemTime;
			::GetLocalTime(&systemTime);

			char time_str[256];
			sprintf_s(time_str, sizeof(time_str), "%d/%02d/%02d %02d:%02d:%02d", 
													systemTime.wYear, systemTime.wMonth, systemTime.wDay,
													systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

			log.Write(time_str, strlen(time_str));

			if (repStatus.num_packets > 0)
			{
				char val_str[256];
				sprintf_s(val_str, sizeof(val_str),	"\t%gV\t%gC\n",
													repStatus.battery_voltage / 100.0,
													repStatus.temperature / 10.0);

				log.Write(val_str, strlen(val_str));
			} else {
				const char* line = "\tTracker not found\n";
				log.Write(line, strlen(line));
			}
		}
	}

	_batt_log_counter++;
}

void WHTDialog::OnException(const std::wstring& str)
{
	_in_exception = true;

	_dongle.Close();			// close the HID device
	_is_config_changed = false;	// we're haven't changed anything since we're closing
	ChangeConnectedStateUI();	// disable controls change the UI

	_in_exception = false;
}

void WHTDialog::OnRawInput(size_t input_code, HRAWINPUT hRawInput)
{
	bound_function_e bf = _raw_input.ProcessRawInput(hRawInput);

	if (!_dongle.IsOpen())
		bf = bf_none;

	if (bf == bf_recenter)
	{
		FeatRep_Command repReset;
		repReset.report_id = COMMAND_REPORT_ID;
		repReset.command = CMD_RECENTER;
		_dongle.SetFeatureReport(repReset);

	} else if (bf == bf_disable_toggle) {

		FeatRep_Command repReset;
		repReset.report_id = COMMAND_REPORT_ID;
		repReset.command = CMD_DISABLE_TOGGLE;
		_dongle.SetFeatureReport(repReset);
	}
}

#ifdef MINIMIZE_TO_TRAY

bool WHTDialog::OnSysCommand(WPARAM wParam)
{
	if (wParam == SC_MINIMIZE)
	{
		// minimize to tray
		CreateTrayIcon();
		Hide();

		return true;
	}

	return false;
}

void WHTDialog::CreateTrayIcon()
{
	NOTIFYICONDATA nid;
 
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = GetHandle();
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYNOTIFY;
	nid.hIcon = _hIconSmall;
	wcscpy_s(nid.szTip, L"Wireless Head Tracker");
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
 
	Shell_NotifyIcon(NIM_ADD, &nid);
}

void WHTDialog::OnTrayNotify(LPARAM lParam)
{
	if (lParam == WM_LBUTTONDOWN)
	{
		Show();
		RemoveTrayIcon();
	}
}

void WHTDialog::RemoveTrayIcon()
{
	NOTIFYICONDATA nid;
 
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = GetHandle();
	nid.uID = 100;
	nid.uVersion = NOTIFYICON_VERSION;
	nid.uCallbackMessage = WM_TRAYNOTIFY;
	nid.hIcon = _hIconSmall;
	wcscpy_s(nid.szTip, L"Wireless Head Tracker");
	nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
 
	Shell_NotifyIcon(NIM_DELETE, &nid);
}

#endif	// MINIMIZE_TO_TRAY


void WHTDialog::ReadConfigFromDevice()
{
	FeatRep_DongleSettings dngSettings;
	dngSettings.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.GetFeatureReport(dngSettings);

	_ignore_notifications = true;

	_cmb_axis_response.SetSelection(dngSettings.is_linear ? 1 : 0);
	_cmb_yaw_correct_method.SetSelection(dngSettings.yc_method);
	_cmb_active_devices.SetSelection(dngSettings.active_devices - 1);
	_cmb_tracker_orientation.SetSelection(dngSettings.tracker_orient);

	_sld_mouse_sens_x.SetValue(dngSettings.mouse_sens[0]);
	_sld_mouse_sens_y.SetValue(dngSettings.mouse_sens[1]);
	_sld_mouse_sens_wheel.SetValue(dngSettings.mouse_sens[2]);

	_sld_joystick_sens_x.SetValue(dngSettings.joy_fact[0]);
	_sld_joystick_sens_y.SetValue(dngSettings.joy_fact[1]);
	_sld_joystick_sens_z.SetValue(dngSettings.joy_fact[2]);

	_sld_magnetometer_sens.SetValue(dngSettings.mag_sens);

	_is_config_changed = false;

	_ignore_notifications = false;
}

void WHTDialog::RefreshTrackerStatus(FeatRep_DongleSettings* pRepSettings, FeatRep_TrackerSettings* pRepTracker)
{
	// read the setting if they haven't been passed in argument
	FeatRep_DongleSettings repObj;
	if (pRepSettings == 0)
	{
		repObj.report_id = DONGLE_SETTINGS_REPORT_ID;
		pRepSettings = &repObj;
		_dongle.GetFeatureReport(repObj);
	} else if (pRepSettings->report_id != DONGLE_SETTINGS_REPORT_ID) {
		pRepSettings->report_id = DONGLE_SETTINGS_REPORT_ID;
		_dongle.GetFeatureReport(*pRepSettings);
	}

	// if we still have the identity matrix - we're not mag calibrated
	if (pRepSettings->mag_matrix[0][0] == (1<<MAG_MATRIX_SCALE_BITS))
		_lbl_magnetometer_calibration_status.SetText(L"Not calibrated");
	else
		_lbl_magnetometer_calibration_status.SetText(L"Calibrated");

	// get the tracker settings too
	FeatRep_TrackerSettings repTracker;
	if (pRepTracker == 0)
	{
		repTracker.report_id = TRACKER_SETTINGS_REPORT_ID;
		pRepTracker = &repTracker;
		_dongle.GetFeatureReport(repTracker);
	} else if (pRepTracker->report_id != TRACKER_SETTINGS_REPORT_ID) {
		pRepTracker->report_id = TRACKER_SETTINGS_REPORT_ID;
		_dongle.GetFeatureReport(*pRepTracker);
	}

	_is_tracker_found = (pRepTracker->has_tracker_responded != 0);

	if (pRepTracker->has_tracker_responded == 0)
		_lbl_gyro_accel_calibration_status.SetText(L"Tracker not found");
	else if (pRepTracker->is_calibrated)
		_lbl_gyro_accel_calibration_status.SetText(L"Calibrated");
	else
		_lbl_gyro_accel_calibration_status.SetText(L"Not calibrated");

	_cmb_sleep_mode.SetSelection(pRepTracker->sleep_mode - CMD_DISABLE_SLEEP);
}

void WHTDialog::SendConfigToDevice()
{
	FeatRep_DongleSettings rep;

	// get the current settings
	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.GetFeatureReport(rep);

	// change the values we are interested in
	rep.is_linear = _cmb_axis_response.GetSelection();
	rep.yc_method = _cmb_yaw_correct_method.GetSelection();
	rep.active_devices = _cmb_active_devices.GetSelection() + 1;
	rep.tracker_orient = _cmb_tracker_orientation.GetSelection();

	rep.joy_fact[0] = _sld_joystick_sens_x.GetValue();
	rep.joy_fact[1] = _sld_joystick_sens_y.GetValue();
	rep.joy_fact[2] = _sld_joystick_sens_z.GetValue();

	rep.mouse_sens[0] = _sld_mouse_sens_x.GetValue();
	rep.mouse_sens[1] = _sld_mouse_sens_y.GetValue();
	rep.mouse_sens[2] = _sld_mouse_sens_wheel.GetValue();

	rep.mag_sens = _sld_magnetometer_sens.GetValue();

	// send it back to the dongle
	rep.report_id = DONGLE_SETTINGS_REPORT_ID;
	_dongle.SetFeatureReport(rep);

	// send the sleep mode
	FeatRep_Command cmd;
	cmd.report_id = COMMAND_REPORT_ID;
	cmd.command = _cmb_sleep_mode.GetSelection() + CMD_DISABLE_SLEEP;
	_dongle.SetFeatureReport(cmd);
}

bool WHTDialog::ConnectDongle()
{
	if (_dongle.Open())
	{
		protocol_version_e dongle_ver, tracker_ver;
		if (!_dongle.ReadProtocolVersion(dongle_ver, tracker_ver))
		{
			// try to get the input report to figure out if we are disconnected or the tracker
			// is too old and not replying to the version request report

			hid_joystick_report_t rep;
			rep.report_id = JOYSTICK_REPORT_ID;
			bool threw_ex = false;
			try {
				_dongle.GetInputReport(rep);
			} catch (...) {
				threw_ex = true;
			}

			_dongle.Close();

			if (!threw_ex)
				_protocol_version_mismatch = true;

			return false;

		} else if (dongle_ver != CURR_PROTOCOL_VER  ||  (tracker_ver != CURR_PROTOCOL_VER  &&  tracker_ver != pv_unknown)) {

			_dongle.Close();
			_protocol_version_mismatch = true;
			return false;
		}

		ReadConfigFromDevice();
		ChangeConnectedStateUI();

		return true;
	} else {
		_protocol_version_mismatch = false;
	}

	return false;
}

void WHTDialog::ChangeConnectedStateUI()
{
	bool is_connected = _dongle.IsOpen();

	_cmb_axis_response.Enable(is_connected);
	_cmb_yaw_correct_method.Enable(is_connected);
	_cmb_active_devices.Enable(is_connected);

	_sld_mouse_sens_x.Enable(is_connected);
	_sld_mouse_sens_y.Enable(is_connected);
	_sld_mouse_sens_wheel.Enable(is_connected);

	_sld_joystick_sens_x.Enable(is_connected);
	_sld_joystick_sens_y.Enable(is_connected);
	_sld_joystick_sens_z.Enable(is_connected);

	_cmb_tracker_orientation.Enable(is_connected);
	_cmb_sleep_mode.Enable(is_connected);

	_sld_magnetometer_sens.Enable(is_connected);

	_btn_dongle_settings.Enable(is_connected  &&  _is_config_changed);

	_btn_recenter_bind.Enable(is_connected);
	_btn_recenter_unbind.Enable(is_connected  &&  _raw_input.IsBound(bf_recenter));

	_btn_disable_tgl_bind.Enable(is_connected);
	_btn_disable_tgl_unbind.Enable(is_connected  &&  _raw_input.IsBound(bf_disable_toggle));
}
