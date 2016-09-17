#include "stdafx.h"

#include "my_utils.h"
#include "my_win.h"

#include "raw_input.h"

#include "hid.h"

#define VK_CONST_DESC(a)		case VK_##a:	return L ## #a

struct hhb_vkey
{
	uint8_t		vkey;
	wchar_t*	name;
};

const hhb_vkey hhb_vkey_list[] = {
{ VK_LBUTTON,		L"Left mouse button"},
{ VK_RBUTTON,		L"Right mouse button" },
{ VK_CANCEL,		L"Control-break processing" },
{ VK_MBUTTON,		L"Middle mouse button (three-button mouse)" },
{ VK_XBUTTON1,		L"X1 mouse button" },
{ VK_XBUTTON2,		L"X2 mouse button" },
{ 0x07,				L"Undefined" },
{ VK_BACK,			L"BACKSPACE key" },
{ VK_TAB,			L"TAB key" },
{ VK_CLEAR,			L"CLEAR key" },
{ VK_RETURN,		L"ENTER key" },
{ VK_SHIFT,			L"SHIFT key" },
{ VK_CONTROL,		L"CTRL key" },
{ VK_MENU,			L"ALT key" },
{ VK_PAUSE,			L"PAUSE key" },
{ VK_CAPITAL,		L"CAPS LOCK key" },
{ VK_KANA,			L"IME Kana mode" },
{ VK_JUNJA,			L"IME Junja mode" },
{ VK_FINAL,			L"IME final mode" },
{ VK_HANJA,			L"IME Hanja mode" },
{ VK_KANJI,			L"IME Kanji mode" },
{ VK_ESCAPE,		L"ESC key" },
{ VK_CONVERT,		L"IME convert" },
{ VK_NONCONVERT,	L"IME nonconvert" },
{ VK_ACCEPT,		L"IME accept" },
{ VK_MODECHANGE,	L"IME mode change request" },
{ VK_SPACE,			L"SPACEBAR" },
{ VK_PRIOR,			L"PAGE UP key" },
{ VK_NEXT,			L"PAGE DOWN key" },
{ VK_END,			L"END key" },
{ VK_HOME,			L"HOME key" },
{ VK_LEFT,			L"LEFT ARROW key" },
{ VK_UP,			L"UP ARROW key" },
{ VK_RIGHT,			L"RIGHT ARROW key" },
{ VK_DOWN,			L"DOWN ARROW key" },
{ VK_SELECT,		L"SELECT key" },
{ VK_PRINT,			L"PRINT key" },
{ VK_EXECUTE,		L"EXECUTE key" },
{ VK_SNAPSHOT,		L"PRINT SCREEN key" },
{ VK_INSERT,		L"INS key" },
{ VK_DELETE,		L"DEL key" },
{ VK_HELP,			L"HELP key" },
{ 0x30,				L"0 key" },
{ 0x31,				L"1 key" },
{ 0x32,				L"2 key" },
{ 0x33,				L"3 key" },
{ 0x34,				L"4 key" },
{ 0x35,				L"5 key" },
{ 0x36,				L"6 key" },
{ 0x37,				L"7 key" },
{ 0x38,				L"8 key" },
{ 0x39,				L"9 key" },
{ 0x41,				L"A key" },
{ 0x42,				L"B key" },
{ 0x43,				L"C key" },
{ 0x44,				L"D key" },
{ 0x45,				L"E key" },
{ 0x46,				L"F key" },
{ 0x47,				L"G key" },
{ 0x48,				L"H key" },
{ 0x49,				L"I key" },
{ 0x4A,				L"J key" },
{ 0x4B,				L"K key" },
{ 0x4C,				L"L key" },
{ 0x4D,				L"M key" },
{ 0x4E,				L"N key" },
{ 0x4F,				L"O key" },
{ 0x50,				L"P key" },
{ 0x51,				L"Q key" },
{ 0x52,				L"R key" },
{ 0x53,				L"S key" },
{ 0x54,				L"T key" },
{ 0x55,				L"U key" },
{ 0x56,				L"V key" },
{ 0x57,				L"W key" },
{ 0x58,				L"X key" },
{ 0x59,				L"Y key" },
{ 0x5A,				L"Z key" },
{ VK_LWIN,			L"Left Windows key (Natural keyboard)" },
{ VK_RWIN,			L"Right Windows key (Natural keyboard)" },
{ VK_APPS,			L"Applications key (Natural keyboard)" },
{ VK_SLEEP,			L"Computer Sleep key" },
{ VK_NUMPAD0,		L"Numeric keypad 0 key" },
{ VK_NUMPAD1,		L"Numeric keypad 1 key" },
{ VK_NUMPAD2,		L"Numeric keypad 2 key" },
{ VK_NUMPAD3,		L"Numeric keypad 3 key" },
{ VK_NUMPAD4,		L"Numeric keypad 4 key" },
{ VK_NUMPAD5,		L"Numeric keypad 5 key" },
{ VK_NUMPAD6,		L"Numeric keypad 6 key" },
{ VK_NUMPAD7,		L"Numeric keypad 7 key" },
{ VK_NUMPAD8,		L"Numeric keypad 8 key" },
{ VK_NUMPAD9,		L"Numeric keypad 9 key" },
{ VK_MULTIPLY,		L"Multiply key" },
{ VK_ADD,			L"Add key" },
{ VK_SEPARATOR,		L"Separator key" },
{ VK_SUBTRACT,		L"Subtract key" },
{ VK_DECIMAL,		L"Decimal key" },
{ VK_DIVIDE,		L"Divide key" },
{ VK_F1,			L"F1 key" },
{ VK_F2,			L"F2 key" },
{ VK_F3,			L"F3 key" },
{ VK_F4,			L"F4 key" },
{ VK_F5,			L"F5 key" },
{ VK_F6,			L"F6 key" },
{ VK_F7,			L"F7 key" },
{ VK_F8,			L"F8 key" },
{ VK_F9,			L"F9 key" },
{ VK_F10,			L"F10 key" },
{ VK_F11,			L"F11 key" },
{ VK_F12,			L"F12 key" },
{ VK_F13,			L"F13 key" },
{ VK_F14,			L"F14 key" },
{ VK_F15,			L"F15 key" },
{ VK_F16,			L"F16 key" },
{ VK_F17,			L"F17 key" },
{ VK_F18,			L"F18 key" },
{ VK_F19,			L"F19 key" },
{ VK_F20,			L"F20 key" },
{ VK_F21,			L"F21 key" },
{ VK_F22,			L"F22 key" },
{ VK_F23,			L"F23 key" },
{ VK_F24,			L"F24 key" },
{ VK_NUMLOCK,		L"NUM LOCK key" },
{ VK_SCROLL,		L"SCROLL LOCK key" },
{ VK_NUMLOCK,		L"NUM LOCK key" },
{ VK_SCROLL,		L"SCROLL LOCK key" },
{ VK_LSHIFT,		L"Left SHIFT key" },
{ VK_RSHIFT,		L"Right SHIFT key" },
{ VK_LCONTROL,		L"Left CONTROL key" },
{ VK_RCONTROL,		L"Right CONTROL key" },
{ VK_LMENU,			L"Left MENU key" },
{ VK_RMENU,			L"Right MENU key" },
{ VK_BROWSER_BACK,	L"Browser Back key" },
{ VK_BROWSER_FORWARD, L"Browser Forward key" },
{ VK_BROWSER_REFRESH, L"Browser Refresh key" },
{ VK_BROWSER_STOP,	L"Browser Stop key" },
{ VK_BROWSER_SEARCH, L"Browser Search key" },
{ VK_BROWSER_FAVORITES, L"Browser Favorites key" },
{ VK_BROWSER_HOME,	L"Browser Start and Home key" },
{ VK_VOLUME_MUTE,	L"Volume Mute key" },
{ VK_VOLUME_DOWN,	L"Volume Down key" },
{ VK_VOLUME_UP,		L"Volume Up key" },
{ VK_MEDIA_NEXT_TRACK, L"Next Track key" },
{ VK_MEDIA_PREV_TRACK, L"Previous Track key" },
{ VK_MEDIA_STOP,	L"Stop Media key" },
{ VK_MEDIA_PLAY_PAUSE, L"Play/Pause Media key" },
{ VK_LAUNCH_MAIL,	L"Start Mail key" },
{ VK_LAUNCH_MEDIA_SELECT, L"Select Media key" },
{ VK_LAUNCH_APP1,	L"Start Application 1 key" },
{ VK_LAUNCH_APP2,	L"Start Application 2 key" },
{ VK_OEM_1,			L"';:' key" },
{ VK_OEM_PLUS,		L"+ key"},
{ VK_OEM_COMMA,		L", key" },
{ VK_OEM_MINUS,		L"- key" },
{ VK_OEM_PERIOD,	L". key" },
{ VK_OEM_2,			L"/? key" },
{ VK_OEM_3,			L"`~ key" },
{ VK_OEM_4,			L"[{ key" },
{ VK_OEM_5,			L"\\| key" },
{ VK_OEM_6,			L"]} key" },
{ VK_OEM_7,			L"'\" key" },
{ VK_OEM_8,			L"VK_OEM_8 key" },
{ VK_OEM_102,		L"VK_OEM_102 key" },
{ VK_PROCESSKEY,	L"IME PROCESS key" },
{ VK_PACKET,		L"VK_PACKET key" },
{ VK_ATTN,			L"Attn key" },
{ VK_CRSEL,			L"CrSel key" },
{ VK_EXSEL,			L"ExSel key" },
{ VK_EREOF,			L"Erase EOF key" },
{ VK_PLAY,			L"Play key" },
{ VK_ZOOM,			L"Zoom key" },
{ VK_NONAME,		L"VK_NONAME key" },
{ VK_PA1,			L"PA1 key" },
{ VK_OEM_CLEAR,		L"Clear key" },
{ 0, 0 }
};

