#include "stdafx.h"

#include "my_utils.h"
#include "my_win.h"

Icon::Icon(int iconID, bool is_large)
	: _is_large(is_large)
{
	const int pixels = _is_large ? 48 : 16;
	_hIcon = (HICON) LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(iconID), IMAGE_ICON, pixels, pixels, LR_SHARED);
}

// ***************************************************************************

std::wstring TextControl::GetText()
{
	const int BUFF_SIZE = 2048;		// should be enough, i guess...
	wchar_t buff[BUFF_SIZE];

	int bytes_copied = GetWindowText(_hWnd, buff, BUFF_SIZE);

	return buff;
}

bool TextControl::SetText(const wchar_t* str)
{
	// compare
	if (str != GetText())
		return SetWindowText(_hWnd, str) == TRUE;

	return false;
}

// ***************************************************************************

bool SimpleFile::Open(const std::wstring& fname, bool for_write, bool trunc)
{
	Close();

	_hFile = CreateFile(fname.c_str(),
								for_write ? GENERIC_WRITE : GENERIC_READ,
								FILE_SHARE_READ,
								NULL,
								for_write ? (trunc ? CREATE_ALWAYS : OPEN_ALWAYS) : OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL,
								NULL);

	return _hFile != INVALID_HANDLE_VALUE;
}

int SimpleFile::Read(char* buff, const int num_bytes)
{
	DWORD num_bytes_read;
	BOOL res = ReadFile(_hFile, buff, num_bytes, &num_bytes_read, NULL);

	if (res == FALSE)
		throw std::wstring(L"Unable to read input file.");

	return num_bytes_read;
}

bool SimpleFile::Write(const char* buff, const int num_bytes)
{
	DWORD num_bytes_written;
	BOOL res = WriteFile(_hFile, buff, num_bytes, &num_bytes_written, NULL);

	return res == TRUE  &&  num_bytes == num_bytes_written;
}

bool SimpleFile::SeekToEnd()
{
	return SetFilePointer(_hFile, 0, NULL, FILE_END) != INVALID_SET_FILE_POINTER;
}

// ***************************************************************************

LRESULT CALLBACK Dialog::DialogProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Dialog* pDlg = (Dialog*) GetWindowLong(hwnd, GWL_USERDATA);

	try {
		if (message == WM_INITDIALOG)
		{
			// get the dialog<> pointer as the creation param
			pDlg = reinterpret_cast<Dialog*> (lParam);
			pDlg->_hWnd = hwnd;

			// set the dialog pointer
			SetWindowLong(hwnd, GWL_USERDATA, (LONG) pDlg);

			pDlg->OnInit();		// send the init dialog message

			return TRUE;

		} else if (pDlg == 0) {
			return FALSE;
		}

		switch (message)
		{
		case WM_CLOSE:
			pDlg->OnClose();
			return TRUE;
		case WM_DESTROY:
			pDlg->OnDestroy();
			pDlg->_hWnd = 0;

			// clear the dialog pointer
			SetWindowLong(hwnd, GWL_USERDATA, 0);

			return TRUE;
		case WM_MOUSEMOVE:
			pDlg->OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
			break;
		case WM_LBUTTONDOWN:
			pDlg->OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
			break;
		case WM_LBUTTONUP:
			pDlg->OnLButtonUp(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
			break;
		case WM_SIZE:
			pDlg->OnSize(LOWORD(lParam), HIWORD(lParam), wParam);
			break;
		case WM_SIZING:
			pDlg->OnSizing(int(wParam), LPRECT(lParam));
			break;
		case WM_TIMER:
			pDlg->OnTimer(wParam);
			break;
		case WM_MOUSEWHEEL:
			pDlg->OnMouseWheel(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), GET_WHEEL_DELTA_WPARAM(wParam), wParam);
			return TRUE;
		case WM_COMMAND:

			if (HIWORD(wParam) == 0  &&  lParam == 0)
				pDlg->OnMenu(LOWORD(lParam));
			else if (HIWORD(wParam) == 1  &&  lParam == 0)
				pDlg->OnAccelerator(LOWORD(wParam));
			else
				pDlg->OnControl(LOWORD(wParam), HIWORD(wParam), (HWND) lParam);

			return TRUE;

		case WM_SYSCOMMAND:
			if (pDlg->OnSysCommand(wParam & 0xfff0))
				return TRUE;
			break;
		case WM_TRAYNOTIFY:
			pDlg->OnTrayNotify(lParam);
			return TRUE;
		case WM_INPUT:
			pDlg->OnRawInput(wParam, (HRAWINPUT) lParam);
			return 0;
		case WM_HSCROLL:
		case WM_VSCROLL:

			HWND hCtrl = HWND(lParam);

			// we are translating the SliderBar notification messages to OnControl calls
			if (GetWinClassName(hCtrl) == L"msctls_trackbar32")
			{
				pDlg->OnControl(GetDlgCtrlID(hCtrl), 0, hCtrl);
				return 0;
			}
		}

	} catch (std::wstring& e) {
		if (pDlg != 0)
			pDlg->OnException(e);
		else
			MessageBox(hwnd, e.c_str(), L"Exception", MB_OK | MB_ICONERROR);
	}

	return FALSE;
}

