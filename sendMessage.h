#pragma once
#include "framework.h"
#include <string>


VOID SendWXIDTextMessage(wchar_t* wxid, wchar_t* message);
VOID Wchar_tToString(std::string& szDst, wchar_t* wchar);  //�ַ�ת�ͷ���
VOID Smessage2(HWND hwndDlg);
VOID Smessage3(HWND hwndDlg);
VOID Smessage4(HWND hwndDlg);
VOID Smessage5(HWND hwndDlg);
