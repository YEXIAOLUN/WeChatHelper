#include "pch.h"
#include <Windows.h>
#include <stdio.h>
#include "commctrl.h"
#include <TlHelp32.h>
#include <stdlib.h>
#include <conio.h>
#include <locale.h>
#include <map>

#include <tuple>
#include <string>
#include <iostream>
#include "resource.h"
//#include "sendmessage.h"

#pragma comment(lib, "Version.lib")
#define HOOK_LEN 5
BYTE backCode[HOOK_LEN] = { 0 };
HWND hDlg = 0;
DWORD hookData = 0;
DWORD WinAdd = 0;
DWORD retCallAdd = 0;
DWORD retAdd = 0;
HWND gHwndList = 0;
DWORD r_esi = 0;
DWORD overWritedCallAdd;
wchar_t oldWxid[0x100] = { 0 };
wchar_t newWxid[0x100] = { 0 };


DWORD hookAdd;
LPVOID jmpAdd;

DWORD cEax = 0;
DWORD cEcx = 0;
DWORD cEdx = 0;
DWORD cEbx = 0;
DWORD cEsp = 0;
DWORD cEbp = 0;
DWORD cEsi = 0;
DWORD cEdi = 0;

using namespace std;

typedef tuple <
	//wxid1
	wstring,
	//wxName
	wstring,
	//nickName
	wstring
> USER_INFO;

//����7000���û��б�


list<USER_INFO> userInfoList(1);



//��ȡģ���ַ
DWORD getWechatWin()
{
	return (DWORD)LoadLibrary(L"WeChatWin.dll");
}
char* wchar2char(const wchar_t* wchar)
{
	char* m_char;
	int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
	m_char = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
	m_char[len] = '\0';
	return m_char;
}

//��ʼhook
/**
 * ����һ hookAdd ��Ҫhook�ĵ�ַ
 * ������ jmpAdd hook���ȥ�ĵ�ַ
 * ������ oldBye ���ݱ�hook��ַ���Ƕζ��������� ���ڻָ�hook
**/
VOID StartHook()
{
	BYTE JmpCode[HOOK_LEN] = { 0 };
	//������Ҫ���һ������������
	// E9 11051111(��������ת�ĵط�����ط�����һ�������ַ ���Ǹ���hook��ַ����ת�Ĵ����ַ�ľ�����������)
	JmpCode[0] = 0xE9;
	//������ת�ľ��빫ʽ�ǹ̶���
	//���㹫ʽΪ ��ת�ĵ�ַ(Ҳ�������Ǻ����ĵ�ַ) - hook�ĵ�ַ - hook���ֽڳ���
	*(DWORD*)&JmpCode[1] = (DWORD)jmpAdd - hookAdd - HOOK_LEN;

	//hook�ڶ��� �ȱ��ݽ�Ҫ�����Ǹ��ǵ�ַ������ ����Ϊ����hook�ĳ��� HOOK_LEN 5���ֽ�

	//��ȡ���̾��
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());


	//��������
	if (ReadProcessMemory(hWHND, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook��ַ�����ݶ�ȡʧ��", L"��ȡʧ��", MB_OK);
		return;
	}
	//������hook��ʼ�� ������Ҫ�滻�ĺ�����ַд��ȥ ����ֱ���������Ǻ�������ȥȻ�����Ǵ�����Ϻ��ٷ��аɣ�
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hookд��ʧ�ܣ������滻ʧ��", L"����", MB_OK);
		return;
	}

}

CHAR* UnicodeToUTF8(const WCHAR* wideStr)
{
	char* utf8Str = NULL;
	int charLen = -1;
	charLen = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
	utf8Str = (char*)malloc(charLen);
	WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, utf8Str, charLen, NULL, NULL);
	return utf8Str;
}


