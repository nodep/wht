#pragma once

class Dll
{
private:
	HMODULE		_hDll;

public:
	Dll()
		: _hDll(0)
	{}

	~Dll()
	{
		Release();
	}

	void Release()
	{
		if (_hDll != 0)
		{
			::FreeLibrary(_hDll);
			_hDll = 0;
		}
	}

	bool IsEmpty() const
	{
		return _hDll == 0;
	}

	bool Load(const wchar_t* dll_name)
	{
		Release();

		_hDll = ::LoadLibrary(dll_name);
		
		return _hDll != 0;
	}

	template <typename ProcType>
	bool GetProc(ProcType& p, const char* proc_name)
	{
		p = (ProcType) ::GetProcAddress(_hDll, proc_name);
		return p != 0;
	}
};
