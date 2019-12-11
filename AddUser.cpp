#include "pch.h"
#include "AddUser.h"
#include "resource.h"

struct wechatText {
	wchar_t* pStr;
	int strLen;
	int strMaxLen;
	int fill1 = 0;
	int fill2 = 0;
};

VOID addUser(wchar_t* wxid, wchar_t* message) {
	DWORD base = (DWORD)GetModuleHandleA("WeChatWin.dll");
	DWORD callAdd = base + 0x47AE60;
	DWORD callAdd2 = base + 0x4BE50;
	DWORD callAdd3 = base + 0x2651C0;
	DWORD callAdd4 = base + 0x47AE20;
	DWORD callAdd5 = base + 0xE975F8;
	DWORD callAdd6 = base + 0x653C0;
	DWORD callAdd7 = base + 0x54260;
	wechatText pwxid2 = { 0 };
	pwxid2.pStr = wxid;
	pwxid2.strLen = wcslen(wxid);
	pwxid2.strMaxLen = wcslen(wxid) * 2;

	wechatText msg = { 0 };
	msg.pStr = message;
	msg.strLen = wcslen(message);
	msg.strMaxLen = wcslen(message) * 2;

	char* asmWxid = (char*)&pwxid2.pStr;
	char* asmMsg = (char*)&msg.pStr;
	DWORD asmMsgtext = (DWORD)msg.pStr;
	char buff3[0x100] = { 0 };
	char* buff = buff3;
	__asm {

		sub esp, 0x18
		mov ecx, esp
		mov dword ptr ss : [ebp - 0xDC] , esp
		push callAdd5
		call callAdd6
		sub esp, 0x18
		mov eax, buff
		mov dword ptr ss : [ebp - 0xE4] , esp
		mov ecx, esp
		push eax
		call callAdd7
		push 0x6
		sub esp, 0x14
		mov ecx, esp
		mov dword ptr ss : [ebp - 0xE8] , esp
		push - 0x1
		mov edi, asmMsgtext
		push edi
		call callAdd4
		mov eax, asmMsg
		push 0x2
		sub esp, 0x14
		mov ecx, esp
		mov dword ptr ss : [ebp - 0xE0] , esp
		mov ebx, asmWxid
		push ebx
		call callAdd
		call callAdd2
		mov ecx, eax
		call callAdd3
	}
}
VOID Add7(HWND hwndDlg) {     //添加指定的联系人
	wchar_t messagetemp[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS2);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	wchar_t wxid[0x100] = { 0 };
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		if (bo != 0) {
			ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
			wchar_t wxid[0x100] = { 0 };
			swprintf_s(wxid, L"%s", wstrText);
			GetDlgItemText(hwndDlg, MESSAGE2, messagetemp, sizeof(messagetemp));
			//MessageBox(NULL, wxid, L"标题", 0);
			addUser(wxid, messagetemp);
			Sleep(5000);
		}
			
	}
	MessageBox(NULL, L"添加好友发送成功", L"标题", 0);
}

VOID Add8(HWND hwndDlg) {     //添加所有联系人
	wchar_t messagetemp[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS2);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		GetDlgItemText(hwndDlg, MESSAGE2, messagetemp, sizeof(messagetemp));
		addUser(wxid, messagetemp);
		Sleep(5000);
	}
	MessageBox(NULL, L"添加好友发送成功", L"aaa", 0);
}