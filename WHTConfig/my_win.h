#pragma once

#include "my_utils.h"

// this is a very light weight windows class library
// only the stuff I actually use in this project

#define WM_TRAYNOTIFY		(WM_APP+1)

// only icons from resources are supported
class Icon
{
private:
	HICON		_hIcon;
	bool		_is_large;
public:
	Icon(int iconID, bool is_large);

	bool IsLarge() const	{ return _is_large; }

	HICON GetHandle() const
	{
		return _hIcon;
	}
};

// ***************************************************************************

class Font
{
private:
	LOGFONT		_log_font;
	HFONT		_hFont;

	void Clear();
	void CreateFont();
	void ReleaseFont();

	// no copy ctor
	Font(const Font&);

public:
	Font();
	Font(const wchar_t* name, const int height, const bool is_bold = false, const bool is_italic = false);

	~Font()
	{
		ReleaseFont();
	}

	void SetAll(const wchar_t* name, const int height, const bool is_bold = false, const bool is_italic = false);

	HFONT GetHFont() const
	{
		return _hFont;
	}
};

// ***************************************************************************

class Window
{
protected:
	HWND		_hWnd;

public:
	Window()
		: _hWnd(0)
	{}

	void SetHandle(HWND hWnd)	{ _hWnd = hWnd; }
	HWND GetHandle() const		{ return _hWnd; }

	void Destroy()
	{
		::DestroyWindow(_hWnd);
	}

	bool IsValid() const		{ return _hWnd != 0; }
	void GetRect(RECT& r) const
	{
		::GetWindowRect(_hWnd, &r);
	}

	void GetClientRect(RECT& r) const
	{
		::GetClientRect(_hWnd, &r);
	}

	void GetWindowRect(RECT& r) const
	{
		::GetWindowRect(_hWnd, &r);
	}

	bool Enable(bool should_enable)
	{
		return ::EnableWindow(_hWnd, should_enable ? TRUE : FALSE) == 0;
	}

	void Hide()
	{
		::ShowWindow(_hWnd, SW_HIDE);
	}

	void Show()
	{
		::ShowWindow(_hWnd, SW_SHOW);
	}

	bool HasFocus() const
	{
		return _hWnd == ::GetFocus();
	}

	void SetFocus()
	{
		::SetFocus(_hWnd);
	}

	void SetFont(const Font& f)
	{
		::SendMessage(_hWnd, WM_SETFONT, (WPARAM) f.GetHFont(), TRUE);
	}
};

// ***************************************************************************
class TextControl: public Window
{
public:
	// This one sets the window's text only if the new text is different than the current one
	// This avoids flickering. All the other SetText variants call this one
	bool SetText(const wchar_t* str);

	bool SetText(const char* str)
	{
		return ::SetWindowTextA(_hWnd, str) == TRUE;
	}

	bool SetText(const std::wstring& str)
	{
		return SetText(str.c_str());
	}

	bool SetText(const std::string& str)
	{
		return SetText(str.c_str());
	}

	// help the compiler a little :)
	bool SetText(const int val)				{ return SetText(int64_t(val)); }
	bool SetText(const int16_t val)			{ return SetText(int64_t(val)); }
	bool SetText(const size_t val)			{ return SetText(int64_t(val)); }

	bool SetText(const double val, const int num_decimals = 1)
	{
		return SetText(dbl2str(val, num_decimals));
	}

	bool ClearText()
	{
		return SetText(L"");
	}

	std::wstring GetText();

	float GetFloat()
	{
		return (float) _wtof(GetText().c_str());
	}

	int GetInt()
	{
		return _wtoi(GetText().c_str());
	}

	int16_t GetInt16()
	{
		return (int16_t) GetInt();
	}

	// the int and float variants are shortcuts -- we use these a lot
	bool SetText(const int64_t val)
	{
		return SetText(int2str(val));
	}
};

// ***************************************************************************

class ComboBox: public Window
{
public:
	void AddString(const wchar_t* str)
	{
		::SendMessage(_hWnd, CB_ADDSTRING, 0, (LPARAM) str);
	}

	void SetSelection(int selection)
	{
		::SendMessage(_hWnd, CB_SETCURSEL, (WPARAM) selection, 0);
	}

	int GetSelection()
	{
		return ::SendMessage(_hWnd, CB_GETCURSEL, 0, 0);
	}
};

// ***************************************************************************

class CheckBox: public TextControl
{
public:
	void SetState(bool is_checked)
	{
		::SendMessage(_hWnd, BM_SETCHECK, is_checked ? BST_CHECKED : BST_UNCHECKED, 0);
	}

	bool IsChecked()
	{
		return ::SendMessage(_hWnd, BM_GETCHECK, 0, 0) == BST_CHECKED;
	}
};

// ***************************************************************************

class ProgressBar: public Window
{
public:
	void SetRange(uint16_t min, uint16_t max)
	{
		::SendMessage(_hWnd, PBM_SETRANGE, 0, MAKELPARAM(min, max));
	}

