#include "pch.h"
#include "getchatroomuser.h"
#include "resource.h"
#include <string>
#include <cstring>
using namespace std;
class TEXT_WX     //聊天室结构体
{
public:
	wchar_t* pWxid = nullptr;
	DWORD length = 0;
	DWORD maxLength = 0;
	DWORD fill1 = 0;
	DWORD fill2 = 0;
	wchar_t wxid[1024] = { 0 };

	TEXT_WX(wstring wsWxid)
	{
		const wchar_t* temp = wsWxid.c_str();
		wmemcpy(wxid, temp, wsWxid.length());
		length = wsWxid.length();
		maxLength = wsWxid.capacity();
		fill1 = 0;
		fill2 = 0;
		pWxid = wxid;
	}
};

struct userInfo {        //群列表联系人结构体
	wchar_t wxid[0x100];
	wchar_t username[0x100];
	wchar_t nick[0x100];
	wchar_t v1[0x100];
};
struct wxStr {         //wxid结构体
	wchar_t* pStr;
	int strLen;
	int strMaxLen;
	int fill1 = 0;
	int fill2 = 0;
};


userInfo getChatromList(wchar_t* wxid);    // 获取群联系人方法
list<wstring> ChatroomUserList(1);         //群联系人列表
HWND ChatroomView = NULL;					//输出列表句柄
LPCWSTR GetMsgByAddress(DWORD memAddress);   //获取信息方法
VOID AddList(DWORD res_eax);				//添加联系人操作

