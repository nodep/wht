#include "stdafx.h"
#pragma hdrstop

#pragma comment (lib, "winmm.lib")

#include "resource.h"
#include "../dongle/reports.h"

#include "hid.h"
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

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	try {
		// load hid.dll and the necessary functions from it
		if (!InitHID())
			return -1;

		// we need to seed the random generator
		// it's used to generate test ellipsoid data
		srand(GetTickCount());

		// the accelerator table
		AcceleratorTable accTbl(IDR_ACCELERATORS);

		// create & show the app window
		WHTDialog mainDlg;
		mainDlg.CreateDlg(IDD_MAIN_DIALOG);

		// enter the message loop
		int i = 0;
		MSG msg;
		do {
			if (mainDlg.GetMagCompDialog().IsValid()  ||  mainDlg.GetCustomCurvesDialog().IsValid())
			{
				// clear all the messages
				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
				{
					if (TranslateAccelerator(mainDlg.GetMagCompDialog().GetHandle(), accTbl.GetHAccel(), &msg) == 0)
					{
						if (!IsDialogMessage(mainDlg.GetHandle(), &msg)
								&&  !IsDialogMessage(mainDlg.GetMagCompDialog().GetHandle(), &msg))
						{
							TranslateMessage(&msg);
							DispatchMessage(&msg);
						}
					}
				}

				if (mainDlg.GetMagCompDialog().IsValid())
					mainDlg.GetMagCompDialog().Render();

				if (mainDlg.GetCustomCurvesDialog().IsValid())
					mainDlg.GetCustomCurvesDialog().Render();

			} else {

				if (GetMessage(&msg, 0, 0, 0))
				{
					if (!IsDialogMessage(mainDlg.GetHandle(), &msg))
					{
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				}
			}

		} while (msg.message != WM_QUIT);

	} catch (std::wstring& e) {
		MessageBox(0, e.c_str(), L"Exception", MB_OK | MB_ICONERROR);
	}

	return 0;
}