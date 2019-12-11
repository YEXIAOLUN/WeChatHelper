#include "pch.h"
#include "sendmessage.h"
#include "resource.h"
using namespace std;



struct wxStr
{
	//发送的文本消息指针
	wchar_t* pStr;
	//字符串长度
	DWORD strLen;
	//字符串最大长度
	DWORD strLen2;

	//补充两个占位数据
	DWORD fill1 = 0;
	DWORD fill2 = 0;
};


VOID Wchar_tToString(std::string& szDst, wchar_t* wchar);  //字符转型方法



VOID SendWXIDTextMessage(wchar_t* wxid, wchar_t* message) {
	//发送消息CALL

	DWORD base = (DWORD)LoadLibrary(L"WeChatWin.dll");

	DWORD  sendCall = base + 0x2DA0F0;
	//DWORD  sendCall = GetWeChatWin() + 0x2FA7F0;    //新版微信

	//组装需要的数据格式
	wxStr pWxid = { 0 };
	pWxid.pStr = wxid;
	pWxid.strLen = wcslen(wxid);
	pWxid.strLen2 = wcslen(wxid) * 2;

	wxStr pMessage = { 0 };
	pMessage.pStr = message;
	pMessage.strLen = wcslen(message);
	pMessage.strLen2 = wcslen(message) * 2;

	char* asmWxid = (char*)&pWxid.pStr;
	char* asmMessage = (char*)&pMessage.pStr;
	//擦汗ar buff[0x87C] = { 0 };    //新版本微信发送消息缓冲区
	char buff[0x81C] = { 0 };
	wchar_t aaa[0x100] = { 0 };
	swprintf_s(aaa, L" wxid = %s message = %s", asmWxid, asmMessage);
	//MessageBox(NULL, aaa, L"aaa", 0);
	OutputDebugString(LPCWSTR(asmWxid));


	OutputDebugString(LPCWSTR(asmMessage));
	/*
	__asm {
		mov edx, asmWxid

		//传递参数
		push 0x1

		mov eax, 0x0
		push eax

		//微信消息内容
		mov ebx, asmMessage
		push ebx

		lea ecx, buff

		//调用函数
		call sendCall

		//平衡堆栈
		add esp, 0xC
	}
	*/

	__asm {
		mov edx, asmWxid
		mov eax, 0x0
		push 0x1
		push eax
		mov ebx, asmMessage
		push ebx
		lea ecx, buff
		call sendCall
		add esp, 0xC
	}


	//MessageBox(NULL, aaa, L"aaa", 0);
	/*
	try {
		__asm {
			mov edx, asmWxid;
			push 0x1;
			push 0;
			mov ebx, asmMessage;
			push ebx;
			lea ecx, buff;
			call sendCall;
			add esp, 0xC;
		}
	}
	catch(wchar_t s){
		wchar_t yi[0x100] = { 0 };
		swprintf_s(yi, L" 异常：%s", s);
		MessageBox(NULL, yi, L"aaa", 0);
	}
	*/
	/*
	__asm
	{
		mov edx, asmWxid
		lea eax, buff
		mov ebx, asmMessage
		lea ecx, buff

		push 0x1
		push eax
		push ebx
		call sendCall
		add esp, 0xC

		//mov returnValue,eax
	}
	*/
}




VOID Smessage2(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "gh_") != NULL) {
			GetDlgItemText(hwndDlg, MESSAGE, messagetemp, sizeof(messagetemp));
			SendWXIDTextMessage(wxid, messagetemp);

			wchar_t wxid1[0x100] = { 0 };
			swprintf_s(wxid1, L"%s,%d", wstrText, pos);
			//MessageBox(NULL, wxid1, L"aaa", 0);
			Sleep(1000);
		}
	}
	MessageBox(NULL, L"发送成功", L"aaa", 0);
}   //公众号发送
VOID Smessage3(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "@chatroom") != NULL) {
			GetDlgItemText(hwndDlg, MESSAGE, messagetemp, sizeof(messagetemp));
			SendWXIDTextMessage(wxid, messagetemp);
			Sleep(1000);
		}


	}
	MessageBox(NULL, L"发送成功", L"aaa", 0);
}	//群发送
VOID Smessage4(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		string  str;
		Wchar_tToString(str, wxid);
		if (strstr(str.c_str(), "@chatroom") == NULL && strstr(str.c_str(), "gh") == NULL) {
			GetDlgItemText(hwndDlg, MESSAGE, messagetemp, sizeof(messagetemp));
			SendWXIDTextMessage(wxid, messagetemp);
			Sleep(1000);
		}


	}
	MessageBox(NULL, L"发送成功", L"aaa", 0);
}
VOID Smessage5(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		if (bo != 0) {
			ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
			wchar_t wxid[0x100] = { 0 };
			swprintf_s(wxid, L"%s", wstrText);
			GetDlgItemText(hwndDlg, MESSAGE, messagetemp, sizeof(messagetemp));
			SendWXIDTextMessage(wxid, messagetemp);
			Sleep(1000);
		}

	}
	MessageBox(NULL, L"发送成功", L"aaa", 0);
}
void Wchar_tToString(std::string& szDst, wchar_t* wchar)
{
	wchar_t* wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
	char* psText; // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
	szDst = psText;// std::string赋值
	delete[]psText;// psText的清除
}