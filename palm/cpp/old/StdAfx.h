// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__26E80884_7F85_4781_BA95_E48E824F0B1A__INCLUDED_)
#define AFX_STDAFX_H__26E80884_7F85_4781_BA95_E48E824F0B1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

#include "const.h"


char *read_string(ifstream &ifs, char *str, int allocate_memory);
int skip_new_line(char *str);

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__26E80884_7F85_4781_BA95_E48E824F0B1A__INCLUDED_)
