#include "pch.h"
#include "AddUser.h"
#include "resource.h"

struct wechatText2 {
	wchar_t* pStr;
	int strLen;
	int iStrLen;
	int fill1 = 0;
	int fill2 = 0;
};


struct wechatText {
	wchar_t* pStr;
	int strLen;
	int strMaxLen;
	int fill1 = 0;
	int fill2 = 0;
};
/*
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
*/
//�汾��2.6.6.28
//����ƫ�ƣ�16D827

/*********���ܣ���Ӻ���  ƫ�� ****************************/
#define WECHATADDUSERPUSH 0xe975f8
#define WECHATADDUSERCALL1 0x653c0
#define WECHATADDUSERCALL2 0x54260
#define WECHATADDUSERCALL3 0x47ae20
#define WECHATADDUSERCALL4 0x47ae60
#define WECHATADDUSERCALL5 0x4be50
#define WECHATADDUSERCALL6 0x2651c0

/*********���ܣ���Ӻ���****************************/
/*********����1��0=������� 1=Ⱥ����� 2=��Ƭ���****************************/
/*********����2���Է�WXID****************************/
/*********����3����֤��Ϣ****************************/

void addUser(DWORD dwType, wchar_t*  wxid, wchar_t* msg)
{
	DWORD dwWeChatWinAddr = (DWORD)GetModuleHandleA("WeChatWin.dll");
	DWORD dwParam1 = dwWeChatWinAddr + WECHATADDUSERPUSH;
	DWORD dwCall1 = dwWeChatWinAddr + WECHATADDUSERCALL1;
	DWORD dwCall2 = dwWeChatWinAddr + WECHATADDUSERCALL2;
	DWORD dwCall3 = dwWeChatWinAddr + WECHATADDUSERCALL3;
	DWORD dwCall4 = dwWeChatWinAddr + WECHATADDUSERCALL4;
	DWORD dwCall5 = dwWeChatWinAddr + WECHATADDUSERCALL5;
	DWORD dwCall6 = dwWeChatWinAddr + WECHATADDUSERCALL6;
	struct TextStruct
	{
		wchar_t* pStr;
		int strLen;
		int strMaxLen;
	};

	TextStruct pWxid = { 0 };
	pWxid.pStr = wxid;
	pWxid.strLen = wcslen(wxid) + 1;
	pWxid.strMaxLen = (wcslen(wxid) + 1) * 2;


	TextStruct pMsg = { 0 };
	pMsg.pStr = msg;
	pMsg.strLen = wcslen(msg) + 1;
	pMsg.strMaxLen = (wcslen(msg) + 1) * 2;

	//new char()
	//char* asmWxid = (char*)&pWxid.pStr;
	TextStruct* asmWxid = new TextStruct(pWxid);
	char* asmMsg = (char*)&pMsg.pStr;
	DWORD asmMsgText = (DWORD)pMsg.pStr;
	char buff3[0x100] = { 0 };
	char* buff = buff3;
	int nType = 0xE;
	if (dwType == 0)
	{
		nType = 0x6;
	}
	else if (dwType == 1)
	{
		nType = 0xE;
	}
	else if (dwType == 2)
	{
		nType = 0x11;
	}
	else
	{
		nType = 0xE;
	}
	const DWORD dwCType = nType;
	__asm
	{
		pushad
		sub esp, 0x18
		mov ecx, esp
		mov dword ptr ss : [ebp - 0xDC] , esp
		push  dwParam1
		call dwCall1
		sub esp, 0x18
		mov eax, buff
		//mov dword ptr ss : [ebp - 0xE4], esp
		mov ecx, esp
		push eax
		call dwCall2
		push dwCType //�����Դ
		sub esp, 0x14
		mov ecx, esp
		//mov dword ptr ss : [ebp - 0xE8], esp
		push - 0x1;
		mov edi, asmMsgText;
		push edi;
		call dwCall3;
		//mov eax, asmMsg;
		push 0x2;
		sub esp, 0x14;
		mov ecx, esp;
		//mov dword ptr ss : [ebp - 0xE0], esp;
		mov ebx, asmWxid;
		push ebx;
		call dwCall4;
		call dwCall5;
		mov ecx, eax;
		call dwCall6;
		popad
	}
	delete asmWxid;
}

