#include "pch.h"
#include "sendmessage.h"
#include "resource.h"
using namespace std;



struct wxStr
{
	//���͵��ı���Ϣָ��
	wchar_t* pStr;
	//�ַ�������
	DWORD strLen;
	//�ַ�����󳤶�
	DWORD strLen2;

	//��������ռλ����
	DWORD fill1 = 0;
	DWORD fill2 = 0;
};


VOID Wchar_tToString(std::string& szDst, wchar_t* wchar);  //�ַ�ת�ͷ���



VOID SendWXIDTextMessage(wchar_t* wxid, wchar_t* message) {
	//������ϢCALL

	DWORD base = (DWORD)LoadLibrary(L"WeChatWin.dll");

	DWORD  sendCall = base + 0x2DA0F0;
	//DWORD  sendCall = GetWeChatWin() + 0x2FA7F0;    //�°�΢��

	//��װ��Ҫ�����ݸ�ʽ
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
	//����ar buff[0x87C] = { 0 };    //�°汾΢�ŷ�����Ϣ������
	char buff[0x81C] = { 0 };
	wchar_t aaa[0x100] = { 0 };
	swprintf_s(aaa, L" wxid = %s message = %s", asmWxid, asmMessage);
	//MessageBox(NULL, aaa, L"aaa", 0);
	OutputDebugString(LPCWSTR(asmWxid));


	OutputDebugString(LPCWSTR(asmMessage));

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

}



/*
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
	MessageBox(NULL, L"���ͳɹ�", L"aaa", 0);
}   //���ںŷ���
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
	MessageBox(NULL, L"���ͳɹ�", L"aaa", 0);
}	//Ⱥ����
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
	MessageBox(NULL, L"���ͳɹ�", L"aaa", 0);
}
*/
//����ָ������ϵ��
VOID Smessage5(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = -1;
	int sums = ListView_GetItemCount(hiew);
	for(int i=0;i<sums;i++){
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
	MessageBox(NULL, L"���ͳɹ�", L"aaa", 0);
}
//����������ϵ��
VOID Smessage6(HWND hwndDlg) {
	wchar_t messagetemp[0x300] = { 0 };
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlg, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int sums = ListView_GetItemCount(hiew);

	int pos = -1;
	for(int i=0;i<sums;i++){
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
		wchar_t wxid[0x100] = { 0 };
		swprintf_s(wxid, L"%s", wstrText);
		GetDlgItemText(hwndDlg, MESSAGE, messagetemp, sizeof(messagetemp));
		SendWXIDTextMessage(wxid, messagetemp);
		Sleep(1000);

	}
	MessageBox(NULL, L"���ͳɹ�", L"aaa", 0);
}
void Wchar_tToString(std::string& szDst, wchar_t* wchar)
{
	wchar_t* wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte������
	char* psText; // psTextΪchar*����ʱ���飬��Ϊ��ֵ��std::string���м����
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte���ٴ�����
	szDst = psText;// std::string��ֵ
	delete[]psText;// psText�����
}