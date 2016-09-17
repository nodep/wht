#include "stdafx.h"

#include "my_utils.h"

std::wstring int2str(const int64_t i)
{
	const int BUFF_SIZE = 64;
	wchar_t buffer[BUFF_SIZE];
	swprintf_s(buffer, BUFF_SIZE, L"%ld", i);
	return buffer;
}

std::wstring int2hex(const uint64_t i, const int num_digits)
{
	std::wstring format(L"0x%0" + int2str(num_digits) + L"x");
	
	const int BUFF_SIZE = 32;
	wchar_t buff[BUFF_SIZE];

	swprintf_s(buff, BUFF_SIZE, format.c_str(), i);

	return buff;
}

std::wstring dbl2str(const double dbl, const int decimals)
{
	const int BUFF_SIZE = 32;
	wchar_t buff[BUFF_SIZE];

	if (decimals == -1)
		swprintf_s(buff, BUFF_SIZE, L"%g", dbl);
	else
		swprintf_s(buff, BUFF_SIZE, (L"%." + int2str(decimals) + L"f").c_str(), dbl);

	return buff;
}

std::wstring int2str_th(const int64_t i, const wchar_t separator)
{
	std::wstring ret_val, s(int2str(i));

	size_t cnt = 0;
	for (std::wstring::const_reverse_iterator ri(s.rbegin()); ri != s.rend(); ++ri)
	{
		if (cnt++ == 3)
		{
			ret_val = separator + ret_val;
			cnt = 1;
		}

		ret_val = *ri + ret_val;
	}

	return ret_val;
}

std::wstring get_date_str()
{
	const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

	std::string dt_str(__DATE__);
	std::string mnth(dt_str.substr(0, 3));

	int m;
	for (m = 1; m <= 12; ++m)
	{
		if (mnth == months[m-1])
			break;
	}

	int year = atoi(dt_str.c_str() + 7);
	int day = atoi(dt_str.c_str() + 4);

	const int BUFF_SIZE = 64;
	wchar_t buff[BUFF_SIZE];

	swprintf_s(buff, BUFF_SIZE, L"%04d%02d%02d", year, m, day);

	return buff;
}