void Dialog::SetIcon(const Icon& ic)
{
	SendMessage(_hWnd, WM_SETICON, ic.IsLarge() ? ICON_BIG : ICON_SMALL, (LPARAM) ic.GetHandle());
} 

bool Dialog::CreateDlg(int dlgID, HWND hWndParent)
{
	HWND hDlg = CreateDialogParam(	GetModuleHandle(NULL),
									MAKEINTRESOURCE(dlgID),
									hWndParent,
									(DLGPROC) DialogProcedure,
									(LPARAM) this);

	return hDlg != NULL;
}

// ***************************************************************************

Font::Font()
	: _hFont(0)
{
	Clear();
}

Font::Font(const wchar_t* name, const int height, const bool is_bold, const bool is_italic)
	: _hFont(0)
{
	Clear();

	SetAll(name, height, is_bold, is_italic);
}

void Font::Clear()
{
	ReleaseFont();

	_log_font.lfHeight = 0;
	_log_font.lfWidth = 0;
	_log_font.lfEscapement = 0;
	_log_font.lfOrientation = 0;
	_log_font.lfWeight = FW_NORMAL;
	_log_font.lfItalic = FALSE;
	_log_font.lfUnderline = FALSE;
	_log_font.lfStrikeOut = FALSE;
	_log_font.lfCharSet = ANSI_CHARSET;
	_log_font.lfOutPrecision = OUT_DEFAULT_PRECIS;
	_log_font.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	_log_font.lfQuality = DEFAULT_QUALITY;
	_log_font.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;
	_log_font.lfFaceName[0] = '\0';
}

void Font::SetAll(const wchar_t* name, const int height, const bool is_bold, const bool is_italic)
{
	ReleaseFont();

	wcsncpy_s(_log_font.lfFaceName, name, sizeof(_log_font.lfFaceName) / sizeof(*_log_font.lfFaceName));
	_log_font.lfHeight = height;
	_log_font.lfWeight = is_bold ? FW_BOLD : FW_NORMAL;
	_log_font.lfItalic = is_italic ? TRUE : FALSE;

	CreateFont();
}

void Font::CreateFont()
{
	ReleaseFont();

	// get a handle to the new font
	_hFont = CreateFontIndirect(&_log_font);
}

void Font::ReleaseFont()
{
	if (_hFont != 0)
	{
		DeleteObject(_hFont);
		_hFont = 0;
	}
}

// ***************************************************************************