	void SetPos(uint16_t pos)
	{
		::SendMessage(_hWnd, PBM_SETPOS, (WPARAM) pos, 0);
	}
};

// ***************************************************************************

class StatusBar: public Window
{
public:
	void SetParts(const int* parts, int num_parts)
	{
		::SendMessage(_hWnd, SB_SETPARTS, num_parts, (LPARAM) parts);
	}

	void SetPartText(int part, const std::wstring& txt)
	{
		::SendMessage(_hWnd, SB_SETTEXT, part, (LPARAM) txt.c_str());
	}
};

// ***************************************************************************

class IndexedSliderBar: public Window
{
private:
	std::vector<int>	_values;

public:
	void SetValues(const std::vector<int>& v)
	{
		_values = v;
		::SendMessage(_hWnd, TBM_SETRANGE, TRUE, (LPARAM) (MAKELPARAM(0, v.size() - 1)));
	}

	int GetValue() const
	{
		size_t pos = ::SendMessage(_hWnd, TBM_GETPOS, 0, 0);
		if (_values.empty())
			return 0;
		else if (pos < 0)
			return _values.front();
		else if (pos >= _values.size())
			return _values.back();

		return _values[pos];
	}

	void SetValue(int val)
	{
		LPARAM pos = 0;

		if (!_values.empty())
		{
			std::vector<int>::iterator f = std::lower_bound(_values.begin(), _values.end(), val);

			::SendMessage(_hWnd, TBM_SETPOS, TRUE, f - _values.begin());
		}
	}
};

// ***************************************************************************

// at the moment, we can't really do anything special with these simple controls that we can not do with a base window
// but I will keep them anyway
class Button: public TextControl	{};
class Label: public TextControl		{};
class EditBox: public TextControl	{};

// ***************************************************************************

class Dialog: public TextControl
{
protected:

	// the dialog procedure
	static LRESULT CALLBACK DialogProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual ~Dialog()	{}

	bool CreateDlg(int dlgID, HWND hWndParent = NULL);

	HWND GetCtrl(int ctrlID)
	{
		return ::GetDlgItem(_hWnd, ctrlID);
	}

	void Hide()
	{
		::ShowWindow(_hWnd, SW_HIDE);
	}

	void BringToTop()
	{
		::BringWindowToTop(_hWnd);
	}

	void Show()
	{
		::ShowWindow(_hWnd, SW_SHOW);
	}

	void MsgBox(const std::wstring& msg, const std::wstring& title, const UINT boxtype)
	{
		::MessageBox(_hWnd, msg.c_str(), title.c_str(), boxtype);
	}

	void SetIcon(const Icon& ic);

	virtual void OnInit()					{}
	virtual void OnTimer(int timerID)		{}
	virtual void OnClose()					{ ::DestroyWindow(_hWnd); }
	virtual void OnDestroy()				{}
	virtual void OnSize(int width, int height, WPARAM wParam)				{}
	virtual void OnSizing(int which_edge, LPRECT lpRect)					{}
	virtual void OnMouseMove(int x, int y, WPARAM wParam)					{}
	virtual void OnLButtonDown(int x, int y, WPARAM wParam)					{}
	virtual void OnLButtonUp(int x, int y, WPARAM wParam)					{}
	virtual void OnMouseWheel(int x, int y, int delta, WPARAM wParam)		{}
	virtual void OnControl(int ctrlID, int notifyID, HWND hWndCtrl)			{}
	virtual void OnMenu(int menuID)											{}
	virtual void OnAccelerator(int acceleratorID)							{}
	virtual bool OnSysCommand(WPARAM wParam)								{ return false; }
	virtual void OnTrayNotify(LPARAM lParam)								{}
	virtual void OnRawInput(size_t input_code, HRAWINPUT hRawInput)			{}

	// pure virtual - the derived class MUST handle exceptions
	virtual void OnException(const std::wstring& str) = 0;
};

// ***************************************************************************

class WaitCursor
{
protected:
	HCURSOR		_prev_cursor;

public:
	WaitCursor()
	{
		_prev_cursor = ::GetCursor();
		::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	}

	void Restore()
	{
		if (_prev_cursor != 0)
		{
			::SetCursor(_prev_cursor);
			_prev_cursor = 0;
		}
	}

	~WaitCursor()
	{
		Restore();
	}
};

// ***************************************************************************

class AcceleratorTable
{
private:
	HACCEL	_hAccel;

	AcceleratorTable(const AcceleratorTable& c);

	AcceleratorTable& operator = (const AcceleratorTable& lhs)
	{
		return *this;
	}

public:
	AcceleratorTable(const size_t id)
		: _hAccel(0)
	{
		Load(id);
	}

	AcceleratorTable()
		: _hAccel(0)
	{}

	void Load(const size_t id)
	{
		Release();
		_hAccel = ::LoadAccelerators(NULL, MAKEINTRESOURCE(id));
	}

	HACCEL GetHAccel()
	{
		return _hAccel;
	}

