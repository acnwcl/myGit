#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
#include <shellapi.h>
#include <WinError.h>
#include <ShlObj.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

#include <atlbase.h>
#include <atlapp.h>
#include <atlstr.h>

//wtl

#include <atlmisc.h>
#include <TCHAR.h>
#include <atlctrls.h>
#include <atlctrlw.h>