// wchar_t to string
void Wchar_tToString2(std::string& szDst, wchar_t* wchar)
{
	wchar_t* wText = wchar;
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte������
	char* psText; // psTextΪchar*����ʱ���飬��Ϊ��ֵ��std::string���м����
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte���ٴ�����
	szDst = psText;// std::string��ֵ
	delete[]psText;// psText�����
}

//��ʾ�����б�
VOID insertUserLists()
{
	//MessageBox(NULL, L"hook�б�", L"����", 0);
	/*
	eax + 0x10 wxid Ⱥ
	eax + 0x30 wxid Ⱥ
	eax + 0x44 ΢�ź�
	eax + 0x58 V1����
	eax + 0x8C �ǳ�
	eax + 0x11C Сͷ��
	eax + 0x130 ��ͷ��
	eax + 0x144 δ֪md5����
	eax + 0x1C8 ����
	eax + 0x1DC ʡ��
	eax + 0x1F0 ����wxidV1Add
	eax + 0x204 �����Դ
	eax + 0x294 ����Ȧ��ֽ
	*/

	DWORD wxidAdd = cEax + 0x10;     //wxid
	DWORD wxuserIDAdd = cEax + 0x44; //΢�ź�
	DWORD wxidV1Add = cEax + 0x58;   //v1����
	DWORD wxNickAdd = cEax + 0x8C;   //�ǳ�
	//DWORD headPicAdd = cEax + 0x11C; //Сͷ��

	wchar_t wxid[0x100] = { 0 };
	swprintf_s(wxid, L"%s", *((LPVOID*)wxidAdd),0);

	wchar_t nick[0x100] = { 0 };
	swprintf_s(nick, L"%s", *((LPVOID*)wxNickAdd),0);

	wchar_t wxuserID[0x100] = { 0 };
	swprintf_s(wxuserID, L"%s", *((LPVOID*)wxuserIDAdd),0);

	
	if (wcslen(wxid) == 0 ) {
		return;
	}
	if (wcslen(nick) == 0) {
		return;
	}
	if (wcslen(wxuserID) == 0) {
		return;
	}
	if (oldWxid[0] == 0 && newWxid[0] == 0) {
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd),0);
	}

	if (oldWxid[0] == 0 && newWxid[0] != 0) {
		swprintf_s(oldWxid, L"%s", newWxid,0);
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd),0);
	}

	if (oldWxid[0] != 0 && newWxid[0] != 0) {
		swprintf_s(oldWxid, L"%s", newWxid);
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd),0);
	}
	USER_INFO userInfo(wxid, wxuserID, nick);

	
	
	for (auto& userInfoOld : userInfoList)
	{
		wstring wxid1 = get<0>(userInfoOld);
		if (wxid1 == wxid)
		{
			return;
		}
	}
	
	
	string str;
	Wchar_tToString2(str, wxid);
	
	
	



	if (wcscmp(oldWxid, newWxid) != 0 ) {

		
		if (wcslen(wxid) == 0) {
			return;
		}
		if (wcslen(nick) == 0) {
			return;
		}
		if (wcslen(wxuserID) == 0) {
			return;
		}

		userInfoList.push_front(userInfo);


		
		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;

		item.iSubItem = 0;
		item.pszText = (LPWSTR)(wxid);
		ListView_InsertItem(gHwndList, &item);

		item.iSubItem = 1;
		item.pszText = (LPWSTR)(wxuserID);
		ListView_SetItem(gHwndList, &item);

		item.iSubItem = 2;
		item.pszText = (LPWSTR)(nick);
		ListView_SetItem(gHwndList, &item);

		
	}

}
/*
VOID AddUerList()
{
	for (auto& user : userInfoList)
	{


		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;
			
		item.iSubItem = 0;
		auto a = get<0>(user);
		item.pszText = (LPWSTR)a;
		ListView_InsertItem(gHwndList, &item);

		item.iSubItem = 1;
		auto b = get<1>(user);
		item.pszText = (LPWSTR)(b);
		ListView_SetItem(gHwndList, &item);

		item.iSubItem = 2;
		auto c = get<2>(user);
		item.pszText = (LPWSTR)(c);
		ListView_SetItem(gHwndList, &item);

	}
	MessageBox(NULL, L"��ϵ���б��ȡ�ɹ�", L"aaa", 0);
}
*/