	void Release()
	{
		if (_hAccel != 0)
		{
			::DestroyAcceleratorTable(_hAccel);
			_hAccel = 0;
		}
	}
};

// ***************************************************************************

// I never liked the std C++ stream library...
class SimpleFile
{
protected:
	HANDLE	_hFile;

public:
	SimpleFile()
		: _hFile(INVALID_HANDLE_VALUE)
	{}

	~SimpleFile()
	{
		Close();
	}

	bool Open(const std::wstring& fname, bool for_write, bool trunc = false);

	void Close()
	{
		if (_hFile != INVALID_HANDLE_VALUE)
			::CloseHandle(_hFile);

		_hFile = NULL;
	}

	int Read(char* buff, const int num_bytes);
	bool Write(const char* buff, const int num_bytes);
	bool SeekToEnd();
};

// ***************************************************************************

class OpenSaveFileDialog
{
private:
	OPENFILENAME				_ofname;
	std::vector<std::wstring>	_filters;
	std::wstring				_default_file_name;
	std::wstring				_default_path;
	std::wstring				_selected_full_file_name;
	std::wstring				_selected_file_name;

	bool Run(const std::wstring& window_title, const Window& winOwner, bool runSave);
	
public:
	OpenSaveFileDialog()
	{
		::memset(&_ofname, 0, sizeof(_ofname));
		_ofname.lStructSize = sizeof(_ofname);
	}

	void AddFilter(const std::wstring& desc, const std::wstring& filter)
	{
		_filters.push_back(desc);
		_filters.push_back(filter);
	}

	void SetDefaultFileName(const std::wstring& fname)
	{
		_default_file_name = fname;
	}

	void SetDefaultPath(const std::wstring& path)
	{
		_default_path = path;
	}

	bool GetSaveFile(const std::wstring& window_title, const Window& winOwner)
	{
		return Run(window_title, winOwner, true);
	}

	bool GetOpenFile(const std::wstring& window_title, const Window& winOwner)
	{
		return Run(window_title, winOwner, false);
	}

	const std::wstring& GetFileName() const			{ return _selected_file_name; }
	const std::wstring& GetFullFileName() const		{ return _selected_full_file_name; }
};

// ***************************************************************************

class MouseCapture
{
private:
	bool	_is_capturing;
public:
	MouseCapture()
		: _is_capturing(false)
	{}

	MouseCapture(const Window& w)
		: _is_capturing(false)
	{
		Capture(w);
	}

	~MouseCapture()
	{
		Release();
	}

	void Capture(const Window& w)
	{
		Release();

		SetCapture(w.GetHandle());
		_is_capturing = true;
	}

	void Release()
	{
		if (_is_capturing)
		{
			ReleaseCapture();
			_is_capturing = false;
		}
	}
};

// ***************************************************************************

// yeah, good ol' ini files - none of this fancy registry rubish ;)
class IniFile
{
private:
	std::wstring	_file_name;

	static IniFile	_ini_file;

public:
	IniFile();

	static IniFile& GetIni()
	{
		return _ini_file;
	}

	void WriteString(const wchar_t* section, const wchar_t* key, const std::wstring& val);
	void WriteInt(const wchar_t* section, const wchar_t* key, const int val);
	void WriteDouble(const wchar_t* section, const wchar_t* key, const double val);

	std::wstring ReadString(const wchar_t* section, const wchar_t* key, const std::wstring& def = L"");
	int ReadInt(const wchar_t* section, const wchar_t* key, const int def = 0);
	double ReadDouble(const wchar_t* section, const wchar_t* key, const double def = 0.0);
};

// ***************************************************************************

class LogFile
{
private:
	HANDLE	_hLogFile;

	bool CheckOpen();

public:
	LogFile()
		: _hLogFile(INVALID_HANDLE_VALUE)
	{}

	void Line(const std::wstring& l);
	void Raw(const std::wstring& r);

	void Flush()
	{
		FlushFileBuffers(_hLogFile);
	}
};

extern LogFile logfile;

// ***************************************************************************

class Stopwatch
{
private:
	static double	_freq;
	int64_t			_start;

public:
	Stopwatch();

	void Start()
	{
		LARGE_INTEGER c;
		QueryPerformanceCounter(&c);

		_start = int64_t(c.QuadPart / _freq);
	}

	int64_t GetMicroSec() const
	{
		LARGE_INTEGER c;
		QueryPerformanceCounter(&c);

		return int64_t(c.QuadPart / _freq) - _start;
	}
};

// ***************************************************************************

// used for timing performance
class PerfTimer
{
private:
	Stopwatch		_swatch;
	std::vector<std::pair<int, int64_t>>	_times;

public:

	void AddTime(const int id);
	void Clear();
	
	int64_t GetMicrosSec() const
	{
		return _swatch.GetMicroSec();
	}

	void Dump(LogFile& lf);
};

// ***************************************************************************

std::wstring GetExePath();
std::wstring GetErrorDescription(DWORD error_num);
std::wstring GetWinClassName(HWND hw);
