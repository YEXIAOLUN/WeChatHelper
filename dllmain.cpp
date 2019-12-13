﻿// dllmain.cpp : 定义 DLL 应用程序的入口点。


#include "pch.h"
#include "AddUser.h"
#include "sendMessage.h"
#include "getchatroomuser.h"
#include "GetUserLists.h"
#include "sendPictureMessage.h"
#include "resource.h"
#include "sendXMLmessage.h"

#include "framework.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <stdlib.h>
#include "commctrl.h"
#include "tlhelp32.h"
#include <string>
#include <cstring>





using namespace std;

HWND hListView = NULL;     //群公众号和好友列表
HWND hListView2 = NULL;		//群联系人列表


#ifdef _X86_
extern "C" { int _afxForceUSRDLL; }
#else
extern "C" { int __afxForceUSRDLL; }
#endif



INT_PTR CALLBACK DialogProc(
	_In_ HWND   hwndDlg,
	_In_ UINT   uMsg,
	_In_ WPARAM wParam,
	_In_ LPARAM lParam
);
void InitListContrl2(HWND List);
void InitListContrl(HWND List);
HWND gUserListView = NULL;
HWND gUserListView2 = NULL;
DWORD	ThreadProc(HMODULE hModule);
VOID UnInjectDll();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_call, LPVOID ipreversed) {
	switch (ul_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, hModule, 0, NULL);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

DWORD	ThreadProc(HMODULE hModule) {
	return DialogBox(hModule, MAKEINTRESOURCE(MAIN), NULL, DialogProc);
}


HWND hwndDlgtemp;

INT_PTR CALLBACK DialogProc(

	_In_ HWND  hwndDlg,
	_In_ UINT  uMsg,
	_In_ WPARAM wparam,
	_In_ LPARAM	lparam
) {
	wchar_t wxid[0x100] = { 0 };
	wchar_t message[0x300] = { 0 };
	HWND hListView = NULL;
	HWND hListView2 = NULL;
	hwndDlgtemp = hwndDlg;
	switch (uMsg) {
	case WM_INITDIALOG:
		//MessageBox(NULL, L"欢迎使用，使用前请先保证电脑上登录过微信，然后打开本软件，在登录微信，由于好友群列表数量可能比较多，发送需要耐心等待一会，发送完成时会有提示", L"aaa", 0);
		SetDlgItemTextW(hwndDlg, MESSAGE, L"群发消息框");
		SetDlgItemTextW(hwndDlg, MESSAGE2, L"发送好友验证消息框");
		SetDlgItemTextW(hwndDlg, MESSAGE3, L"链接文章标题");
		SetDlgItemTextW(hwndDlg, MESSAGE4, L"链接文章副标题");
		SetDlgItemTextW(hwndDlg, MESSAGE5, L"链接文章链接");

		hListView = GetDlgItem(hwndDlg, USER_LISTS);
		hListView2 = GetDlgItem(hwndDlg, USER_LISTS2);
		gUserListView = hListView;
		gUserListView2 = hListView2;
		InitListContrl(hListView);
		InitListContrl2(hListView2);
		HookWechatQrcode(hwndDlg, gUserListView, 0x420994);      //获取好友联系列表
		//HookWechatQrcode(hwndDlg, gUserListView, 0x441894);     //新版微信
		break;
	case WM_COMMAND:
		if (wparam == SEND2) {       //公众号群发(文本信息)
			
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Smessage2, hwndDlg, 0, NULL);
			//Smessage2(hwndDlg);
		}
		if (wparam == SEND3) {     //群群发(文本信息)
			Smessage3(hwndDlg);
		}
		if (wparam == SEND4) {     //其他联系人群发(文本信息)
			Smessage4(hwndDlg);
		}
		if (wparam == SEND5) {    //选择用户群发(文本信息)
			Smessage5(hwndDlg);
		}
		if (wparam == SEND6){    //获取群联系人列表
			DoAction(gUserListView2,hwndDlgtemp);  
		}
		if (wparam == SEND7) {	  //添加指定联系人
			Add7(hwndDlg);      
		}
		if (wparam == SEND8) {    //添加所有联系人
			Add8(hwndDlg);	 
		}
		if (wparam == SEND9) {   //发送xml消息（公众号）
			SendXml12(hwndDlg); 
		}
		if (wparam == SEND10) {  //发送xml消息（群）
			SendXml13(hwndDlg);
		}
		if (wparam == SEND11) {		 //发送xml消息（其他联系人）
			SendXml14(hwndDlg);
		}
		if (wparam == SEND12) {		//发送xml消息（指定联系人）
			SendXml15(hwndDlg);
		}
		if (wparam == SEND13) {    //公众号群发(图片)
			SImage13(hwndDlg);
		}
		if (wparam == SEND14) {		//群群发(图片)
			SImage14(hwndDlg);
		}
		if (wparam == SEND15) {    //其他联系人群发(图片)
			SImage15(hwndDlg);
		}
		if (wparam == SEND16) {    //选择用户群发(图片)
			SImage16(hwndDlg);
		}
		if (wparam == SEND17) {    //选择用户群发(图片)
			GetFriendList();
		}
		if (wparam == SEND18) {    //选择用户群发(图片)
			GetFriendList2();
		}
		if (wparam == SEND19) {    //选择用户群发(图片)
			GetFriendList3();
		}
		break;
	case WM_CLOSE:
		EndDialog(hwndDlg, 0);
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)UnInjectDll, NULL, 0, NULL);
		//UnInjectDll();
		break;
	default:
		break;
	}
	return FALSE;

}




void InitListContrl(HWND List)    //群和好友列表初始化操作
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(List);
	//USER_LISTS.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	ListView_SetExtendedListViewStyle(List, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	LVCOLUMN pcol = { 0 };
	pcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	pcol.fmt = LVCFMT_LEFT;
	pcol.cx = 180;
	pcol.pszText = (LPWSTR)L"wxid";
	ListView_InsertColumn(List, 0, &pcol);
	pcol.pszText = (LPWSTR)L"微信账号";
	ListView_InsertColumn(List, 1, &pcol);
	pcol.pszText = (LPWSTR)L"微信昵称";
	ListView_InsertColumn(List, 2, &pcol);
}
void InitListContrl2(HWND List)  //群里联系人初始化操作
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(List);
	ListView_SetExtendedListViewStyle(List, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	LVCOLUMN pcol = { 0 };
	pcol.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	pcol.fmt = LVCFMT_LEFT;
	pcol.cx = 150;
	pcol.pszText = (LPWSTR)L"wxid";
	ListView_InsertColumn(List, 0, &pcol);
	pcol.cx = 150;
	pcol.pszText = (LPWSTR)L"微信账号";
	ListView_InsertColumn(List, 1, &pcol);
	pcol.cx = 180;
	pcol.pszText = (LPWSTR)L"微信昵称";
	ListView_InsertColumn(List, 2, &pcol);
}


VOID UnInject()
{
	HMODULE hModule = NULL;

	//GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS 会增加引用计数
	//因此，后面还需执行一次FreeLibrary
	//直接使用本函数（UnInject）地址来定位本模块
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPWSTR)&UnInject, &hModule);

	if (hModule != 0)
	{
		
		//从内存中卸载
		FreeLibraryAndExitThread(hModule, 0);

		//减少一次引用计数
		FreeLibrary(hModule);
	}
}





