#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>
#include <shellapi.h>
#include <WinError.h>
#include <ShlObj.h>

// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

#include <atlbase.h>
#include <atlapp.h>
#include <atlstr.h>

//wtl

#include <atlmisc.h>
#include <TCHAR.h>
#include <atlctrls.h>
#include <atlctrlw.h>
