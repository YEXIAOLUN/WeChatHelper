#pragma once

VOID HookWechatQrcode(HWND hwndDlg, HWND hwndList, DWORD HookAdd);
char* wchar2char(const wchar_t* wchar);
BOOL GetFriendList();
BOOL GetFriendList2();
BOOL GetFriendList3();
BOOL GetFriendList4(HWND hwndDlg);