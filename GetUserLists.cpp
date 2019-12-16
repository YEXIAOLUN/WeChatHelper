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
#include "sendMessage.h"
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
	swprintf_s(wxid, L"%s", *((LPVOID*)wxidAdd), 0);
	OutputDebugString(LPCWSTR(wxid));

	wchar_t nick[0x100] = { 0 };
	swprintf_s(nick, L"%s", *((LPVOID*)wxNickAdd), 0);
	OutputDebugString(LPCWSTR(nick));
	wchar_t wxuserID[0x100] = { 0 };
	swprintf_s(wxuserID, L"%s", *((LPVOID*)wxuserIDAdd), 0);
	OutputDebugString(LPCWSTR(wxuserID));

	if (wcslen(wxid) == 0) {
		return;
	}
	if (wcslen(nick) == 0) {
		return;
	}
	if (wcslen(wxuserID) == 0) {
		return;
	}
	if (oldWxid[0] == 0 && newWxid[0] == 0) {
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd), 0);
	}

	if (oldWxid[0] == 0 && newWxid[0] != 0) {
		swprintf_s(oldWxid, L"%s", newWxid, 0);
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd), 0);
	}

	if (oldWxid[0] != 0 && newWxid[0] != 0) {
		swprintf_s(oldWxid, L"%s", newWxid);
		swprintf_s(newWxid, L"%s", *((LPVOID*)wxidAdd), 0);
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






	if (wcscmp(oldWxid, newWxid) != 0) {


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

VOID __declspec(naked) HookF()
{

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

	insertUserLists();
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
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartHook, NULL, 0, NULL);
	//StartHook(WinAdd + HookAdd, &HookF);
}







//�����б��ַ
#define WXADDR_DATA_BASE *((PDWORD)(getWechatWin()+0x113227C))
#define WXADDR_ALL_USERS (WXADDR_DATA_BASE+0x24+0x68) 

// �洢�ڵ��ַ������
typedef std::list< DWORD> DWORD_LIST;

// WX����������ڽṹ��
typedef struct _RAW_WXNODE
{
	union
	{
		DWORD dwStart;          // ����ʼֵ
		PDWORD pStartNode;      // ������ʼ�ڵ�
	} u1;

	DWORD dwTotal;              // �ڵ�����

} RAW_WXNODE, * PRAW_WXNODE;


//΢���ַ����ṹ��
typedef struct _WXUNICODE_STRING
{
	wchar_t* Buffer; //ָ���ַ�����ָ��
	UINT Length;   //��Ч�ַ����ĳ��ȣ��ֽ�����
	UINT MaximumLength; //�ַ�������󳤶ȣ��ֽ�����

}WXUNICODE_STRING, * PWXUNICODE_STRING;


// ��������ڵ�
void TraversalWxNode(PDWORD pWxNode, DWORD_LIST& dwListAddr, DWORD dwStart, DWORD dwTotal)
{
	// ������Բ�д������Ϊ�˰�ȫ
	if (dwListAddr.size() >= dwTotal) return;

	for (int i = 0; i < 3; i++)
	{
		DWORD dwAddr = pWxNode[i];

		// �ж��Ƿ����б��У����Ҳ���������ַ
		DWORD_LIST::iterator iter;
		iter = std::find(dwListAddr.begin(), dwListAddr.end(), dwAddr);
		if (iter == dwListAddr.end() && dwAddr != dwStart)
		{
			// �ݹ����
			dwListAddr.push_back(dwAddr);
			TraversalWxNode((PDWORD)dwAddr, dwListAddr, dwStart, dwTotal);
		}
	}
}
// ��ȡ���й��ں�
BOOL GetFriendList()
{
	ListView_DeleteAllItems(gHwndList);

	PRAW_WXNODE pFriendNodes = (PRAW_WXNODE)WXADDR_ALL_USERS;

	// �洢���к�������ڵ���׵�ַ
	DWORD_LIST listFriendAddr;

	// �������к��ѵĽڵ��ַ
	TraversalWxNode(pFriendNodes->u1.pStartNode, listFriendAddr,
		pFriendNodes->u1.dwStart, pFriendNodes->dwTotal);

	DWORD_LIST::iterator iter;
	for (iter = listFriendAddr.begin(); iter != listFriendAddr.end(); iter++)
	{
		DWORD dwAddr = *iter;
		DWORD usersListData = dwAddr + 0x28;

		PWXUNICODE_STRING pwxid = (PWXUNICODE_STRING)(usersListData + 0x8); //΢��id
		PWXUNICODE_STRING pWxNumber = (PWXUNICODE_STRING)(usersListData + 0x1c); //΢�ź�
		PWXUNICODE_STRING pWxV1 = (PWXUNICODE_STRING)(usersListData + 0x30); //wxV1
		PWXUNICODE_STRING pWxRemarks = (PWXUNICODE_STRING)(usersListData + 0x50);//��ע
		PWXUNICODE_STRING pWxName = (PWXUNICODE_STRING)(usersListData + 0x64); //΢������
		PWXUNICODE_STRING headMaxImg = (PWXUNICODE_STRING)(usersListData + 0x108);//��ͼ
		PWXUNICODE_STRING headMinImg = (PWXUNICODE_STRING)(usersListData + 0xF4);//Сͼ
		//pWxNumber->Buffer �����ָ���
		if (pwxid->Length > 0) {
			wchar_t* wcWxid = pwxid->Buffer; //΢�ź�
			wchar_t* wcWxNumber = pWxNumber->Buffer; //΢�ź�
			wchar_t* wcWxRemarks = pWxRemarks->Buffer; //΢�ź�
			wchar_t* wcpWxName = pWxName->Buffer; //΢�ź�
			OutputDebugStringW(wcWxid);
			//wstring wstrWxnumber = pWxNumber->Buffer; //΢�ź�
			string  str;
			Wchar_tToString(str, wcWxid);
			if (strstr(str.c_str(), "gh_") != NULL) {
				LVITEM item = { 0 };
				item.mask = LVIF_TEXT;

				item.iSubItem = 0;
				item.pszText = (LPWSTR)(wcWxid);
				ListView_InsertItem(gHwndList, &item);

				item.iSubItem = 1;
				item.pszText = (LPWSTR)(wcWxNumber);
				ListView_SetItem(gHwndList, &item);

				if (wcWxRemarks == NULL) {
					item.iSubItem = 2;
					item.pszText = (LPWSTR)(wcpWxName);
					ListView_SetItem(gHwndList, &item);
				}
				else {

					item.iSubItem = 2;
					item.pszText = (LPWSTR)(wcWxRemarks);
					ListView_SetItem(gHwndList, &item);

				}
			}
		}
	}
	return TRUE;

}
// ��ȡ����Ⱥ
BOOL GetFriendList2()
{
	ListView_DeleteAllItems(gHwndList);

	PRAW_WXNODE pFriendNodes = (PRAW_WXNODE)WXADDR_ALL_USERS;

	// �洢���к�������ڵ���׵�ַ
	DWORD_LIST listFriendAddr;

	// �������к��ѵĽڵ��ַ
	TraversalWxNode(pFriendNodes->u1.pStartNode, listFriendAddr,
		pFriendNodes->u1.dwStart, pFriendNodes->dwTotal);

	DWORD_LIST::iterator iter;
	for (iter = listFriendAddr.begin(); iter != listFriendAddr.end(); iter++)
	{
		DWORD dwAddr = *iter;
		DWORD usersListData = dwAddr + 0x28;

		PWXUNICODE_STRING pwxid = (PWXUNICODE_STRING)(usersListData + 0x8); //΢��id
		PWXUNICODE_STRING pWxNumber = (PWXUNICODE_STRING)(usersListData + 0x1c); //΢�ź�
		PWXUNICODE_STRING pWxV1 = (PWXUNICODE_STRING)(usersListData + 0x30); //wxV1
		PWXUNICODE_STRING pWxRemarks = (PWXUNICODE_STRING)(usersListData + 0x50);//��ע
		PWXUNICODE_STRING pWxName = (PWXUNICODE_STRING)(usersListData + 0x64); //΢������
		PWXUNICODE_STRING headMaxImg = (PWXUNICODE_STRING)(usersListData + 0x108);//��ͼ
		PWXUNICODE_STRING headMinImg = (PWXUNICODE_STRING)(usersListData + 0xF4);//Сͼ
		//pWxNumber->Buffer �����ָ���
		if (pwxid->Length > 0) {
			wchar_t* wcWxid = pwxid->Buffer; //΢�ź�
			wchar_t* wcWxNumber = pWxNumber->Buffer; //΢�ź�
			wchar_t* wcWxRemarks = pWxRemarks->Buffer; //΢�ź�
			wchar_t* wcpWxName = pWxName->Buffer; //΢�ź�
			OutputDebugStringW(wcWxid);
			//wstring wstrWxnumber = pWxNumber->Buffer; //΢�ź�
			string  str;
			Wchar_tToString(str, wcWxid);
			if (strstr(str.c_str(), "@chatroom") != NULL) {
				LVITEM item = { 0 };
				item.mask = LVIF_TEXT;

				item.iSubItem = 0;
				item.pszText = (LPWSTR)(wcWxid);
				ListView_InsertItem(gHwndList, &item);

				item.iSubItem = 1;
				item.pszText = (LPWSTR)(wcWxNumber);
				ListView_SetItem(gHwndList, &item);

				if (wcWxRemarks == NULL) {
					item.iSubItem = 2;
					item.pszText = (LPWSTR)(wcpWxName);
					ListView_SetItem(gHwndList, &item);
				}
				else {

					item.iSubItem = 2;
					item.pszText = (LPWSTR)(wcWxRemarks);
					ListView_SetItem(gHwndList, &item);

				}
			}
		}
	}
	return TRUE;

}
// ��ȡ������ϵ��
BOOL GetFriendList3()
{
	ListView_DeleteAllItems(gHwndList);

	PRAW_WXNODE pFriendNodes = (PRAW_WXNODE)WXADDR_ALL_USERS;

	// �洢���к�������ڵ���׵�ַ
	DWORD_LIST listFriendAddr;

	// �������к��ѵĽڵ��ַ
	TraversalWxNode(pFriendNodes->u1.pStartNode, listFriendAddr,
		pFriendNodes->u1.dwStart, pFriendNodes->dwTotal);

	DWORD_LIST::iterator iter;
	for (iter = listFriendAddr.begin(); iter != listFriendAddr.end(); iter++)
	{
		DWORD dwAddr = *iter;
		DWORD usersListData = dwAddr + 0x28;

		PWXUNICODE_STRING pwxid = (PWXUNICODE_STRING)(usersListData + 0x8); //΢��id
		PWXUNICODE_STRING pWxNumber = (PWXUNICODE_STRING)(usersListData + 0x1c); //΢�ź�
		PWXUNICODE_STRING pWxV1 = (PWXUNICODE_STRING)(usersListData + 0x30); //wxV1
		PWXUNICODE_STRING pWxRemarks = (PWXUNICODE_STRING)(usersListData + 0x50);//��ע
		PWXUNICODE_STRING pWxName = (PWXUNICODE_STRING)(usersListData + 0x64); //΢������
		PWXUNICODE_STRING headMaxImg = (PWXUNICODE_STRING)(usersListData + 0x108);//��ͼ
		PWXUNICODE_STRING headMinImg = (PWXUNICODE_STRING)(usersListData + 0xF4);//Сͼ
		//pWxNumber->Buffer �����ָ���
		if (pwxid->Length > 0) {


			wchar_t* wcWxid = pwxid->Buffer; //΢�ź�
			wchar_t* wcWxNumber = pWxNumber->Buffer; //΢�ź�
			wchar_t* wcWxRemarks = pWxRemarks->Buffer; //΢�ź�
			wchar_t* wcpWxName = pWxName->Buffer; //΢�ź�
			
			//wstring wstrWxnumber = pWxNumber->Buffer; //΢�ź�
			string  str;
			Wchar_tToString(str, wcWxid);
			if (strstr(str.c_str(), "@chatroom") == NULL && strstr(str.c_str(), "gh") == NULL) {
				LVITEM item = { 0 };
				item.mask = LVIF_TEXT;

				item.iSubItem = 0;
				item.pszText = (LPWSTR)(wcWxid);
				ListView_InsertItem(gHwndList, &item);

				item.iSubItem = 1;
				item.pszText = (LPWSTR)(wcWxNumber);
				ListView_SetItem(gHwndList, &item);

				if (wcWxRemarks == NULL){
					item.iSubItem = 2;
					item.pszText = (LPWSTR)(wcpWxName);
					ListView_SetItem(gHwndList, &item);
				}
				else {

					item.iSubItem = 2;
					item.pszText = (LPWSTR)(wcWxRemarks);
					ListView_SetItem(gHwndList, &item);

				}
			}
		}
	}
	return TRUE;

}

BOOL GetFriendList4(HWND hwndDlg)
{
	ListView_DeleteAllItems(gHwndList);

	PRAW_WXNODE pFriendNodes = (PRAW_WXNODE)WXADDR_ALL_USERS;

	// �洢���к�������ڵ���׵�ַ
	DWORD_LIST listFriendAddr;

	// �������к��ѵĽڵ��ַ
	TraversalWxNode(pFriendNodes->u1.pStartNode, listFriendAddr,
		pFriendNodes->u1.dwStart, pFriendNodes->dwTotal);

	DWORD_LIST::iterator iter;
	for (iter = listFriendAddr.begin(); iter != listFriendAddr.end(); iter++)
	{
		DWORD dwAddr = *iter;
		DWORD usersListData = dwAddr + 0x28;

		PWXUNICODE_STRING pwxid = (PWXUNICODE_STRING)(usersListData + 0x8); //΢��id
		PWXUNICODE_STRING pWxNumber = (PWXUNICODE_STRING)(usersListData + 0x1c); //΢�ź�
		PWXUNICODE_STRING pWxV1 = (PWXUNICODE_STRING)(usersListData + 0x30); //wxV1
		PWXUNICODE_STRING pWxRemarks = (PWXUNICODE_STRING)(usersListData + 0x50);//��ע
		PWXUNICODE_STRING pWxName = (PWXUNICODE_STRING)(usersListData + 0x64); //΢������
		PWXUNICODE_STRING headMaxImg = (PWXUNICODE_STRING)(usersListData + 0x108);//��ͼ
		PWXUNICODE_STRING headMinImg = (PWXUNICODE_STRING)(usersListData + 0xF4);//Сͼ
		//pWxNumber->Buffer �����ָ���
		wchar_t messagetemp[0x300] = { 0 };
		GetDlgItemText(hwndDlg, MESSAGE6, messagetemp, sizeof(messagetemp));
		string  temp;
		Wchar_tToString(temp, messagetemp);
		if (pwxid->Length > 0) {


			wchar_t* wcWxid = pwxid->Buffer; //΢�ź�
			wchar_t* wcWxNumber = pWxNumber->Buffer; //΢�ź�
			wchar_t* wcWxRemarks = pWxRemarks->Buffer; //΢�ź�
			wchar_t* wcpWxName = pWxName->Buffer; //΢�ź�

			string  str;
			Wchar_tToString(str, wcWxRemarks);
			string  str2;
			Wchar_tToString(str2, wcpWxName);
			int idx = -1;
			int idx2 = -1;
			idx = str.find(temp);
			idx2 = str2.find(temp);
			
			if (idx != -1 || idx2 != -1) {
				LVITEM item = { 0 };
				item.mask = LVIF_TEXT;

				item.iSubItem = 0;
				item.pszText = (LPWSTR)(wcWxid);
				ListView_InsertItem(gHwndList, &item);

				item.iSubItem = 1;
				item.pszText = (LPWSTR)(wcWxNumber);
				ListView_SetItem(gHwndList, &item);

				if (wcWxRemarks == NULL) {
					item.iSubItem = 2;
					item.pszText = (LPWSTR)(wcpWxName);
					ListView_SetItem(gHwndList, &item);
				}
				else {

					item.iSubItem = 2;
					item.pszText = (LPWSTR)(wcWxRemarks);
					ListView_SetItem(gHwndList, &item);

				}
			}
		}
	}
	return TRUE;

}