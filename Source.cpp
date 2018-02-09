#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "dsound")

#include <windows.h>
#include <dsound.h>
#include "resource.h"

TCHAR szClassName[] = TEXT("Window");

BOOL LoadSoundFromResource(IDirectSound *ds, INT nID, IDirectSoundBuffer** dsb)
{
	DSBUFFERDESC dsbd = { sizeof(DSBUFFERDESC) };
	dsbd.dwFlags = DSBCAPS_STATIC | (DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY);
	DWORD *pRData = (DWORD*)LockResource(LoadResource(GetModuleHandle(0), FindResource(GetModuleHandle(0), MAKEINTRESOURCE(nID), TEXT("WAVE"))));
	dsbd.dwBufferBytes = *(pRData + 10);
	dsbd.lpwfxFormat = (LPWAVEFORMATEX)(pRData + 5);
	ds->CreateSoundBuffer(&dsbd, dsb, NULL);
	LPBYTE pMem1, pMem2;
	DWORD dwSize1, dwSize2;
	(*dsb)->Lock(0, dsbd.dwBufferBytes, (void**)&pMem1, &dwSize1, (void**)&pMem2, &dwSize2, 0);
	memcpy(pMem1, (LPBYTE)(pRData + 11), dwSize1);
	if (dwSize2 != 0)
	{
		memcpy(pMem2, (LPBYTE)(pRData + 11) + dwSize1, dwSize2);
	}
	(*dsb)->Unlock(pMem1, dwSize1, pMem2, dwSize2);
	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static IDirectSound *ds;
	static IDirectSoundBuffer *dsb1, *dsb2;
	static HWND hButton1;
	static HWND hButton2;
	switch (msg)
	{
	case WM_CREATE:
		hButton1 = CreateWindow(TEXT("BUTTON"), TEXT("音を鳴らす1"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton2 = CreateWindow(TEXT("BUTTON"), TEXT("音を鳴らす2"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDCANCEL, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		DirectSoundCreate(NULL, &ds, NULL);
		ds->SetCooperativeLevel(hWnd, DSSCL_NORMAL);
		LoadSoundFromResource(ds, IDR_WAVE1, &dsb1);
		LoadSoundFromResource(ds, IDR_WAVE2, &dsb2);
		break;
	case WM_SIZE:
		MoveWindow(hButton1, 10, 10, 256, 32, TRUE);
		MoveWindow(hButton2, 10, 50, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			dsb1->SetCurrentPosition(0);
			dsb1->Play(0, 0, 0);
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			dsb2->SetCurrentPosition(0);
			dsb2->Play(0, 0, 0);
		}
		break;
	case WM_DESTROY:
		ds->Release();
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("DirectSound を使って音を鳴らす（多重再生も可能）"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