std::wstring GetVKeyDesc(USHORT vkey)
{
	int i = 0;
	while (hhb_vkey_list[i].vkey != 0)
	{
		if (vkey == hhb_vkey_list[i].vkey)
			return hhb_vkey_list[i].name;

		++i;
	}

	return int2hex(vkey, 4) + L" key";
}

RawInputHandler::RawInputHandler()
	: _binding_table(2, bind_element_t())
{}

bool RawInputHandler::GetActiveJoystickButtons(PRAWINPUT pRawInput, std::vector<int>& activeButtons)
{
	activeButtons.clear();

	// get the preparsed data block
	UINT bufferSize;
	if (GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, NULL, &bufferSize) != 0)
	{
		logfile.Line(L"GetRawInputDeviceInfo(1) FAILED " + GetErrorDescription(GetLastError()));
		return false;
	}

	std::vector<char> buffPreparsedData(bufferSize, '\0');
	PHIDP_PREPARSED_DATA pPreparsedData = (PHIDP_PREPARSED_DATA) &buffPreparsedData.front();

	// GetRawInputDeviceInfo returns UINT, but that value can be negative! Windows API - we love ya! ;)
	UINT res = GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_PREPARSEDDATA, pPreparsedData, &bufferSize);
	if (int(res) < 0)
	{
		logfile.Line(L"GetRawInputDeviceInfo(2) FAILED " + GetErrorDescription(GetLastError()));
		return false;
	}

	// get the joystick's capabilities
	HIDP_CAPS caps;
	if (HidP_GetCaps(pPreparsedData, &caps) != HIDP_STATUS_SUCCESS)
	{
		logfile.Line(L"HidP_GetCaps() FAILED " + GetErrorDescription(GetLastError()));
		return false;
	}

	// do we have buttons on this HID?
	USHORT capsLength;
	if (caps.NumberInputButtonCaps)
	{
		// make a buffer to receive the data
		std::vector<HIDP_BUTTON_CAPS> buttonCaps(caps.NumberInputButtonCaps);

		// get all the button caps for this HID device
		capsLength = caps.NumberInputButtonCaps;
		if (HidP_GetButtonCaps(HidP_Input, &buttonCaps.front(), &capsLength, pPreparsedData) != HIDP_STATUS_SUCCESS)
		{
			logfile.Line(L"HidP_GetButtonCaps() FAILED " + GetErrorDescription(GetLastError()));
			return false;
		}

		const USAGE ButtonUsagePage = 9;

		// check if this joystick has a button usage page
		bool has_button_usage_page = false;
		for (std::vector<HIDP_BUTTON_CAPS>::iterator bci(buttonCaps.begin()); bci != buttonCaps.end(); bci++)
			has_button_usage_page |= bci->UsagePage == ButtonUsagePage;

		if (has_button_usage_page)
		{
			// get the pressed buttons
			const ULONG MAX_BUTTONS = 128;
			ULONG usageLength = MAX_BUTTONS;
			USAGE usages[MAX_BUTTONS];
			if (HidP_GetUsages(HidP_Input, ButtonUsagePage, 0, usages, &usageLength, pPreparsedData,
								(PCHAR)pRawInput->data.hid.bRawData, pRawInput->data.hid.dwSizeHid) != HIDP_STATUS_SUCCESS)
			{
				logfile.Line(L"HidP_GetUsages() FAILED " + GetErrorDescription(GetLastError()));
				return false;
			}

			// add the active buttons
			ULONG i;
			for (i = 0; i < usageLength; i++)
				activeButtons.push_back(usages[i]);
		}
	}

	return true;
}

