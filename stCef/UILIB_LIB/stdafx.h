// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#define _CRT_SECURE_NO_DEPRECATE

#include "UIlib.h"

#include <olectl.h>
#include <zmouse.h>

#define lengthof(x) (sizeof(x)/sizeof(*x))
#define MAX max
#define MIN min
#define CLAMP(x,a,b) (MIN(b,MAX(a,x)))

#include "targetver.h"
#include <atlbase.h>
#include <atlconv.h>
#include <mshtmhst.h>
#include <ExDisp.h>
#include <ExDispId.h>
#include <Mshtml.h>
#include <comutil.h>
#include "resource.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料


// TODO: 在此处引用程序需要的其他头文件
