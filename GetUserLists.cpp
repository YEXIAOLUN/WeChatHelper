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
	MessageBox(NULL, L"联系人列表获取成功", L"aaa", 0);
}
*/

//跳转过来的函数 我们自己的
/*
5BBB989D  |.  E8 5EE8FFFF   call WeChatWi.5BBB8100   418100                           ;  这里也可以获取到数
5BBB98A2  |.  84C0          test al,al                                                ;  edi是数据
51280
*/
VOID __declspec(naked) HookF()
{
	//pushad: 将所有的32位通用寄存器压入堆栈
	//pushfd:然后将32位标志寄存器EFLAGS压入堆栈
	//popad:将所有的32位通用寄存器取出堆栈
	//popfd:将32位标志寄存器EFLAGS取出堆栈
	//先保存寄存器
	// 使用pushad这些来搞还是不太稳定  还是用变量把寄存器的值保存下来 这样可靠点
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
	
	//然后跳转到我们自己的处理函数 想干嘛干嘛
	insertUserLists();
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)insertUserLists, NULL, 0, NULL);
	//然后在还原他进来之前的所有数据
	/*popad
		popfd  不太可靠恢复不全 所以才有变量的方式保存下来再赋值过去*/
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

		//补充被覆盖的代码
		call overWritedCallAdd

		//跳回被HOOK指令的下一条指令
		jmp retAdd
	}
	*/
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
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartHook, NULL, 0, NULL);
	//StartHook(WinAdd + HookAdd, &HookF);
}	

