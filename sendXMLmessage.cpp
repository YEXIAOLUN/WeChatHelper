// SendXmlMessage.cpp : 定义 DLL 应用程序的导出函数。
//

#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include "sendXMLmessage.h"
#include "sendMessage.h"
#include "sendPictureMessage.h"

using namespace std;

void SendXml(wchar_t* title, wchar_t* subtitle, wchar_t* linkurl, wchar_t* pic, wchar_t* SendWxid, wchar_t* RevWxid)
{

	//群ID结构体 
	struct RoomIdStruct
	{
		wchar_t* roomid;
		int roomidLen;
		int roomidMaxLen;
		int full = 0;
		int full2 = 0;
	};


	char buff1[0x44] = { 0 };
	char buff2[0x64] = { 0 };
	char maxBuff[0x77C] = { 0 };
	wchar_t iconPath[0x9C] = { 0 };
	//swprintf_s(iconPath, L"C:\\a.jpg");

	wchar_t xml[0x2000] = { 0 };	//1.自己的微信ID																									2.标题          3.副标题																				  4.url																																																																																							    
	swprintf_s(xml, L"<msg><fromusername>%s</fromusername><scene>0</scene><commenturl></commenturl><appmsg appid=\"\" sdkver=\"0\"><title>%s</title><des>%s</des><action>view</action><type>5</type><showtype>0</showtype><content></content><url>%s</url><dataurl></dataurl><lowurl></lowurl><thumburl></thumburl><messageaction></messageaction><recorditem><![CDATA[]]></recorditem><extinfo></extinfo><sourceusername></sourceusername><sourcedisplayname></sourcedisplayname><commenturl></commenturl><appattach><totallen>0</totallen><attachid></attachid><emoticonmd5></emoticonmd5><fileext></fileext><cdnthumburl></cdnthumburl><cdnthumbaeskey>1db5e2493df05eb9cbc07a90d0f59540</cdnthumbaeskey><cdnthumblength>3934</cdnthumblength><cdnthumbheight>150</cdnthumbheight><cdnthumbwidth>150</cdnthumbwidth></appattach></appmsg><appinfo><version>1</version><appname>Window wechat</appname></appinfo></msg>", SendWxid, title, subtitle, linkurl);

	RoomIdStruct xmldata;
	xmldata.roomid = xml;
	xmldata.roomidLen = wcslen(xml) + 1;
	xmldata.roomidMaxLen = (wcslen(xml) + 1) * 2;

	RoomIdStruct sendiddata;
	sendiddata.roomid = SendWxid;
	sendiddata.roomidLen = wcslen(SendWxid) + 1;
	sendiddata.roomidMaxLen = (wcslen(SendWxid) + 1) * 2;


	RoomIdStruct reviddata;
	reviddata.roomid = RevWxid;
	reviddata.roomidLen = wcslen(RevWxid) + 1;
	reviddata.roomidMaxLen = (wcslen(RevWxid) + 1) * 2;


	RoomIdStruct picpath;
	picpath.roomid = pic;
	picpath.roomidLen = wcslen(pic) + 1;
	picpath.roomidMaxLen = (wcslen(pic) + 1) * 2;


	char* asmXml = (char*)&xmldata.roomid;
	char* asmWxid = (char*)&reviddata.roomid;
	char* asmFWxid = (char*)&sendiddata.roomid;


	//拿到call的数据地址 
	DWORD dwCall1 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x22B990;
	DWORD dwCall2 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x11393AC;
	DWORD dwCall3 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x22BBA0;
	DWORD dwCall4 = (DWORD)GetModuleHandle(L"WeChatWin.dll") + 0x402750;


	__asm
	{
		pushad;
		push 0x5;
		lea eax, buff1;			//0x14大小缓冲区
		push eax;
		lea eax, picpath.roomid;//图片结构的地址
		push eax;
		mov eax, asmXml;		//xml内容结构体
		push eax;
		mov eax, asmWxid;		//接收者的微信ID结构体
		push eax;
		mov eax, asmFWxid;		//发送者的微信ID结构体
		push eax;
		lea eax, maxBuff;		//0x778大小缓冲区
		push eax;
		call dwCall1;
		push dwCall2;
		push dwCall2;
		push 0x0;
		lea eax, maxBuff;
		push eax;
		call dwCall3;
		lea ecx, maxBuff;
		call dwCall4;
		popad;
	}
}

