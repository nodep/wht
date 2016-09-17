#pragma once

#define WIDEN2(x)		L ## x
#define WIDEN(x)		WIDEN2(x)

std::wstring int2str(const int64_t i);
std::wstring int2hex(const uint64_t i, const int num_digits = 2);
std::wstring dbl2str(const double dbl, const int decimals = -1);

std::wstring int2str_th(const int64_t i, const wchar_t separator = L'\'');

std::wstring get_date_str();

inline int smaller(const int v1, const int v2)
{
	return v1 < v2 ? v1 : v2;
}

inline int round(const double dbl)
{
	return int(dbl >= 0 ? dbl + 0.5 : dbl - 0.5);
}

inline void debug(const wchar_t* str)
{
	::OutputDebugString(str);
	::OutputDebugString(L"\n");
}

inline void debug(const std::wstring& str)
{
	debug(str.c_str());
}

inline void debug(const int i)
{
	debug(int2str(i));
}
