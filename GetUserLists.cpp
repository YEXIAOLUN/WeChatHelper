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

//定义7000个用户列表


list<USER_INFO> userInfoList(1);



//获取模块基址
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

//开始hook
/**
 * 参数一 hookAdd 想要hook的地址
 * 参数二 jmpAdd hook完回去的地址
 * 参数三 oldBye 备份被hook地址的那段二进制数据 用于恢复hook
**/
VOID StartHook()
{
	BYTE JmpCode[HOOK_LEN] = { 0 };
	//我们需要组成一段这样的数据
	// E9 11051111(这里是跳转的地方这个地方不是一个代码地址 而是根据hook地址和跳转的代码地址的距离计算出来的)
	JmpCode[0] = 0xE9;
	//计算跳转的距离公式是固定的
	//计算公式为 跳转的地址(也就是我们函数的地址) - hook的地址 - hook的字节长度
	*(DWORD*)&JmpCode[1] = (DWORD)jmpAdd - hookAdd - HOOK_LEN;

	//hook第二步 先备份将要被我们覆盖地址的数据 长度为我们hook的长度 HOOK_LEN 5个字节

	//获取进程句柄
	HANDLE hWHND = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());


	//备份数据
	if (ReadProcessMemory(hWHND, (LPVOID)hookAdd, backCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook地址的数据读取失败", L"读取失败", MB_OK);
		return;
	}
	//真正的hook开始了 把我们要替换的函数地址写进去 让他直接跳到我们函数里面去然后我们处理完毕后再放行吧！
	if (WriteProcessMemory(hWHND, (LPVOID)hookAdd, JmpCode, HOOK_LEN, NULL) == 0) {
		MessageBox(NULL, L"hook写入失败，函数替换失败", L"错误", MB_OK);
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
	DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte的运用
	char* psText; // psText为char*的临时数组，作为赋值给std::string的中间变量
	psText = new char[dwNum];
	WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte的再次运用
	szDst = psText;// std::string赋值
	delete[]psText;// psText的清除
}

//显示好友列表
VOID insertUserLists()
{
	//MessageBox(NULL, L"hook列表", L"标题", 0);
	/*
	eax + 0x10 wxid 群
	eax + 0x30 wxid 群
	eax + 0x44 微信号
	eax + 0x58 V1数据
	eax + 0x8C 昵称
	eax + 0x11C 小头像
	eax + 0x130 大头像
	eax + 0x144 未知md5数据
	eax + 0x1C8 国籍
	eax + 0x1DC 省份
	eax + 0x1F0 城市wxidV1Add
	eax + 0x204 添加来源
	eax + 0x294 朋友圈壁纸
	*/

	DWORD wxidAdd = cEax + 0x10;     //wxid
	DWORD wxuserIDAdd = cEax + 0x44; //微信号
	DWORD wxidV1Add = cEax + 0x58;   //v1数据
	DWORD wxNickAdd = cEax + 0x8C;   //昵称
	//DWORD headPicAdd = cEax + 0x11C; //小头像

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
	MessageBox(NULL, L"again", L"标题", 0);
}

//41989D
//418154
VOID HookWechatQrcode(HWND hwndDlg, HWND hwndList, DWORD HookAdd)
{
	gHwndList = hwndList;
	WinAdd = getWechatWin();
	hDlg = hwndDlg;
	retCallAdd = WinAdd + 0x528A0;
	//retCallAdd = WinAdd + 0x5A8A0;   //新版微信
	retAdd = WinAdd + 0x420999;
	//retAdd = WinAdd + 0x441899;      //新版微信


	hookAdd = WinAdd + HookAdd;
	jmpAdd = &HookF;
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartHook, NULL, 0, NULL);
	//StartHook(WinAdd + HookAdd, &HookF);
}







//好友列表基址
#define WXADDR_DATA_BASE *((PDWORD)(getWechatWin()+0x113227C))
#define WXADDR_ALL_USERS (WXADDR_DATA_BASE+0x24+0x68) 

// 存储节点地址的类型
typedef std::list< DWORD> DWORD_LIST;

// WX数据链表入口结构体
typedef struct _RAW_WXNODE
{
	union
	{
		DWORD dwStart;          // 是起始值
		PDWORD pStartNode;      // 又是起始节点
	} u1;

	DWORD dwTotal;              // 节点总数

} RAW_WXNODE, * PRAW_WXNODE;


//微信字符串结构体
typedef struct _WXUNICODE_STRING
{
	wchar_t* Buffer; //指向字符串的指针
	UINT Length;   //有效字符串的长度（字节数）
	UINT MaximumLength; //字符串的最大长度（字节数）

}WXUNICODE_STRING, * PWXUNICODE_STRING;