#include <atlconv.h>
VOID SendXml12(HWND hwndDlg) {
	wchar_t pic[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	CString strFile = OpenImage();
	swprintf_s(pic, L"%s", strFile);

	wchar_t title[0x3000] = { 0 };
	wchar_t content[0x3000] = { 0 };
	wchar_t linkurl[0x3000] = { 0 };
	USES_CONVERSION;
	char* szBuf = (char*)GetModuleHandle(L"WeChatWin.dll") + 0X1131BEC;
	const char* cid = (const char*) * ((DWORD*)szBuf);
	wchar_t* id = A2W(cid);
	GetDlgItemText(hwndDlg, MESSAGE3, title, sizeof(title));
	GetDlgItemText(hwndDlg, MESSAGE4, content, sizeof(content));
	GetDlgItemText(hwndDlg, MESSAGE5, linkurl, sizeof(linkurl));
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "gh_") != NULL) {
			SendXml(title, content, linkurl, pic, id, wxid);
			Sleep(1000);
		}

	}
	MessageBox(NULL, L"发送成功", L"aaa", 0);
}
VOID SendXml13(HWND hwndDlg) {
	wchar_t pic[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	CString strFile = OpenImage();
	swprintf_s(pic, L"%s", strFile);

	wchar_t title[0x3000] = { 0 };
	wchar_t content[0x3000] = { 0 };
	wchar_t linkurl[0x3000] = { 0 };
	USES_CONVERSION;
	char* szBuf = (char*)GetModuleHandle(L"WeChatWin.dll") + 0X1131BEC;
	const char* cid = (const char*) * ((DWORD*)szBuf);
	wchar_t* id = A2W(cid);
	GetDlgItemText(hwndDlg, MESSAGE3, title, sizeof(title));
	GetDlgItemText(hwndDlg, MESSAGE4, content, sizeof(content));
	GetDlgItemText(hwndDlg, MESSAGE5, linkurl, sizeof(linkurl));
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "@chatroom") != NULL) {
			SendXml(title, content, linkurl, pic, id, wxid);
			Sleep(1000);
		}

	}
	MessageBox(NULL, L"发送成功", L"aaa", 0);
}
VOID SendXml14(HWND hwndDlg) {
	wchar_t pic[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	CString strFile = OpenImage();
	swprintf_s(pic, L"%s", strFile);

	wchar_t title[0x3000] = { 0 };
	wchar_t content[0x3000] = { 0 };
	wchar_t linkurl[0x3000] = { 0 };
	USES_CONVERSION;
	char* szBuf = (char*)GetModuleHandle(L"WeChatWin.dll") + 0X1131BEC;
	const char* cid = (const char*) * ((DWORD*)szBuf);
	wchar_t* id = A2W(cid);
	GetDlgItemText(hwndDlg, MESSAGE3, title, sizeof(title));
	GetDlgItemText(hwndDlg, MESSAGE4, content, sizeof(content));
	GetDlgItemText(hwndDlg, MESSAGE5, linkurl, sizeof(linkurl));
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "@chatroom") == NULL && strstr(str.c_str(), "gh") == NULL) {
			SendXml(title, content, linkurl, pic, id, wxid);
			Sleep(1000);
		}

	}
	MessageBox(NULL, L"发送成功", L"aaa", 0);
}
VOID SendXml15(HWND hwndDlg) {
	wchar_t pic[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	CString strFile = OpenImage();
	swprintf_s(pic, L"%s", strFile);

	wchar_t title[0x3000] = { 0 };
	wchar_t content[0x3000] = { 0 };
	wchar_t linkurl[0x3000] = { 0 };

	USES_CONVERSION;
	char* szBuf = (char*)GetModuleHandle(L"WeChatWin.dll") + 0X1131BEC;
	const char* cid = (const char*) * ((DWORD*)szBuf);
	wchar_t* id = A2W(cid);


	GetDlgItemText(hwndDlg, MESSAGE3, title, sizeof(title));
	GetDlgItemText(hwndDlg, MESSAGE4, content, sizeof(content));
	GetDlgItemText(hwndDlg, MESSAGE5, linkurl, sizeof(linkurl));
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		if (bo != 0) {
			ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
			wchar_t wxid[0x100] = { 0 };
			swprintf_s(wxid, L"%s", wstrText);
			SendXml(title, content, linkurl, pic, id, wxid);
			Sleep(1000);
		}
	}
	MessageBox(NULL, L"发送成功", L"aaa", 0);
}