VOID addUserfront(wchar_t * chatid1, wchar_t *wxid1) {

	char* chatid = (char*)chatid1;

	wechatText2 wxid = { 0 };

	wxid.pStr = wxid1;

	wxid.strLen = wcslen(wxid1);

	wxid.iStrLen = wcslen(wxid1) * 2;

	wchar_t datatbuffer[0xD0] = { 0 };
	DWORD* dwDatabuf = (DWORD*)&datatbuffer;
	dwDatabuf[0] = (DWORD)&wxid.pStr;
	dwDatabuf[1] = dwDatabuf[0] + 0x14;
	dwDatabuf[2] = dwDatabuf[0] + 0x14;


	DWORD wechatWinBaseAddress = (DWORD)GetModuleHandleA("WeChatWin.dll");


	DWORD call1 = wechatWinBaseAddress + 0x47B070;
	DWORD call2 = wechatWinBaseAddress + 0x80E10;
	DWORD call3 = wechatWinBaseAddress + 0x265990;

	DWORD call5 = wechatWinBaseAddress + 0x113224C;


	call5 = *((DWORD*)call5);

	;

	__asm {
		pushad

		sub esp, 0x14
		mov ecx, esp
		push - 0x1
		mov dword ptr ds : [ecx] , 0x0
		mov dword ptr ds : [ecx + 0x4] , 0x0
		mov dword ptr ds : [ecx + 0x8] , 0x0
		mov dword ptr ds : [ecx + 0xC] , 0x0
		mov dword ptr ds : [ecx + 0x10] , 0x0
		push chatid
		call call1
		sub esp, 0xC
		lea eax, datatbuffer
		mov ecx, esp
		push eax
		call call2
		mov eax, call5
		mov ecx, eax
		call call3
		popad
	}



}

VOID Add8(HWND hwndDlg) {     //���ָ������ϵ��
	wchar_t chatid1[0x300] = { 0 };
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = -1;
	wchar_t wxid[0x100] = { 0 };
	int sums = ListView_GetItemCount(hiew);
	for (int i = 0; i < sums; i++) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		if (bo != 0) {
			ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
			swprintf_s(chatid1, L"%s", wstrText);
		}
	}
	wchar_t messagetemp[0x300] = { 0 };
	wchar_t wxid1[0x300] = { 0 };
	HWND hiew2 = GetDlgItem(hwndDlg, USER_LISTS2);
	TCHAR wstrText2[4][128] = { 0 };
	int pos2 = -1;
	int sums2 = ListView_GetItemCount(hiew2);
	for (int i = 0; i < sums2; i++) {
		pos2 = ListView_GetNextItem(hiew2, pos2, LVNI_ALL);
		ListView_GetItemText(hiew2, pos2, 0, wstrText2[0], sizeof(wstrText2[0]));
		swprintf_s(wxid1, L"%s", wstrText2);
		addUserfront(chatid1, wxid1);
		Sleep(5000);
		GetDlgItemText(hwndDlg, MESSAGE2, messagetemp, sizeof(messagetemp));
		//wchar_t strsum[0x300] = { 0 };
		//swprintf_s(strsum, L"chatroom:%s wxid:%s message:%s", chatid1, wxid1, messagetemp);
		//MessageBox(NULL, strsum, L"aaa", 0);
		addUser(1, wxid1, messagetemp);

	}
	MessageBox(NULL, L"��Ӻ��ѷ��ͳɹ�", L"����", 0);
}

VOID Add7(HWND hwndDlg) {     //���������ϵ��
	wchar_t chatid1[0x300] = { 0 };
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = -1;
	wchar_t wxid[0x100] = { 0 };
	int sums = ListView_GetItemCount(hiew);
	for (int i = 0; i < sums; i++) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		if (bo != 0) {
			ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
			swprintf_s(chatid1, L"%s", wstrText);
		}
	}
	wchar_t messagetemp[0x300] = { 0 };
	wchar_t wxid1[0x300] = { 0 };
	HWND hiew2 = GetDlgItem(hwndDlg, USER_LISTS2);
	TCHAR wstrText2[4][128] = { 0 };
	int pos2 = -1;
	int sums2 = ListView_GetItemCount(hiew2);
	for (int i = 0; i < sums2; i++) {
		pos2 = ListView_GetNextItem(hiew2, pos2, LVNI_ALL);
		int bo2 = ListView_GetItemState(hiew2, pos2, LVIS_SELECTED);
		if (bo2 != 0) {
			ListView_GetItemText(hiew2, pos2, 0, wstrText2[0], sizeof(wstrText2[0]));
			swprintf_s(wxid1, L"%s", wstrText2);
			addUserfront(chatid1, wxid1);
			Sleep(10000);
			GetDlgItemText(hwndDlg, MESSAGE2, messagetemp, sizeof(messagetemp));
			//wchar_t strsum[0x300] = { 0 };
			//swprintf_s(strsum, L"chatroom:%s wxid:%s message:%s", chatid1, wxid1, messagetemp);
			//MessageBox(NULL, strsum, L"aaa", 0);
			
			addUser(1,wxid1, messagetemp);
		}
	}
	MessageBox(NULL, L"��Ӻ��ѷ��ͳɹ�", L"aaa", 0);

}