// 遍历链表节点
void TraversalWxNode(PDWORD pWxNode, DWORD_LIST& dwListAddr, DWORD dwStart, DWORD dwTotal)
{
	// 这个可以不写，但是为了安全
	if (dwListAddr.size() >= dwTotal) return;

	for (int i = 0; i < 3; i++)
	{
		DWORD dwAddr = pWxNode[i];

		// 判断是否在列表中，并且不等于起点地址
		DWORD_LIST::iterator iter;
		iter = std::find(dwListAddr.begin(), dwListAddr.end(), dwAddr);
		if (iter == dwListAddr.end() && dwAddr != dwStart)
		{
			// 递归查找
			dwListAddr.push_back(dwAddr);
			TraversalWxNode((PDWORD)dwAddr, dwListAddr, dwStart, dwTotal);
		}
	}
}
// 获取所有公众号
BOOL GetFriendList()
{
	ListView_DeleteAllItems(gHwndList);

	PRAW_WXNODE pFriendNodes = (PRAW_WXNODE)WXADDR_ALL_USERS;

	// 存储所有好友链表节点的首地址
	DWORD_LIST listFriendAddr;

	// 遍历所有好友的节点地址
	TraversalWxNode(pFriendNodes->u1.pStartNode, listFriendAddr,
		pFriendNodes->u1.dwStart, pFriendNodes->dwTotal);

	DWORD_LIST::iterator iter;
	for (iter = listFriendAddr.begin(); iter != listFriendAddr.end(); iter++)
	{
		DWORD dwAddr = *iter;
		DWORD usersListData = dwAddr + 0x28;

		PWXUNICODE_STRING pwxid = (PWXUNICODE_STRING)(usersListData + 0x8); //微信id
		PWXUNICODE_STRING pWxNumber = (PWXUNICODE_STRING)(usersListData + 0x1c); //微信号
		PWXUNICODE_STRING pWxV1 = (PWXUNICODE_STRING)(usersListData + 0x30); //wxV1
		PWXUNICODE_STRING pWxRemarks = (PWXUNICODE_STRING)(usersListData + 0x50);//备注
		PWXUNICODE_STRING pWxName = (PWXUNICODE_STRING)(usersListData + 0x64); //微信名称
		PWXUNICODE_STRING headMaxImg = (PWXUNICODE_STRING)(usersListData + 0x108);//大图
		PWXUNICODE_STRING headMinImg = (PWXUNICODE_STRING)(usersListData + 0xF4);//小图
		//pWxNumber->Buffer 就是字付串
		if (pwxid->Length > 0) {
			wchar_t* wcWxid = pwxid->Buffer; //微信号
			wchar_t* wcWxNumber = pWxNumber->Buffer; //微信号
			wchar_t* wcWxRemarks = pWxRemarks->Buffer; //微信号
			wchar_t* wcpWxName = pWxName->Buffer; //微信号
			OutputDebugStringW(wcWxid);
			//wstring wstrWxnumber = pWxNumber->Buffer; //微信号
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
// 获取所有群
BOOL GetFriendList2()
{
	ListView_DeleteAllItems(gHwndList);

	PRAW_WXNODE pFriendNodes = (PRAW_WXNODE)WXADDR_ALL_USERS;

	// 存储所有好友链表节点的首地址
	DWORD_LIST listFriendAddr;

	// 遍历所有好友的节点地址
	TraversalWxNode(pFriendNodes->u1.pStartNode, listFriendAddr,
		pFriendNodes->u1.dwStart, pFriendNodes->dwTotal);

	DWORD_LIST::iterator iter;
	for (iter = listFriendAddr.begin(); iter != listFriendAddr.end(); iter++)
	{
		DWORD dwAddr = *iter;
		DWORD usersListData = dwAddr + 0x28;

		PWXUNICODE_STRING pwxid = (PWXUNICODE_STRING)(usersListData + 0x8); //微信id
		PWXUNICODE_STRING pWxNumber = (PWXUNICODE_STRING)(usersListData + 0x1c); //微信号
		PWXUNICODE_STRING pWxV1 = (PWXUNICODE_STRING)(usersListData + 0x30); //wxV1
		PWXUNICODE_STRING pWxRemarks = (PWXUNICODE_STRING)(usersListData + 0x50);//备注
		PWXUNICODE_STRING pWxName = (PWXUNICODE_STRING)(usersListData + 0x64); //微信名称
		PWXUNICODE_STRING headMaxImg = (PWXUNICODE_STRING)(usersListData + 0x108);//大图
		PWXUNICODE_STRING headMinImg = (PWXUNICODE_STRING)(usersListData + 0xF4);//小图
		//pWxNumber->Buffer 就是字付串
		if (pwxid->Length > 0) {
			wchar_t* wcWxid = pwxid->Buffer; //微信号
			wchar_t* wcWxNumber = pWxNumber->Buffer; //微信号
			wchar_t* wcWxRemarks = pWxRemarks->Buffer; //微信号
			wchar_t* wcpWxName = pWxName->Buffer; //微信号
			OutputDebugStringW(wcWxid);
			//wstring wstrWxnumber = pWxNumber->Buffer; //微信号
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
// 获取所有联系人
BOOL GetFriendList3()
{
	ListView_DeleteAllItems(gHwndList);

	PRAW_WXNODE pFriendNodes = (PRAW_WXNODE)WXADDR_ALL_USERS;

	// 存储所有好友链表节点的首地址
	DWORD_LIST listFriendAddr;

	// 遍历所有好友的节点地址
	TraversalWxNode(pFriendNodes->u1.pStartNode, listFriendAddr,
		pFriendNodes->u1.dwStart, pFriendNodes->dwTotal);

	DWORD_LIST::iterator iter;
	for (iter = listFriendAddr.begin(); iter != listFriendAddr.end(); iter++)
	{
		DWORD dwAddr = *iter;
		DWORD usersListData = dwAddr + 0x28;

		PWXUNICODE_STRING pwxid = (PWXUNICODE_STRING)(usersListData + 0x8); //微信id
		PWXUNICODE_STRING pWxNumber = (PWXUNICODE_STRING)(usersListData + 0x1c); //微信号
		PWXUNICODE_STRING pWxV1 = (PWXUNICODE_STRING)(usersListData + 0x30); //wxV1
		PWXUNICODE_STRING pWxRemarks = (PWXUNICODE_STRING)(usersListData + 0x50);//备注
		PWXUNICODE_STRING pWxName = (PWXUNICODE_STRING)(usersListData + 0x64); //微信名称
		PWXUNICODE_STRING headMaxImg = (PWXUNICODE_STRING)(usersListData + 0x108);//大图
		PWXUNICODE_STRING headMinImg = (PWXUNICODE_STRING)(usersListData + 0xF4);//小图
		//pWxNumber->Buffer 就是字付串
		if (pwxid->Length > 0) {


			wchar_t* wcWxid = pwxid->Buffer; //微信号
			wchar_t* wcWxNumber = pWxNumber->Buffer; //微信号
			wchar_t* wcWxRemarks = pWxRemarks->Buffer; //微信号
			wchar_t* wcpWxName = pWxName->Buffer; //微信号
			
			//wstring wstrWxnumber = pWxNumber->Buffer; //微信号
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

	// 存储所有好友链表节点的首地址
	DWORD_LIST listFriendAddr;

	// 遍历所有好友的节点地址
	TraversalWxNode(pFriendNodes->u1.pStartNode, listFriendAddr,
		pFriendNodes->u1.dwStart, pFriendNodes->dwTotal);

	DWORD_LIST::iterator iter;
	for (iter = listFriendAddr.begin(); iter != listFriendAddr.end(); iter++)
	{
		DWORD dwAddr = *iter;
		DWORD usersListData = dwAddr + 0x28;

		PWXUNICODE_STRING pwxid = (PWXUNICODE_STRING)(usersListData + 0x8); //微信id
		PWXUNICODE_STRING pWxNumber = (PWXUNICODE_STRING)(usersListData + 0x1c); //微信号
		PWXUNICODE_STRING pWxV1 = (PWXUNICODE_STRING)(usersListData + 0x30); //wxV1
		PWXUNICODE_STRING pWxRemarks = (PWXUNICODE_STRING)(usersListData + 0x50);//备注
		PWXUNICODE_STRING pWxName = (PWXUNICODE_STRING)(usersListData + 0x64); //微信名称
		PWXUNICODE_STRING headMaxImg = (PWXUNICODE_STRING)(usersListData + 0x108);//大图
		PWXUNICODE_STRING headMinImg = (PWXUNICODE_STRING)(usersListData + 0xF4);//小图
		//pWxNumber->Buffer 就是字付串
		wchar_t messagetemp[0x300] = { 0 };
		GetDlgItemText(hwndDlg, MESSAGE6, messagetemp, sizeof(messagetemp));
		string  temp;
		Wchar_tToString(temp, messagetemp);
		if (pwxid->Length > 0) {


			wchar_t* wcWxid = pwxid->Buffer; //微信号
			wchar_t* wcWxNumber = pWxNumber->Buffer; //微信号
			wchar_t* wcWxRemarks = pWxRemarks->Buffer; //微信号
			wchar_t* wcpWxName = pWxName->Buffer; //微信号

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