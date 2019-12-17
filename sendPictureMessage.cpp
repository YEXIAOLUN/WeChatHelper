#include "pch.h"
#include "sendPictureMessage.h"
#include "sendMessage.h"
#include "resource.h"
#include <string>
#include "sendMessage.h"
#include <afxdlgs.h>
#include <cstring>
using namespace std;


//发送图片的结构体
struct SendImageStruct
{
	wchar_t* pWxid = nullptr;
	DWORD length = 0;
	DWORD maxLength = 0;
	DWORD fill1 = 0;
	DWORD fill2 = 0;
	wchar_t wxid[1024] = { 0 };
};

//************************************************************
// 函数名称: SendImageMessage
// 函数说明: 发送图片消息
// 作    者: 占家宝
// 时    间: 2019/12/32
// 参    数: wxid 微信ID filepath 图片路径
// 返 回 值: void 
//************************************************************
VOID SendImageMessage(wchar_t* wxid, wchar_t* filepath)
{
	//组装微信ID的数据结构
	SendImageStruct imagewxid;
	memcpy(imagewxid.wxid, wxid, wcslen(wxid) + 1);
	imagewxid.length = wcslen(wxid) + 1;
	imagewxid.maxLength = wcslen(wxid) * 2;
	imagewxid.pWxid = wxid;

	//组装文件路径的数据结构
	SendImageStruct imagefilepath;
	memcpy(imagefilepath.wxid, filepath, wcslen(filepath) + 1);
	imagefilepath.length = wcslen(filepath) + 1;
	imagefilepath.maxLength = MAX_PATH;
	imagefilepath.pWxid = filepath;

	char buff[0x45C] = { 0 };

	DWORD dwCall1 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x7d250;
	DWORD dwCall2 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x2d9b00;
	DWORD myEsp = 0;

	/*
	//微信发送图片调用了GidCreateBimapFileCM 之后图片一直被占用 无法删除 patch掉下面的代码
	unsigned char oldcode[5] = { 0 };
	unsigned char fix[5] = { 0x31,0xC0,0x90,0x90,0x90 };
	DWORD dwPathcAddr = (DWORD)GetModuleHandle(L"WeChatWin.dll") + WxPatchAddr;
	//修改代码段属性
	DWORD dwOldAttr = 0;
	VirtualProtect((LPVOID)dwPathcAddr, 5, PAGE_EXECUTE_READWRITE, &dwOldAttr);
	//先保存原有指令
	memcpy(oldcode, (LPVOID)dwPathcAddr, 5);
	//再Patch掉
	memcpy((LPVOID)dwPathcAddr, fix, 5);
	*/

	__asm
	{
		//保存esp
		mov myEsp, esp;

		lea ebx, imagefilepath;
		push ebx;
		lea eax, imagewxid;
		push eax;
		lea eax, buff;
		push eax;
		push 0;
		call dwCall1;
		add esp, 0x4;
		mov ecx, eax;
		call dwCall2;
		//恢复堆栈
		mov eax, myEsp;
		mov esp, eax;
	}
	/*
	//调用完之后恢复
	memcpy((LPVOID)dwPathcAddr, oldcode, 5);
	//恢复属性
	VirtualProtect((LPVOID)dwPathcAddr, 5, dwOldAttr, &dwOldAttr);
	*/
	
}



VOID SImage13(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	wchar_t messagetemp2[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	CString strFile = OpenImage();
	swprintf_s(messagetemp, L"%s", strFile);
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "gh_") != NULL) {
			GetDlgItemText(hwndDlg, MESSAGE, messagetemp2, sizeof(messagetemp2));
			SendImageMessage(wxid, messagetemp);
			SendWXIDTextMessage(wxid, messagetemp2);
			Sleep(1000);
		}


	}
	MessageBox(NULL, L"发送成功", L"提示", 0);
}   //公众号发送
VOID SImage14(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	wchar_t messagetemp2[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	CString strFile = OpenImage();
	swprintf_s(messagetemp, L"%s", strFile);
	int pos = 1;
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "@chatroom") != NULL) {
			GetDlgItemText(hwndDlg, MESSAGE, messagetemp2, sizeof(messagetemp2));
			
			SendImageMessage(wxid, messagetemp);
			SendWXIDTextMessage(wxid, messagetemp2);
			Sleep(1000);
		}


	}
	MessageBox(NULL, L"发送成功", L"提示", 0);
}	//群发送
VOID SImage15(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	wchar_t messagetemp2[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = -1;
	CString strFile = OpenImage();
	swprintf_s(messagetemp, L"%s", strFile);
	int sums = ListView_GetItemCount(hiew);
	for(int i=0;i<sums;i++) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "@chatroom") == NULL && strstr(str.c_str(), "gh") == NULL) {
			GetDlgItemText(hwndDlg, MESSAGE, messagetemp2, sizeof(messagetemp2));
			SendImageMessage(wxid, messagetemp);
			SendWXIDTextMessage(wxid, messagetemp2);
			Sleep(1000);
		}


	}
	MessageBox(NULL, L"发送成功", L"提示", 0);
}
VOID SImage16(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	wchar_t messagetemp2[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	CString strFile = OpenImage();
	swprintf_s(messagetemp, L"%s", strFile);
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		if (bo != 0) {
			ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
			wchar_t wxid[0x100] = { 0 };
			swprintf_s(wxid, L"%s", wstrText);
			GetDlgItemText(hwndDlg, MESSAGE, messagetemp2, sizeof(messagetemp2));
			//MessageBox(NULL, messagetemp2, L"aaa", 0);
			SendImageMessage(wxid, messagetemp);
			SendWXIDTextMessage(wxid, messagetemp2);
			Sleep(1000);
		}
		
	}
	MessageBox(NULL, L"发送成功", L"提示", 0);
}

VOID SImage17(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	wchar_t messagetemp2[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = -1;
	int sums = ListView_GetItemCount(hiew);
	CString strFile = OpenImage();
	swprintf_s(messagetemp, L"%s", strFile);
	for(int i=0;i<sums;i++) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		GetDlgItemText(hwndDlg, MESSAGE, messagetemp2, sizeof(messagetemp2));
		//MessageBox(NULL, messagetemp2, L"aaa", 0);
		SendImageMessage(wxid, messagetemp);
		SendWXIDTextMessage(wxid, messagetemp2);
		Sleep(1000);

	}
	MessageBox(NULL, L"发送成功", L"提示", 0);
}

CString OpenImage() {
	CString strFile = _T("");
	AfxSetResourceHandle(GetModuleHandle(NULL));
	CFileDialog   dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("Describe Files (*.jpg)|*.jpg|All Files (*.*)|*.*||"), (CWnd*)AfxGetResourceHandle());

	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
		return strFile;
	}
	return strFile;
	
}