bool OpenSaveFileDialog::Run(const std::wstring& window_title, const Window& winOwner, bool runSave)
{
	// the filters
	std::vector<wchar_t> buff;
	std::vector<std::wstring>::iterator filters_iter = _filters.begin();
	while (filters_iter != _filters.end())
	{
		buff.insert(buff.end(), filters_iter->begin(), filters_iter->end());
		buff.push_back(L'\0');

		++filters_iter;
	}

	buff.push_back(L'\0');
	buff.push_back(L'\0');

	_ofname.lpstrFilter = &buff.front();
	_ofname.nFilterIndex = 1;

	// the default file name buffer, also receives the selected file name
	wchar_t file_name[MAX_PATH];
	wcscpy_s(file_name, MAX_PATH, _default_file_name.c_str());

	_ofname.lpstrFile = file_name;
	_ofname.nMaxFile = MAX_PATH - 1;
	_ofname.lpstrInitialDir = _default_path.c_str();

	if (!window_title.empty())
		_ofname.lpstrTitle = window_title.c_str();
	else
		_ofname.lpstrTitle = NULL;

	_ofname.hInstance = GetModuleHandle(NULL);
	_ofname.hwndOwner = winOwner.GetHandle();
	_ofname.Flags = runSave ? OFN_OVERWRITEPROMPT : 0;

	BOOL result;
	
	if (runSave)
		result = GetSaveFileName(&_ofname);
	else
		result = GetOpenFileName(&_ofname);

	if (result != 0)
	{
		_selected_full_file_name = file_name;
		_selected_file_name = file_name + _ofname.nFileOffset;
	}

	return result != 0;
}

// ***************************************************************************

IniFile IniFile::_ini_file;

IniFile::IniFile()
{
	// set the exe name
	wchar_t full_exe_name[MAX_PATH];
	size_t name_length = GetModuleFileName(NULL, full_exe_name, MAX_PATH);

	if (name_length > 3)
	{
		// replace the EXE with INI
		full_exe_name[name_length - 3] = L'i';
		full_exe_name[name_length - 2] = L'n';
		full_exe_name[name_length - 1] = L'i';

		_file_name = full_exe_name;
	} else {
		_file_name = full_exe_name;
		_file_name += L".ini";
	}
}

void IniFile::WriteString(const wchar_t* section, const wchar_t* key, const std::wstring& val)
{
	WritePrivateProfileString(section, key, val.c_str(), _file_name.c_str());
}

void IniFile::WriteInt(const wchar_t* section, const wchar_t* key, const int val)
{
	WritePrivateProfileString(section, key, int2str(val).c_str(), _file_name.c_str());
}

void IniFile::WriteDouble(const wchar_t* section, const wchar_t* key, const double val)
{
	const int BUFF_SIZE = 32;
	wchar_t buff[BUFF_SIZE];

	swprintf_s(buff, BUFF_SIZE, L"%g", val);

	WritePrivateProfileString(section, key, buff, _file_name.c_str());
}

std::wstring IniFile::ReadString(const wchar_t* section, const wchar_t* key, const std::wstring& def)
{
	wchar_t ret_string[MAX_PATH];
	GetPrivateProfileString(section, key, def.c_str(), ret_string, MAX_PATH, _file_name.c_str());
	return ret_string;
}

int IniFile::ReadInt(const wchar_t* section, const wchar_t* key, const int def)
{
	return GetPrivateProfileInt(section, key, def, _file_name.c_str());
}

double IniFile::ReadDouble(const wchar_t* section, const wchar_t* key, const double def)
{
	wchar_t ret_string[MAX_PATH];
	DWORD result = GetPrivateProfileString(section, key, L"", ret_string, MAX_PATH, _file_name.c_str());

	if (result == 0)
		return def;
	
	return _wtof(ret_string);
}

// ***************************************************************************

double Stopwatch::_freq(0);

Stopwatch::Stopwatch()
{
	if (_freq == 0)
	{
		LARGE_INTEGER f;
		QueryPerformanceFrequency(&f);
		_freq = double(f.QuadPart / 1e6);
	}

	Start();
}

// ***************************************************************************

PerfTimer pt;

void PerfTimer::AddTime(const int id)
{
	_times.push_back(std::pair<int, int64_t>(id, _swatch.GetMicroSec()));
}