bool RawInputHandler::GetRawInputIDs(RAWINPUT* pRawInput, USHORT& vendor_id, USHORT& product_id, std::wstring& product_name)
{
	// get the VID/PID of the HID device
	wchar_t devName[MAX_PATH];
	UINT buffSize = MAX_PATH;
	UINT res = GetRawInputDeviceInfo(pRawInput->header.hDevice, RIDI_DEVICENAME, devName, &buffSize);
	if (int(res) <= 0)
	{
		logfile.Line(L"GetRawInputDeviceInfo() FAILED " + GetErrorDescription(GetLastError()));
		return false;
	}

	std::wstring long_dev_name(devName);

	auto di = _device_details_cache.find(long_dev_name);
	if (di != _device_details_cache.end())
	{
		vendor_id = di->second.vendor_id;
		product_id = di->second.product_id;
		product_name = di->second.product_name;

		return true;
	}

	// open the HID device
	HANDLE hHidDev = CreateFile(devName, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (hHidDev != INVALID_HANDLE_VALUE)
	{
		// query the attribs
		HIDD_ATTRIBUTES attrib;
		attrib.Size = sizeof(HIDD_ATTRIBUTES);
		if (HidD_GetAttributes(hHidDev, &attrib))
		{
			vendor_id = attrib.VendorID;
			product_id = attrib.ProductID;
		} else {
			CloseHandle(hHidDev);
			logfile.Line(L"HidD_GetAttributes() FAILED " + GetErrorDescription(GetLastError()));
			return false;
		}

		// sometimes, for some HIDs this single function call can take 30msec. wow!
		// which is why we cache this data
		if (HidD_GetProductString(hHidDev, devName, MAX_PATH))
			product_name = devName;
		else
			product_name.clear();

		// cache the data for next time
		DeviceDetails dd;
		dd.vendor_id = vendor_id;
		dd.product_id = product_id;
		dd.product_name = product_name;

		_device_details_cache[long_dev_name] = dd;

		CloseHandle(hHidDev);

		return true;
	} else {
		logfile.Line(L"hHidDev == INVALID_HANDLE_VALUE");
	}

	return false;
}

bound_function_e RawInputHandler::ProcessRawInput(HRAWINPUT hRawInput)
{
	// get the buffer size
	UINT dwSize = 0;
	if (GetRawInputData(hRawInput, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER)))
	{
		logfile.Line(L"GetRawInputData(1) FAILED " + GetErrorDescription(GetLastError()));
		return bf_none;
	}

	std::vector<BYTE> buff(dwSize, BYTE(0));
	LPBYTE lpb = &buff.front();
	if (GetRawInputData(hRawInput, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
	{
		logfile.Line(L"GetRawInputData(2) FAILED " + GetErrorDescription(GetLastError()));
		return bf_none;
	}

	RAWINPUT* pRawInput = (RAWINPUT*) lpb;

	for (std::vector<bind_element_t>::iterator bti(_binding_table.begin());
			bti != _binding_table.end(); bti++)
	{
		if (pRawInput->header.dwType == RIM_TYPEKEYBOARD)
		{
			RAWKEYBOARD& rk(pRawInput->data.keyboard);

			// if the key went down
			if ((rk.Flags & RI_KEY_BREAK) == 0)
			{
				if (bti->bound_state == bs_binding)
				{
					bti->vkey = rk.VKey;
					bti->bound_state = bs_keyboard;

					// we don't need the joystick input any more
					//UnregisterJoystick();

					return bf_none;
				} else if (bti->bound_state == bs_keyboard) {
					if (bti->vkey == rk.VKey)
						return bound_function_e(bti - _binding_table.begin());
				}
			}

		} else if (pRawInput->header.dwType == RIM_TYPEHID) {

			if (bti->bound_state == bs_binding  ||  bti->bound_state == bs_joystick)
			{
				std::vector<int> activeButtons;
				GetActiveJoystickButtons(pRawInput, activeButtons);

				if (!activeButtons.empty())
				{
					std::wstring joystick_name;
					USHORT vendorID, productID;
					GetRawInputIDs(pRawInput, vendorID, productID, joystick_name);

					std::pair<int, int> ids(vendorID, productID);

					if (bti->bound_state == bs_binding)
					{
						// remember the button(s) which are down when we start binding
						// because we want to ignore those buttons
						if (_ignore_buttons.find(ids) == _ignore_buttons.end())
						{
							if (bti->binding_started.GetMicroSec() < 1000*50)		// 50msec
								_ignore_buttons[ids] = activeButtons;
							else
								_ignore_buttons[ids];
						}

						// remove the ignored buttons
						std::vector<int>::iterator abi;
						abi = std::remove_if(activeButtons.begin(), activeButtons.end(), 
											[this, ids] (int btn) { return std::find(_ignore_buttons[ids].begin(), _ignore_buttons[ids].end(), btn) != _ignore_buttons[ids].end(); });

						if (abi != activeButtons.end())
							activeButtons.erase(abi, activeButtons.end());

						if (!activeButtons.empty())
						{
							// remember the button
							bti->button_num = activeButtons.front();

							// we don't need the keyboard input any more
							//UnregisterKeyboard();

							// remember the IDs and the name
							bti->vendorID = vendorID;
							bti->productID = productID;
							bti->joystick_name = joystick_name;

							// we don't need the ignore buttons any more
							_ignore_buttons.clear();

							bti->bound_state = bs_joystick;
						}

					} else if (bti->bound_state == bs_joystick) {

						if (bti->vendorID == vendorID  &&  bti->productID == productID
								&&  std::find(activeButtons.begin(), activeButtons.end(), bti->button_num) != activeButtons.end())
								return bound_function_e(bti - _binding_table.begin());
					}
				}
			}
		}
	}

	return bf_none;
}

void RawInputHandler::StartBinding(const bound_function_e fn, Window& w)
{
	Unbind(fn);

	_ignore_buttons.clear();

	_binding_table[fn].bound_state = bs_binding;
	_binding_table[fn].binding_started.Start();

	RegisterRawInput(true, true, w);
}

void RawInputHandler::Unbind(const bound_function_e fn)
{
	//UnregisterKeyboard();
	//UnregisterJoystick();

	_binding_table[fn].bound_state = bs_inactive;
}

std::wstring RawInputHandler::BindingStatus(const bound_function_e fn) const
{
	if (_binding_table[fn].bound_state == bs_inactive)
		return L"Inactive";
	else if (_binding_table[fn].bound_state == bs_binding)
		return L"Binding...";

	return L"Bound";
}

std::wstring RawInputHandler::BoundDevice(const bound_function_e fn) const
{
	if (_binding_table[fn].bound_state == bs_binding  ||  _binding_table[fn].bound_state == bs_inactive)
		return L"";
	else if (_binding_table[fn].bound_state == bs_keyboard)
		return L"Keyboard";

	return _binding_table[fn].joystick_name;
}

std::wstring RawInputHandler::BoundButton(const bound_function_e fn) const
{
	if (_binding_table[fn].bound_state == bs_binding  ||  _binding_table[fn].bound_state == bs_inactive)
		return L"";
	else if (_binding_table[fn].bound_state == bs_keyboard)
		return GetVKeyDesc(_binding_table[fn].vkey);

	return L"Button " + int2str(_binding_table[fn].button_num);
}

bool RawInputHandler::RegisterRawInput(bool reg_keyboard, bool reg_joystick, Window& win)
{
	UnregisterRawInput();

	if (!reg_keyboard  &&  !reg_joystick)
		return true;

	std::vector<RAWINPUTDEVICE> input_devices;

	// register raw input
	if (reg_keyboard)
	{
		RAWINPUTDEVICE rid;
		
		rid.usUsagePage = 0x01;		// generic desktop controls
		rid.usUsage = 0x06;			// keyboard
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = win.GetHandle();

		input_devices.push_back(rid);
	}

	if (reg_joystick)
	{
		RAWINPUTDEVICE rid;
		
		rid.usUsagePage = 0x01;		// generic desktop controls
		rid.usUsage = 0x04;			// joystick
		rid.dwFlags = RIDEV_INPUTSINK;
		rid.hwndTarget = win.GetHandle();

		input_devices.push_back(rid);
	}

	/*
	rid.usUsagePage = 0x01;		// generic desktop controls
	rid.usUsage = 0x02;			// mouse
	rid.dwFlags = RIDEV_INPUTSINK;
	rid.hwndTarget = win.GetHandle();

	rid.usUsagePage = 0x01;		// generic desktop controls
	rid.usUsage = 0x05;			// game pad
	rid.dwFlags = RIDEV_INPUTSINK;
	rid.hwndTarget = win.GetHandle();
	*/

	bool retval = RegisterRawInputDevices(&input_devices.front(), input_devices.size(), sizeof(RAWINPUTDEVICE)) == TRUE;
	if (!retval)
		logfile.Line(L"RegisterRawInputDevices() FAILED " + GetErrorDescription(GetLastError()));

	return retval;
}

void RawInputHandler::UnregisterRawInput()
{
	UnregisterKeyboard();
	UnregisterJoystick();
}

void RawInputHandler::UnregisterKeyboard()
{
	RAWINPUTDEVICE rid;
		
	rid.usUsagePage = 0x01;		// generic desktop controls
	rid.usUsage = 0x06;			// keyboard
	rid.dwFlags = RIDEV_REMOVE;
	rid.hwndTarget = 0;

	BOOL res = RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

void RawInputHandler::UnregisterJoystick()
{
	RAWINPUTDEVICE rid;
		
	rid.usUsagePage = 0x01;		// generic desktop controls
	rid.usUsage = 0x04;			// joystick
	rid.dwFlags = RIDEV_REMOVE;
	rid.hwndTarget = 0;

	BOOL res = RegisterRawInputDevices(&rid, 1, sizeof(rid));
}

void RawInputHandler::LoadBinding(Window& win)
{
	bool register_joystick = false;
	bool register_keyboard = false;

	for (std::vector<bind_element_t>::iterator bti(_binding_table.begin());
			bti != _binding_table.end(); bti++)
	{
		const wchar_t* section(bti == _binding_table.begin() ? L"Recenter" : L"DisableToggle");

		bti->bound_state = (bound_state_e) IniFile::GetIni().ReadInt(section, L"State", bs_inactive);

		if (bti->bound_state == bs_keyboard)
		{
			register_keyboard = true;

			bti->vkey = IniFile::GetIni().ReadInt(section, L"KbdVKey", 0);

			if (bti->vkey == 0)
				bti->bound_state = bs_inactive;

		} else if (bti->bound_state == bs_joystick) {

			register_joystick = true;

			bti->vendorID = IniFile::GetIni().ReadInt(section, L"JoyVendorID", 0);
			bti->productID = IniFile::GetIni().ReadInt(section, L"JoyProductID", 0);
			bti->button_num = IniFile::GetIni().ReadInt(section, L"JoyButtonNum", 0);
			bti->joystick_name = IniFile::GetIni().ReadString(section, L"JoyName", L"");

			if (bti->vendorID == 0  ||  bti->productID == 0  ||  bti->button_num == 0)
				bti->bound_state = bs_inactive;
		} else {
			bti->bound_state = bs_inactive;
		}

		//if (bti->bound_state == bs_joystick)
		//	RegisterRawInput(false, true, win);
		//else if (bti->bound_state == bs_keyboard)
		//	RegisterRawInput(true, false, win);
	}

	RegisterRawInput(register_keyboard, register_joystick, win);

	return;
}

void RawInputHandler::SaveBinding()
{
	for (std::vector<bind_element_t>::iterator bti(_binding_table.begin());
			bti != _binding_table.end(); bti++)
	{
		// don't save anything if we're in the binding state
		if (bti->bound_state == bs_binding)
			continue;

		const wchar_t* section(bti == _binding_table.begin() ? L"Recenter" : L"DisableToggle");

		IniFile::GetIni().WriteInt(section, L"State", bti->bound_state);

		if (bti->bound_state == bs_keyboard)
		{
			IniFile::GetIni().WriteInt(section, L"KbdVKey", bti->vkey);

			IniFile::GetIni().WriteString(section, L"JoyVendorID", L"");
			IniFile::GetIni().WriteString(section, L"JoyProductID", L"");
			IniFile::GetIni().WriteString(section, L"JoyButtonNum", L"");
			IniFile::GetIni().WriteString(section, L"JoyName", L"");

		} else if (bti->bound_state == bs_joystick) {

			IniFile::GetIni().WriteString(section, L"KbdVKey", L"");

			IniFile::GetIni().WriteInt(section, L"JoyVendorID", bti->vendorID);
			IniFile::GetIni().WriteInt(section, L"JoyProductID", bti->productID);
			IniFile::GetIni().WriteInt(section, L"JoyButtonNum", bti->button_num);
			IniFile::GetIni().WriteString(section, L"JoyName", bti->joystick_name);

		} else {

			IniFile::GetIni().WriteString(section, L"KbdVKey", L"");

			IniFile::GetIni().WriteString(section, L"JoyVendorID", L"");
			IniFile::GetIni().WriteString(section, L"JoyProductID", L"");
			IniFile::GetIni().WriteString(section, L"JoyButtonNum", L"");
			IniFile::GetIni().WriteString(section, L"JoyName", L"");
		}
	}
}
