#pragma once

#include "my_win.h"

enum bound_function_e
{
	bf_recenter,
	bf_disable_toggle,

	bf_none,
};

class RawInputHandler
{
private:
	enum bound_state_e
	{
		bs_inactive,
		bs_binding,
		bs_joystick,
		bs_keyboard,
	};

	struct bind_element_t
	{
		bind_element_t()
			: bound_state(bs_inactive)
		{}

		bound_state_e	bound_state;

		Stopwatch		binding_started;

		// if bound to joystick button
		int				button_num;
		USHORT			vendorID;
		USHORT			productID;
		std::wstring	joystick_name;

		// if bound to keyboard
		USHORT			vkey;
	};

	std::vector<bind_element_t>		_binding_table;

	// maps vendorID and productID into a vector of joystick buttons
	std::map<std::pair<int, int>, std::vector<int>>		_ignore_buttons;

	struct DeviceDetails
	{
		USHORT			vendor_id;
		USHORT			product_id;
		std::wstring	product_name;
	};

	// maps the device name (the long, \\HID... name) to the vendorID, productID and joystick name
	std::map<std::wstring, DeviceDetails>	_device_details_cache;

	bool RegisterRawInput(bool reg_keyboard, bool reg_joystick, Window& win);
	void UnregisterRawInput();
	void UnregisterKeyboard();
	void UnregisterJoystick();

	bool GetRawInputIDs(RAWINPUT* pRawInput, USHORT& vendor_id, USHORT& product_id, std::wstring& product_name);
	bool GetActiveJoystickButtons(PRAWINPUT pRawInput, std::vector<int>& activeButtons);

public:
	RawInputHandler();

	~RawInputHandler()
	{
		UnregisterRawInput();
		SaveBinding();
	}

	bool IsBound(const bound_function_e fn) const
	{
		return _binding_table[fn].bound_state == bs_joystick  ||  _binding_table[fn].bound_state == bs_keyboard;
	}

	bool IsBinding(const bound_function_e fn) const
	{
		return _binding_table[fn].bound_state == bs_binding;
	}

	std::wstring BindingStatus(const bound_function_e fn) const;
	std::wstring BoundDevice(const bound_function_e fn) const;
	std::wstring BoundButton(const bound_function_e fn) const;

	void StartBinding(const bound_function_e fn, Window& w);
	void Unbind(const bound_function_e fn);

	bound_function_e ProcessRawInput(HRAWINPUT hRawInput);

	void LoadBinding(Window& win);
	void SaveBinding();
};

