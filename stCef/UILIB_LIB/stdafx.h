// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
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

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����


// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