VOID DoAction(HWND hListView2,HWND hwndDlgtemp)
{
	ChatroomView = hListView2;
	int len = 100;
	HWND hiew = GetDlgItem(hwndDlgtemp, USER_LISTS);
	TCHAR wstrText[4][128] = { 0 };
	int pos = 1;
	wchar_t wxid[0x100] = { 0 };
	while (pos) {
		pos = ListView_GetNextItem(hiew, pos, LVNI_ALL);
		int bo = ListView_GetItemState(hiew, pos, LVIS_SELECTED);
		if (bo != 0) {
			ListView_GetItemText(hiew, pos, 0, wstrText[0], sizeof(wstrText[0]));
			swprintf_s(wxid, L"%s", wstrText);
			break;
		}
	}


	//122A8258  13A9A3F0  UNICODE "5847657683@chatroom"
	//群聊微信ID
	/*
	WCHAR wxid[50];
	HWND hROOMID = GetDlgItem(hwndDlgtemp, IDC_ROOMID);
	GetWindowText(hROOMID, wxid, 50);
	*/
	TEXT_WX wxRoomId(wxid);

	//获取微信基址
	HMODULE dllAdress = GetModuleHandleA("WeChatWin.dll");

	DWORD call1 = (DWORD)dllAdress + 0x40C0F0;
	DWORD call2 = (DWORD)dllAdress + 0x2AD830;
	DWORD call3 = (DWORD)dllAdress + 0x414990;
	DWORD call4 = (DWORD)dllAdress + 0x40CBD0;
	//返回结果
	DWORD res_eax = 0;

	//缓冲区
	//[local.93]==>[ebp-00000174]
	char userListBuff[0x174] = { 0 };

	DWORD myESP = 0;

	//获取群用户
	__asm
	{
		//5C64FBD5  |> \8D8D 9CFEFFFF lea ecx,[local.89]
		//5C64FBDB  |.  E8 50542100   call WeChatWi.5C865030
		//5C64FBE0  |.  C745 FC 01000>mov [local.1],0x1
		//5C64FBE7  |.  C785 8CFEFFFF>mov [local.93],0x0
		//5C64FBF1  |.  C785 90FEFFFF>mov [local.92],0x0
		//5C64FBFB  |.  C785 94FEFFFF>mov [local.91],0x0
		//5C64FC05  |.  8D85 9CFEFFFF lea eax,[local.89]
		//5C64FC0B  |.  C645 FC 02    mov byte ptr ss:[ebp-0x4],0x2
		//5C64FC0F  |.  50            push eax
		//5C64FC10  |.  53            push ebx
		//5C64FC11  |.  E8 6A720B00   call WeChatWi.5C706E80
		//5C64FC16  |.  8BC8          mov ecx,eax
		//5C64FC18  |.  E8 D3DB2100   call WeChatWi.5C86D7F0                   ;  数据构造完成
		//5C64FC1D  |.  8D85 8CFEFFFF lea eax,[local.93]                       ;  数据生成
		//5C64FC23  |.  50            push eax
		//5C64FC24  |.  8D8D 9CFEFFFF lea ecx,[local.89]
		//5C64FC2A  |.  E8 F15E2100   call WeChatWi.5C865B20
		//5C64FC2F  |.  83EC 0C       sub esp,0xC
		//5C64FC32  |.  8D85 8CFEFFFF lea eax,[local.93]                       ;  数据生成


		//###########################################################################################
		mov myESP, esp

		//WeChatWin.dll+1FFBD5 - 8D 8D 9CFEFFFF        - lea ecx,[ebp-00000164]
		lea ecx, userListBuff[0x174 - 0x164]

		//第一个调用
		//WeChatWin.dll+1FFBDB - E8 50542100           - call WeChatWin.dll+415030
		call call1

		//WeChatWin.dll+1FFBE0 - C7 45 FC 01000000     - mov [ebp-04],00000001 { 1 }
		mov userListBuff[0x174 - 0x4], 1

		//WeChatWin.dll+1FFBE7 - C7 85 8CFEFFFF 00000000 - mov [ebp-00000174],00000000 { 0 }
		//WeChatWin.dll+1FFBF1 - C7 85 90FEFFFF 00000000 - mov [ebp-00000170],00000000 { 0 }
		//WeChatWin.dll+1FFBFB - C7 85 94FEFFFF 00000000 - mov [ebp-0000016C],00000000 { 0 }
		//WeChatWin.dll+1FFC05 - 8D 85 9CFEFFFF        - lea eax,[ebp-00000164]
		lea eax, userListBuff[0x174 - 0x164]

		//WeChatWin.dll+1FFC0B - C6 45 FC 02           - mov byte ptr [ebp-04],02 { 2 }
		mov userListBuff[0x174 - 0x4], 2

		//WeChatWin.dll+1FFC0F - 50                    - push eax
		push eax

		//WeChatWin.dll+1FFC10 - 53                    - push ebx
		lea ebx, wxRoomId
		push ebx

		//第二个调用
		//WeChatWin.dll+1FFC11 - E8 6A720B00           - call WeChatWin.dll+2B6E80
		call call2

		//WeChatWin.dll+1FFC16 - 8B C8                 - mov ecx,eax
		mov ecx, eax

		//第三个调用
		//WeChatWin.dll+1FFC18 - E8 D3DB2100           - call WeChatWin.dll+41D7F0
		call call3

		//WeChatWin.dll+1FFC1D - 8D 85 8CFEFFFF        - lea eax,[ebp-00000174]
		lea eax, userListBuff[0]

		//WeChatWin.dll+1FFC23 - 50                    - push eax
		push eax

		//WeChatWin.dll+1FFC24 - 8D 8D 9CFEFFFF        - lea ecx,[ebp-00000164]
		lea ecx, userListBuff[0x174 - 0x164]

		//第四个调用
		//WeChatWin.dll+1FFC2A - E8 F15E2100           - call WeChatWin.dll+415B20
		call call4

		//---------------------------------------------提取寄存器数据---------------------------------
		mov res_eax, eax
		//---------------------------------------------提取寄存器数据---------------------------------

		//WeChatWin.dll+1FFC2F - 83 EC 0C              - sub esp,0C { 12 }
		//sub esp, 0x0C

		//恢复esp寄存器
		mov eax, myESP
		mov esp, eax
		//###########################################################################################
	}

	AddList(res_eax);

}
VOID AddList(DWORD res_eax)
{
	DWORD userList = *((DWORD*)res_eax);

	//微信群成员数量
	int userCount = *((int*)(res_eax + 0xB0));

	//列表清空
	ListView_DeleteAllItems(ChatroomView);

	//清空内存中联系人列表
	ChatroomUserList.clear();


	for (int i = 0; i < userCount; i++)
	{
		wstring wxid = GetMsgByAddress(userList + (i * 20));
		ChatroomUserList.push_back(wxid);
	}

	//联系人排序
	ChatroomUserList.sort();


	//添加数据项
	DWORD index = 0;
	LVITEM item = { 0 };
	item.mask = LVIF_TEXT | LVCF_FMT | LVCF_WIDTH;
	for (auto& wxid : ChatroomUserList)
	{
		userInfo user = getChatromList((wchar_t*)wxid.c_str());

		LVITEM item = { 0 };
		item.mask = LVIF_TEXT;

		item.iSubItem = 0;
		item.pszText = (LPWSTR)(user.wxid);
		ListView_InsertItem(ChatroomView, &item);

		item.iSubItem = 1;
		item.pszText = (LPWSTR)(user.username);
		ListView_SetItem(ChatroomView, &item);

		item.iSubItem = 2;
		item.pszText = (LPWSTR)(user.nick);
		ListView_SetItem(ChatroomView, &item);

	}
	MessageBox(NULL, L"联系人列表获取成功", L"aaa", 0);
}
userInfo getChatromList(wchar_t* wxid) {
	userInfo user = { 0 };
	DWORD WechatBase = (DWORD)GetModuleHandleA("WeChatWin.dll");
	DWORD callAdd = WechatBase + 0x266D30;
	DWORD callAdd1 = WechatBase + 0x4BE50;
	DWORD callAdd2 = WechatBase + 0x47AE60;
	char buff[0xBF8] = { 0 };
	char* asmBuff = buff;
	wxStr pwxid = { 0 };
	pwxid.pStr = wxid;
	pwxid.strLen = wcslen(wxid);
	pwxid.strMaxLen = wcslen(wxid) * 2;
	char* asmwxid = (char*)&pwxid.pStr;

	__asm {
		mov edi, asmBuff
		push edi
		sub esp, 0x14
		mov eax, asmwxid
		mov ecx, esp
		push eax
		call callAdd2
		call callAdd1
		call callAdd
	}
	LPVOID textWxid = *((LPVOID*)((DWORD)buff + 0x8));
	LPVOID textUser = *((LPVOID*)((DWORD)buff + 0x1C));
	LPVOID textNick = *((LPVOID*)((DWORD)buff + 0x64));
	LPVOID textV1 = *((LPVOID*)((DWORD)buff + 0x30));
	swprintf_s(user.wxid, L"%s", textWxid);
	swprintf_s(user.username, L"%s", textUser);
	swprintf_s(user.nick, L"%s", textNick);
	swprintf_s(user.v1, L"%s", textV1);
	return user;
}
LPCWSTR GetMsgByAddress(DWORD memAddress)
{
	//获取字符串长度
	DWORD msgLength = *(DWORD*)(memAddress + 4);
	DWORD msgMaxLength = *(DWORD*)(memAddress + 4);
	if (msgLength == 0 || msgLength > msgMaxLength)
	{
		WCHAR* msg = new WCHAR[1];
		msg[0] = 0;
		return msg;
	}

	WCHAR* msg = new WCHAR[msgLength + 1];
	ZeroMemory(msg, msgLength + 1);

	//复制内容
	wmemcpy_s(msg, msgLength + 1, (WCHAR*)(*(DWORD*)memAddress), msgLength + 1);
	return msg;
}



