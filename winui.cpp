﻿
#include "resource.h"
#include <string>
#include <atlstr.h>
#include <Windows.h>

DWORD QmflacDecrypt(BYTE* InData,DWORD Size,BYTE** OutDataPtr,DWORD* ErrorInfo);

WCHAR* GetFileExt(WCHAR* FileName)
{
	int Len=wcslen(FileName);
	for (WCHAR* n=FileName+Len-1;n!=FileName;n--)
	{
		if (*n==L'.')
			return n;
		else if (*n==L'\\')
			return NULL;
	}
	return NULL;
}

void OnInitDialog(HWND hWndDlg)
{

}

void OnClose(HWND hWndDlg)
{

}

void OnPaint(HDC hDC)
{
	//Graphics g(hDC);
}

void OnCommand(HWND hWndDlg,int nCtlID,int nNotify)
{
	//nNotify==1,accelerator
	//nNotify==0,menu
	if (nNotify==BN_CLICKED && nCtlID==IDB_SAVE)
	{
		int Len=GetWindowTextLength(GetDlgItem(hWndDlg,IDE_PATH));
		if (Len!=0)
		{
			WCHAR* Name=new WCHAR[Len+1];
			GetDlgItemText(hWndDlg,IDE_PATH,Name,Len+1);
			CStringW dir_name(Name);
			WIN32_FIND_DATA ffd;
			CStringW file_filter = Name;
			file_filter += "\\*.*";
		  HANDLE hFind = INVALID_HANDLE_VALUE;
			hFind = FindFirstFile(file_filter, &ffd);
			if (hFind == INVALID_HANDLE_VALUE) {
				MessageBox(hWndDlg,L"无法打开文件",L"ok",MB_OK);
				return;
			}

			do {
				
				CStringW file_name = dir_name;
				file_name += L"\\";
				file_name += ffd.cFileName;
				if (file_name.Right(6) != L".mflac" && file_name.Right(4) != L".mgg")
					continue;
				HANDLE hFileR = CreateFile(file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hFileR == INVALID_HANDLE_VALUE)
				{
					MessageBox(hWndDlg, L"无法打开文件", L"ok", MB_OK);
				}
				DWORD Size = GetFileSize(hFileR, NULL);
				BYTE* ReadBuf = new BYTE[Size];
				memset(ReadBuf, 0, Size);
				ReadFile(hFileR, ReadBuf, Size, &Size, NULL);

				BYTE* OutData = NULL;
				DWORD ErrorInfo = 0;
				Size = QmflacDecrypt(ReadBuf, Size, &OutData, &ErrorInfo);

				WCHAR LogBuf[MAX_PATH + 20];
				if (Size == 0)
				{
					wsprintf(LogBuf, L"解锁失败 %08X", ErrorInfo);
					MessageBox(hWndDlg, LogBuf, L"ok", MB_OK);
					continue;
				}
				else
				{
					if (file_name.Right(6) == (L".mflac")) {
						file_name = file_name.Left(file_name.GetLength() - 6) + L".flac";
					}
					else if (file_name.Right(4) == (L".mgg")) {
						file_name = file_name.Left(file_name.GetLength() - 4) + L".ogg";
					}
					else {
						MessageBox(hWndDlg, file_name + " Can't open", L"OK", MB_OK);
						continue;
					}
					//WCHAR* Ext = GetFileExt(file_name);
					//if (wcscmp(Ext, L".mflac") == 0)
					//	memcpy(Name + Len - 5, L"flac\0", 5 * sizeof(WCHAR));
					//else if (wcscmp(Ext, L".mgg") == 0)
					//	memcpy(Name + Len - 3, L"ogg\0", 4 * sizeof(WCHAR));

					HANDLE hFileW = CreateFile(file_name, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					WriteFile(hFileW, OutData, Size, &Size, NULL);
					CloseHandle(hFileW);
					delete[] OutData;
				}

					//wsprintf(LogBuf, L"解锁成功，保存为：\n%s", Name);
					//MessageBox(hWndDlg, LogBuf, L"ok", MB_OK);

			} while (FindNextFile(hFind, &ffd) != 0);
		  MessageBox(hWndDlg, L"结束", L"OK", MB_OK);
			return;

			{
				HANDLE hFileR=CreateFile(Name,GENERIC_READ,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
				if (hFileR==INVALID_HANDLE_VALUE)
				{
					MessageBox(hWndDlg,L"无法打开文件",L"ok",MB_OK);
				}
				DWORD Size=GetFileSize(hFileR,NULL);
				BYTE* ReadBuf=new BYTE[Size];
				memset(ReadBuf,0,Size);
				ReadFile(hFileR,ReadBuf,Size,&Size,NULL);

				BYTE* OutData=NULL;
				DWORD ErrorInfo=0;
				Size=QmflacDecrypt(ReadBuf,Size,&OutData,&ErrorInfo);

				WCHAR LogBuf[MAX_PATH+20];
				if (Size==0)
				{
					wsprintf(LogBuf,L"解锁失败 %08X",ErrorInfo);
					MessageBox(hWndDlg,LogBuf,L"ok",MB_OK);
				}
				else
				{
					WCHAR* Ext=GetFileExt(Name);
					if (wcscmp(Ext,L".mflac")==0)
						memcpy(Name+Len-5,L"flac\0",5*sizeof(WCHAR));
					else if (wcscmp(Ext,L".mgg")==0)
						memcpy(Name+Len-3,L"ogg\0",4*sizeof(WCHAR));

					HANDLE hFileW=CreateFile(Name,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
					WriteFile(hFileW,OutData,Size,&Size,NULL);
					CloseHandle(hFileW);
					delete[] OutData;

					wsprintf(LogBuf,L"解锁成功，保存为：\n%s",Name);
					MessageBox(hWndDlg,LogBuf,L"ok",MB_OK);
				}

				delete[] ReadBuf;
				CloseHandle(hFileR);
			}
			delete[] Name;
		}
	}
}

void OnTimer(HWND hWndDlg,int nTmrID)
{

}

void OnDropFile(HWND hWndDlg,HDROP hDrop)
{
	RECT Area;
	GetWindowRect(GetDlgItem(hWndDlg,IDS_DROP),&Area);
	POINT Pos;
	DragQueryPoint(hDrop,&Pos);
	if (PtInRect(&Area,Pos))
	{
		int Num=DragQueryFile(hDrop,0xFFFFFFFF,NULL,0);
		if (Num!=1)
		{
			MessageBox(hWndDlg,L"只支持每次1个文件",L"ok",MB_OK);
		}
		else
		{
			int Len=DragQueryFile(hDrop,0,NULL,0);
			WCHAR* Name=new WCHAR[Len+1];
			Len=DragQueryFile(hDrop,0,Name,Len+1);
			WCHAR* Ext=GetFileExt(Name);	//相信DragQueryFile的结果，不进行额外检测
			CStringW path(Name);
			path.Truncate(path.ReverseFind(L'\\'));
			if (Ext==NULL || (wcscmp(Ext,L".mflac")!=0 && wcscmp(Ext,L".mgg")!=0))
				MessageBox(hWndDlg,L"只支持.mflac和.mgg文件",L"ok",MB_OK);
			else
				SetDlgItemText(hWndDlg,IDE_PATH,path);
			delete[] Name;
		}
	}
	else {
			MessageBox(hWndDlg,L"怎么回事？",L"ok",MB_OK);

	}
	DragFinish(hDrop);
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		OnInitDialog(hwndDlg);
		return TRUE;
	case WM_CLOSE:
		OnClose(hwndDlg);
		EndDialog(hwndDlg,0);
		return TRUE;
	case WM_ERASEBKGND:
		return FALSE;	//nonzero if erased background
	case WM_PAINT:
		PAINTSTRUCT ps;
		BeginPaint(hwndDlg,&ps);
		OnPaint(ps.hdc);
		EndPaint(hwndDlg,&ps);
		return TRUE;
	case WM_COMMAND:
		OnCommand(hwndDlg,LOWORD(wParam),HIWORD(wParam));
		return TRUE;
	case WM_TIMER:
		OnTimer(hwndDlg,wParam);
		return TRUE;
	case WM_DROPFILES:
		OnDropFile(hwndDlg,(HDROP)wParam);
		return TRUE;
	}
	return FALSE;
}

void RunWinUI()
{
	DialogBoxParam(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_DIALOG1),NULL,DialogProc,0);
}