//��ת�����ĺ��� �����Լ���
/*
5BBB989D  |.  E8 5EE8FFFF   call WeChatWi.5BBB8100   418100                           ;  ����Ҳ���Ի�ȡ����
5BBB98A2  |.  84C0          test al,al                                                ;  edi������
51280
*/
VOID __declspec(naked) HookF()
{
	//pushad: �����е�32λͨ�üĴ���ѹ���ջ
	//pushfd:Ȼ��32λ��־�Ĵ���EFLAGSѹ���ջ
	//popad:�����е�32λͨ�üĴ���ȡ����ջ
	//popfd:��32λ��־�Ĵ���EFLAGSȡ����ջ
	//�ȱ���Ĵ���
	// ʹ��pushad��Щ���㻹�ǲ�̫�ȶ�  �����ñ����ѼĴ�����ֵ�������� �����ɿ���
	/*
	__asm {
		call retCallAdd
		mov cEax, eax
		mov cEcx, ecx
		mov cEdx, edx
		mov cEbx, ebx
		mov cEsp, esp
		mov cEbp, ebp
		mov cEsi, esi
		mov cEdi, edi
	}
	*/
	
	
	__asm {
		call retCallAdd
		mov cEax, eax
		mov cEcx, ecx
		mov cEdx, edx
		mov cEbx, ebx
		mov cEsp, esp
		mov cEbp, ebp
		mov cEsi, esi
		mov cEdi, edi
		//pushad
		//pushf
	}
	
	//Ȼ����ת�������Լ��Ĵ����� ��������
	insertUserLists();
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)insertUserLists, NULL, 0, NULL);
	//Ȼ���ڻ�ԭ������֮ǰ����������
	/*popad
		popfd  ��̫�ɿ��ָ���ȫ ���Բ��б����ķ�ʽ���������ٸ�ֵ��ȥ*/
		/*
		mov eax, cEax
			mov ecx, cEcx
			mov edx, cEdx
			mov ebx, cEbx
			mov esp, cEsp
			mov ebp, cEbp
			mov esi, cEsi
			mov edi, cEdi
		*/
	
	__asm {
		//popf
		//popad

		mov eax, cEax
		mov ecx, cEcx
		mov edx, cEdx
		mov ebx, cEbx
		mov esp, cEsp
		mov ebp, cEbp
		mov esi, cEsi
		mov edi, cEdi

		//call overWritedCallAdd
		jmp retAdd

	}
	
	/*
	__asm
	{
		mov eax, cEax
		mov ecx, cEcx
		mov edx, cEdx
		mov ebx, cEbx
		mov esp, cEsp
		mov ebp, cEbp
		mov esi, cEsi
		mov edi, cEdi

		//���䱻���ǵĴ���
		call overWritedCallAdd

		//���ر�HOOKָ�����һ��ָ��
		jmp retAdd
	}
	*/
	MessageBox(NULL, L"again", L"����", 0);
}

//41989D
//418154
VOID HookWechatQrcode(HWND hwndDlg, HWND hwndList, DWORD HookAdd)
{
	gHwndList = hwndList;
	WinAdd = getWechatWin();
	hDlg = hwndDlg;
	retCallAdd = WinAdd + 0x528A0;
	//retCallAdd = WinAdd + 0x5A8A0;   //�°�΢��
	retAdd = WinAdd + 0x420999;
	//retAdd = WinAdd + 0x441899;      //�°�΢��
	

	hookAdd = WinAdd + HookAdd;
	jmpAdd = &HookF;
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartHook, NULL, 0, NULL);
	//StartHook(WinAdd + HookAdd, &HookF);
}	