void PerfTimer::Dump(LogFile& logfile)
{
	std::wstring msg(L"---\n");
	const int BUFF_SIZE = 64;
	wchar_t buff[BUFF_SIZE];
	for (std::vector<std::pair<int, int64_t>>::iterator ti(_times.begin() + 1);
				ti != _times.end();
				++ti)
	{
		_itow_s(ti->first, buff, BUFF_SIZE, 10);
		msg += buff;
		msg += L' ';

		_i64tow_s(ti->second - _times.front().second, buff, BUFF_SIZE, 10);
		msg += buff;
		msg += L'\n';
	}

	logfile.Raw(msg);

	_times.clear();
}

// ***************************************************************************

std::wstring GetExePath()
{
	wchar_t full_exe_name[MAX_PATH];
	size_t name_length = GetModuleFileName(NULL, full_exe_name, MAX_PATH);

	std::wstring ret_val(full_exe_name);

	size_t ndx = ret_val.rfind(L'\\');
	if (ndx != std::wstring::npos)
		ret_val = ret_val.substr(0, ndx + 1);
	else
		ret_val.clear();

	return ret_val;
}

// ***************************************************************************

std::wstring GetErrorDescription(DWORD error_num)
{
	// get the error text from the system
	const int BUFF_SIZE = 4096;
	wchar_t buffer[BUFF_SIZE];
	buffer[0] = L'\0';
	int result = ::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
					0,
					error_num,
					MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
					buffer,
					BUFF_SIZE,
					0);

	return buffer;
}

// ***************************************************************************

LogFile logfile;

bool LogFile::CheckOpen()
{
	if (_hLogFile != INVALID_HANDLE_VALUE)
		return true;

	std::wstring file_name;

	// get the exe name
	wchar_t full_exe_name[MAX_PATH];
	size_t name_length = GetModuleFileName(NULL, full_exe_name, MAX_PATH);

	if (name_length > 3)
	{
		// replace the EXE with txt
		full_exe_name[name_length - 3] = L't';
		full_exe_name[name_length - 2] = L'x';
		full_exe_name[name_length - 1] = L't';

		file_name = full_exe_name;
	} else {
		file_name = full_exe_name;
		file_name += L".txt";
	}

	_hLogFile = CreateFile(file_name.c_str(),
							GENERIC_WRITE | GENERIC_READ,
							FILE_SHARE_READ,
							NULL,
							OPEN_ALWAYS,
							FILE_ATTRIBUTE_NORMAL,
							NULL);

	if (_hLogFile == INVALID_HANDLE_VALUE)
		return false;

	// file pointer to the end of file
	DWORD file_size = SetFilePointer(_hLogFile, 0, NULL, FILE_END);

	// if the file is empty write the UTF-8 magic numbers
	if (file_size == 0)
	{
		// magic number
		DWORD written;
		WriteFile(_hLogFile, "\xff\xfe", 2, &written, NULL);
		assert(written == 2);
	}

	return true;
}

void LogFile::Line(const std::wstring& l)
{
	if (!CheckOpen())
		return;

	// current time
	SYSTEMTIME systime;
	::GetLocalTime(&systime);

	const int BUFF_SIZE = 128;
	wchar_t datestr[128];
	swprintf_s(datestr, BUFF_SIZE, L"%04i/%02i/%02i %02i:%02i:%02i.%03i ", systime.wYear, systime.wMonth, systime.wDay,
																			systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds);
	DWORD written;

	// make the line
	std::wstring line(datestr + l + L"\n");

	// write the line
	WriteFile(_hLogFile, line.c_str(), line.length() * 2, &written, NULL);
	assert(written == line.length() * 2);
}

void LogFile::Raw(const std::wstring& r)
{
	if (!CheckOpen())
		return;

	DWORD written;

	// write the string
	WriteFile(_hLogFile, r.c_str(), r.length() * 2, &written, NULL);
	assert(written == r.length() * 2);

	//::FlushFileBuffers(hLogFile);
	//::CloseHandle(hLogFile);
}

std::wstring GetWinClassName(HWND hw)
{
	const int max_class_name_chars = 1024;
	wchar_t lpClassName[max_class_name_chars];
	int res = ::GetClassNameW(hw, lpClassName, max_class_name_chars);

	if (res == 0)
		return L"";

	return lpClassName;
}
